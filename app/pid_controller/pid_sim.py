"""
Interactive velocity PID simulator for the micromouse motor controller.

Run from the repo root:
    python app/pid_controller/pid_sim.py

The PID math mirrors common/core/control/pid/pid.cc. The model is a
small first-order approximation of a motor/wheel/encoder so tuning changes can
be seen immediately without firmware or hardware.
"""

from __future__ import annotations

import math
import tkinter as tk
from dataclasses import dataclass
from tkinter import ttk


WHEEL_DIAMETER_MM = 14.0
GEAR_RATIO = 15.25
TICKS_PER_MOTOR_REV = 12.0
MM_PER_TICK = (WHEEL_DIAMETER_MM * math.pi) / (GEAR_RATIO * TICKS_PER_MOTOR_REV)
MAX_PLOT_POINTS = 700
SLIDER_UPDATE_DELAY_MS = 35


@dataclass
class SimConfig:
    kp: float = 0.001
    ki: float = 0.0
    kd: float = 0.0
    target_mm_s: float = 100.0
    duration_s: float = 3.0
    dt_ms: float = 1.0
    max_speed_mm_s: float = 220.0
    response_ms: float = 80.0
    friction: float = 0.04
    output_limit: float = 1.0
    integral_limit: float = 1000.0
    disturbance_s: float = 1.2
    disturbance_pct: float = 30.0


@dataclass
class SimSeries:
    time: list[float]
    target_speed: list[float]
    speed: list[float]
    output: list[float]
    error: list[float]
    integral: list[float]
    measured_ticks: list[int]
    final_speed_mm_s: float
    overshoot_pct: float
    settle_time_s: float | None
    max_output: float


class PidModel:
    def __init__(self, kp: float, ki: float, kd: float, output_limit: float, integral_limit: float) -> None:
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.min_output = -abs(output_limit)
        self.max_output = abs(output_limit)
        self.integral_limit = abs(integral_limit)
        self.integral = 0.0
        self.prev_error = 0.0

    @staticmethod
    def limit_range(value: float, minimum: float, maximum: float) -> float:
        return max(minimum, min(maximum, value))

    def update(self, desired_speed_ticks: float, measured_ticks: int, dt_sec: float) -> float:
        if dt_sec <= 0.0:
            return 0.0

        error = desired_speed_ticks - (measured_ticks / dt_sec)
        p_term = self.kp * error

        self.integral += error * dt_sec
        self.integral = self.limit_range(self.integral, -self.integral_limit, self.integral_limit)
        i_term = self.ki * self.integral

        derivative = (error - self.prev_error) / dt_sec
        d_term = self.kd * derivative
        self.prev_error = error

        return self.limit_range(p_term + i_term + d_term, self.min_output, self.max_output)


def simulate(config: SimConfig) -> SimSeries:
    dt_sec = max(config.dt_ms / 1000.0, 0.0001)
    steps = max(2, int(config.duration_s / dt_sec))
    target_ticks_per_sec = config.target_mm_s / MM_PER_TICK
    pid = PidModel(config.kp, config.ki, config.kd, config.output_limit, config.integral_limit)

    speed_mm_s = 0.0
    fractional_ticks = 0.0

    time: list[float] = []
    target_speed: list[float] = []
    speed: list[float] = []
    output: list[float] = []
    error: list[float] = []
    integral: list[float] = []
    measured_ticks: list[int] = []

    for step in range(steps + 1):
        now = step * dt_sec

        ticks_float = (speed_mm_s / MM_PER_TICK) * dt_sec + fractional_ticks
        ticks_sample = int(ticks_float)
        fractional_ticks = ticks_float - ticks_sample

        drive = pid.update(target_ticks_per_sec, ticks_sample, dt_sec)

        disturbance = 0.0
        if config.disturbance_pct > 0.0 and config.disturbance_s <= now < config.disturbance_s + 0.18:
            disturbance = config.disturbance_pct / 100.0

        commanded_speed = drive * config.max_speed_mm_s
        tau = max(config.response_ms / 1000.0, dt_sec)
        accel = (commanded_speed - speed_mm_s) / tau
        drag = config.friction * speed_mm_s
        speed_mm_s += (accel - drag - disturbance * config.max_speed_mm_s) * dt_sec

        # The real controller can reverse if the PID output goes negative.
        speed_mm_s = max(-config.max_speed_mm_s, min(config.max_speed_mm_s, speed_mm_s))

        measured_speed_ticks = ticks_sample / dt_sec
        time.append(now)
        target_speed.append(config.target_mm_s)
        speed.append(speed_mm_s)
        output.append(drive)
        error.append(target_ticks_per_sec - measured_speed_ticks)
        integral.append(pid.integral)
        measured_ticks.append(ticks_sample)

    abs_target = abs(config.target_mm_s)
    peak_speed = max(speed) if config.target_mm_s >= 0 else min(speed)
    overshoot_pct = 0.0
    if abs_target > 0.001:
        overshoot_pct = max(0.0, (peak_speed - config.target_mm_s) / abs_target * 100.0)

    settle_time_s: float | None = None
    tolerance = max(abs_target * 0.05, 1.0)
    settled_from_index = len(speed) - 1
    for index in range(len(speed) - 1, -1, -1):
        if abs(speed[index] - config.target_mm_s) <= tolerance:
            settled_from_index = index
        else:
            break
    if settled_from_index < len(speed) - 1:
        settle_time_s = time[settled_from_index]

    return SimSeries(
        time=time,
        target_speed=target_speed,
        speed=speed,
        output=output,
        error=error,
        integral=integral,
        measured_ticks=measured_ticks,
        final_speed_mm_s=speed[-1],
        overshoot_pct=overshoot_pct,
        settle_time_s=settle_time_s,
        max_output=max(abs(v) for v in output),
    )


class PlotCanvas(tk.Canvas):
    def __init__(self, parent: tk.Widget) -> None:
        super().__init__(parent, bg="#f8fafc", highlightthickness=0)
        self.series: SimSeries | None = None
        self.bind("<Configure>", lambda _event: self.redraw())

    def set_series(self, series: SimSeries) -> None:
        self.series = series
        self.redraw()

    def redraw(self) -> None:
        self.delete("all")
        if self.series is None:
            return

        width = max(self.winfo_width(), 400)
        height = max(self.winfo_height(), 260)
        pad_left = 58
        pad_right = 26
        pad_top = 24
        pad_bottom = 42
        plot_w = width - pad_left - pad_right
        plot_h = height - pad_top - pad_bottom

        s = self.series
        x_min = 0.0
        x_max = max(s.time[-1], 0.001)
        y_min = min(-10.0, min(s.speed) * 1.1, min(s.target_speed) * 1.1)
        y_max = max(10.0, max(s.speed) * 1.1, max(s.target_speed) * 1.1)
        if math.isclose(y_min, y_max):
            y_min -= 1.0
            y_max += 1.0

        def x_px(value: float) -> float:
            return pad_left + (value - x_min) / (x_max - x_min) * plot_w

        def y_px(value: float) -> float:
            return pad_top + (y_max - value) / (y_max - y_min) * plot_h

        self.create_rectangle(pad_left, pad_top, width - pad_right, height - pad_bottom, fill="#ffffff", outline="#cbd5e1")

        for i in range(6):
            frac = i / 5
            y = pad_top + frac * plot_h
            value = y_max - frac * (y_max - y_min)
            self.create_line(pad_left, y, width - pad_right, y, fill="#e2e8f0")
            self.create_text(pad_left - 10, y, text=f"{value:0.0f}", anchor="e", fill="#475569", font=("Segoe UI", 9))

        for i in range(7):
            frac = i / 6
            x = pad_left + frac * plot_w
            value = x_min + frac * (x_max - x_min)
            self.create_line(x, pad_top, x, height - pad_bottom, fill="#f1f5f9")
            self.create_text(x, height - pad_bottom + 18, text=f"{value:0.1f}s", anchor="n", fill="#475569", font=("Segoe UI", 9))

        self._draw_line(s.time, s.target_speed, x_px, y_px, "#64748b", width=2, dash=(5, 4))
        self._draw_line(s.time, s.speed, x_px, y_px, "#0f766e", width=3)

        zero_output_y = height - 22
        self.create_line(pad_left, zero_output_y, width - pad_right, zero_output_y, fill="#cbd5e1")
        output_scale = 18
        output_points = self._sample_points(s.time, s.output, x_px, lambda out: zero_output_y - out * output_scale)
        if len(output_points) > 1:
            self.create_line(output_points, fill="#ea580c", width=2)
        self.create_text(width - pad_right, zero_output_y - output_scale - 2, text="PWM output", anchor="e", fill="#ea580c", font=("Segoe UI", 9))

        self.create_text(pad_left, 10, text="Velocity response (mm/s)", anchor="w", fill="#0f172a", font=("Segoe UI", 11, "bold"))
        self.create_text(width - pad_right, 10, text="target dashed, measured solid", anchor="e", fill="#64748b", font=("Segoe UI", 9))

    def _draw_line(
        self,
        xs: list[float],
        ys: list[float],
        x_px,
        y_px,
        color: str,
        width: int,
        dash: tuple[int, int] | None = None,
    ) -> None:
        points = self._sample_points(xs, ys, x_px, y_px)
        if len(points) > 1:
            self.create_line(points, fill=color, width=width, dash=dash)

    def _sample_points(self, xs: list[float], ys: list[float], x_px, y_px) -> list[tuple[float, float]]:
        stride = max(1, len(xs) // MAX_PLOT_POINTS)
        points = [(x_px(x), y_px(y)) for x, y in zip(xs[::stride], ys[::stride])]
        if xs and (points[-1][0] != x_px(xs[-1]) or points[-1][1] != y_px(ys[-1])):
            points.append((x_px(xs[-1]), y_px(ys[-1])))
        return points


class SliderRow(ttk.Frame):
    def __init__(
        self,
        parent: tk.Widget,
        label: str,
        minimum: float,
        maximum: float,
        initial: float,
        resolution: float,
        command,
    ) -> None:
        super().__init__(parent)
        self.command = command
        self.resolution = resolution
        self.value = tk.DoubleVar(value=initial)
        self.label = ttk.Label(self, text=label, width=17)
        self.scale = ttk.Scale(self, from_=minimum, to=maximum, variable=self.value, command=self._scale_changed)
        self.entry = ttk.Entry(self, width=9)
        self.entry.insert(0, self._format(initial))
        self.entry.bind("<Return>", self._entry_changed)
        self.entry.bind("<FocusOut>", self._entry_changed)

        self.label.grid(row=0, column=0, sticky="w", padx=(0, 8))
        self.scale.grid(row=0, column=1, sticky="ew")
        self.entry.grid(row=0, column=2, sticky="e", padx=(8, 0))
        self.columnconfigure(1, weight=1)

    def get(self) -> float:
        return float(self.value.get())

    def set(self, value: float) -> None:
        self.value.set(value)
        self._set_entry(value)

    def _scale_changed(self, _value: str) -> None:
        value = round(self.get() / self.resolution) * self.resolution
        self.value.set(value)
        self._set_entry(value)
        self.command()

    def _entry_changed(self, _event=None) -> None:
        try:
            value = float(self.entry.get())
        except ValueError:
            self._set_entry(self.get())
            return
        scale_min = float(self.scale.cget("from"))
        scale_max = float(self.scale.cget("to"))
        value = max(scale_min, min(scale_max, value))
        self.set(value)
        self.command()

    def _set_entry(self, value: float) -> None:
        self.entry.delete(0, tk.END)
        self.entry.insert(0, self._format(value))

    def _format(self, value: float) -> str:
        if self.resolution < 0.001:
            return f"{value:.5f}"
        if self.resolution < 0.01:
            return f"{value:.4f}"
        if self.resolution < 0.1:
            return f"{value:.2f}"
        return f"{value:.1f}"


class PidSimApp(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self._pending_update: str | None = None
        self.title("Micromouse PID Simulator")
        self.geometry("1120x720")
        self.minsize(900, 600)
        self.configure(bg="#e5e7eb")

        style = ttk.Style(self)
        style.theme_use("clam")
        style.configure("TFrame", background="#e5e7eb")
        style.configure("Panel.TFrame", background="#ffffff")
        style.configure("TLabel", background="#e5e7eb", foreground="#0f172a")
        style.configure("Panel.TLabel", background="#ffffff", foreground="#0f172a")
        style.configure("Metric.TLabel", background="#ffffff", foreground="#0f172a", font=("Segoe UI", 13, "bold"))
        style.configure("TButton", padding=(10, 6))

        root = ttk.Frame(self, padding=14)
        root.pack(fill=tk.BOTH, expand=True)
        root.columnconfigure(0, weight=3)
        root.columnconfigure(1, weight=2)
        root.rowconfigure(0, weight=1)

        left = ttk.Frame(root, style="Panel.TFrame", padding=12)
        left.grid(row=0, column=0, sticky="nsew", padx=(0, 12))
        left.rowconfigure(1, weight=1)
        left.columnconfigure(0, weight=1)

        title = ttk.Label(left, text="PID velocity tuning", style="Panel.TLabel", font=("Segoe UI", 16, "bold"))
        title.grid(row=0, column=0, sticky="w", pady=(0, 8))

        self.plot = PlotCanvas(left)
        self.plot.grid(row=1, column=0, sticky="nsew")

        self.metrics = ttk.Frame(left, style="Panel.TFrame")
        self.metrics.grid(row=2, column=0, sticky="ew", pady=(10, 0))
        for col in range(4):
            self.metrics.columnconfigure(col, weight=1)

        right = ttk.Frame(root, style="Panel.TFrame", padding=12)
        right.grid(row=0, column=1, sticky="nsew")
        right.columnconfigure(0, weight=1)

        controls_title = ttk.Label(right, text="Adjust values", style="Panel.TLabel", font=("Segoe UI", 14, "bold"))
        controls_title.grid(row=0, column=0, sticky="w", pady=(0, 8))

        self.rows: dict[str, SliderRow] = {}
        control_specs = [
            ("kp", "Kp", 0.0, 0.01, 0.001, 0.0001),
            ("ki", "Ki", 0.0, 0.02, 0.0, 0.0001),
            ("kd", "Kd", 0.0, 0.002, 0.0, 0.00001),
            ("target_mm_s", "Target mm/s", 0.0, 250.0, 100.0, 1.0),
            ("duration_s", "Duration s", 0.5, 8.0, 3.0, 0.1),
            ("dt_ms", "Sample dt ms", 0.5, 20.0, 1.0, 0.5),
            ("max_speed_mm_s", "Full PWM mm/s", 50.0, 500.0, 220.0, 5.0),
            ("response_ms", "Motor response ms", 10.0, 500.0, 80.0, 5.0),
            ("friction", "Friction", 0.0, 0.4, 0.04, 0.01),
            ("output_limit", "Output limit", 0.05, 1.0, 1.0, 0.05),
            ("integral_limit", "Integral limit", 0.0, 3000.0, 1000.0, 25.0),
            ("disturbance_s", "Disturbance time s", 0.0, 5.0, 1.2, 0.1),
            ("disturbance_pct", "Disturbance %", 0.0, 100.0, 30.0, 1.0),
        ]

        for row_index, spec in enumerate(control_specs, start=1):
            key, label, minimum, maximum, initial, resolution = spec
            row = SliderRow(right, label, minimum, maximum, initial, resolution, self.schedule_update)
            row.grid(row=row_index, column=0, sticky="ew", pady=4)
            self.rows[key] = row

        buttons = ttk.Frame(right, style="Panel.TFrame")
        buttons.grid(row=len(control_specs) + 1, column=0, sticky="ew", pady=(12, 0))
        buttons.columnconfigure(0, weight=1)
        buttons.columnconfigure(1, weight=1)
        ttk.Button(buttons, text="Reset firmware defaults", command=self.reset_defaults).grid(row=0, column=0, sticky="ew", padx=(0, 6))
        ttk.Button(buttons, text="Try stronger PID", command=self.try_stronger_pid).grid(row=0, column=1, sticky="ew", padx=(6, 0))

        note = (
            f"Encoder scale: {MM_PER_TICK:.4f} mm/tick. "
            "This is a visual tuning model, not a hardware-perfect motor model."
        )
        ttk.Label(right, text=note, style="Panel.TLabel", wraplength=380).grid(
            row=len(control_specs) + 2, column=0, sticky="ew", pady=(12, 0)
        )

        self.update_sim()

    def config_from_controls(self) -> SimConfig:
        return SimConfig(**{key: row.get() for key, row in self.rows.items()})

    def schedule_update(self) -> None:
        if self._pending_update is not None:
            self.after_cancel(self._pending_update)
        self._pending_update = self.after(SLIDER_UPDATE_DELAY_MS, self._run_scheduled_update)

    def _run_scheduled_update(self) -> None:
        self._pending_update = None
        self.update_sim()

    def update_sim(self) -> None:
        if self._pending_update is not None:
            self.after_cancel(self._pending_update)
            self._pending_update = None
        config = self.config_from_controls()
        if config.disturbance_s > config.duration_s:
            config.disturbance_s = config.duration_s
        series = simulate(config)
        self.plot.set_series(series)
        self.update_metrics(series)

    def update_metrics(self, series: SimSeries) -> None:
        for child in self.metrics.winfo_children():
            child.destroy()

        settle = "no settle" if series.settle_time_s is None else f"{series.settle_time_s:.2f}s"
        metrics = [
            ("Final speed", f"{series.final_speed_mm_s:.1f} mm/s"),
            ("Overshoot", f"{series.overshoot_pct:.1f}%"),
            ("Settle time", settle),
            ("Peak output", f"{series.max_output:.2f}"),
        ]
        for col, (label, value) in enumerate(metrics):
            frame = ttk.Frame(self.metrics, style="Panel.TFrame")
            frame.grid(row=0, column=col, sticky="ew", padx=(0 if col == 0 else 8, 0))
            ttk.Label(frame, text=label, style="Panel.TLabel", font=("Segoe UI", 9)).pack(anchor="w")
            ttk.Label(frame, text=value, style="Metric.TLabel").pack(anchor="w")

    def reset_defaults(self) -> None:
        defaults = SimConfig()
        for key, row in self.rows.items():
            row.set(getattr(defaults, key))
        self.update_sim()

    def try_stronger_pid(self) -> None:
        values = {
            "kp": 0.003,
            "ki": 0.0008,
            "kd": 0.00005,
            "integral_limit": 800.0,
            "response_ms": 90.0,
            "disturbance_pct": 25.0,
        }
        for key, value in values.items():
            self.rows[key].set(value)
        self.update_sim()


if __name__ == "__main__":
    app = PidSimApp()
    app.mainloop()
