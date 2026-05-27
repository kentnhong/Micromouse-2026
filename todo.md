# Micromouse 2026 — Known Issues & TODO

## CRITICAL (blocks correct movement)

- [ ] **PWM frequency is 2× actual**
  - `kPwmFreqHz = 323` in `f411_board.cc:28`
  - Only `pwm1_left.set_frequency()` is called; `pwm2_left`, `pwm1_right`, `pwm2_right` are never given a frequency — they inherit whatever the timer defaults to
  - All four PWM channels on TIM2 share one ARR register, so calling `set_frequency` on any one channel sets all four, but this should be verified
  - Confirm that the actual output frequency matches the intended 323 Hz (scope or logic analyser); if the prescaler math doubles it, fix the calculation in `StPwm`

- [ ] **ADC sequence array has only 1 slot for 4 IR channels**
  - `std::array<uint8_t, 1> adc_seq{9}` in `f411_board.cc:130`
  - Only channel 9 (LEFT sensor) is in the scan sequence; channels 8, 10, 11 are not
  - DMA transfers one conversion then stops — FRONT_LEFT, FRONT_RIGHT, RIGHT IR sensors always read stale / zero
  - Fix: expand `adc_seq` to `{9, 8, 11, 10}` and make sure ADC scan length and DMA buffer length match (4 entries)

## HIGH (movement quality)

- [ ] **Left and right motors have different torque characteristics and likely different effective gear ratios — need hardware testing and independent PID tuning**
  - Root cause: motors are not matched units; one has higher back-EMF, different winding resistance, or slightly different gear mesh efficiency — they do not produce the same torque at the same PWM duty
  - Symptom: robot drifts consistently right; `right_speed_scale = 1.0f` is a setpoint scalar, not a physical fix — it hides the mismatch at cruise speed but the motors still diverge during acceleration and deceleration transients
  - Hardware test to run:
    1. Command both motors to the same fixed PWM duty (no PID), log left and right encoder ticks over 2 seconds via serial — note the ratio (e.g. right runs 8% faster → `right_speed_scale ≈ 0.92`)
    2. Enable PID with same gains on both sides, command 300 ticks/s setpoint, log actual speed of each motor separately — observe settling time and steady-state error per side
    3. If settling times differ significantly, the motors need separate PID gain sets (`Gains left_gains`, `Gains right_gains`) rather than one shared `Gains` in `MotionControllerParams`
  - Fix plan:
    - Split `MotionControllerParams.gains` into `gains_left` and `gains_right`
    - Tune left and right independently: start with the same kp/ki, then adjust whichever side oscillates or lags
    - Set `right_speed_scale` from the open-loop PWM ratio test above as a baseline before PID runs
    - Verify on a straight 5-cell run: both encoder counts should match within ±5 ticks at the end

- [ ] **Turn angle not matching 90° / 180° physically**
  - `kWheelbaseMm = 57.5f` and `k90DegTurnTicks` derived from it — these are estimates
  - Symptom: robot consistently over- or under-shoots turns, clips walls on exit
  - Fix: run 4× left-turn square test, measure final heading error, adjust `kWheelbaseMm` or tune `k90DegTurnTicks` directly; `k180DegTurnTicks = k90DegTurnTicks * 2` is correct by construction

- [ ] **Cell distance (`kCellDistanceMm`) under-travels**
  - Currently `47.5 mm` — this is the travel distance after a turn settle, not a full 180 mm cell
  - Symptom: robot stops short, or relies entirely on wall-edge detection to find the correct position
  - Fix: measure actual wheel-to-wheel displacement per move, tune `kCellDistanceMm` to match; verify `kMmPerTick` matches real wheel circumference and gear ratio

- [ ] **PID gains not validated on real hardware**
  - `kp = 0.001f`, `ki = 0.03f`, `kd = 0.0f`
  - `max_output = 0.18f` means P saturates immediately for any velocity error > 180 ticks/s; integral does all the work in steady state
  - Fix: log setpoint vs actual speed over serial, verify integral settles within 1–2 cell lengths; if oscillation occurs lower `ki`; if it's sluggish raise `kp` slightly

- [ ] **Wall centering only kicks in when `ir_left_smooth > kWallSideThreshold`**
  - Threshold is `3667`; normal wall reading is ~3840; open corridor ~3200
  - If robot is far from the wall (sensor reads 3400–3666) centering is completely off
  - Consider lowering `kWallSideThreshold` to ~3400 or switching to a continuous correction (always apply centering, just scale the gain by confidence)

## MEDIUM (reliability)

- [ ] **Wall-edge detection fires on noise / brief IR dropout**
  - A single noisy reading can flip `left_wall` or `right_wall` from true→false and stop the robot mid-corridor
  - Fix: require N consecutive below-threshold readings (debounce counter) before declaring "wall gone"

- [ ] **Trapezoidal profile uses average of both encoder wheels for turn progress**
  - `profile.trapezoidal(EncoderInput{rev_ticks, fwd_ticks}, dt)` — averaging works only if both motors spin at the same rate
  - If one wheel slips or the motors are mismatched the profile finishes at the wrong angle
  - Fix: use a single wheel (or the minimum of the two) as the progress signal, or add a gyro-based override

- [ ] **Front wall threshold hardcoded instead of calibrated**
  - `kFrontWallThreshold = 4000` (just fixed from 3667)
  - The correct value depends on sensor mounting, emitter power, and maze wall reflectivity
  - Fix: add a serial command that prints live front IR values so the threshold can be tuned without reflashing

- [ ] **No timeout / watchdog on `forward()` loop in `main.cc`**
  - If wall-edge detection never fires and the profile never completes (encoder fault, stall), the `while (!hw.motion.forward(...))` loop spins forever
  - Fix: add a tick-count or millisecond timeout that calls `hw.motion.stop()` and breaks out

## LOW / FUTURE

- [ ] **`ir_left_smooth` / `ir_right_smooth` not reset between forward calls**
  - Residual smooth state from the previous corridor can bias centering at the start of the next move
  - Fix: reset all IR smooth state in the `MoveState::FORWARD` init block (same place `ir_front_*_smooth` is already reset)

- [ ] **`g_adc_ovr` is a bare `volatile bool` shared between ISR and main loop**
  - On Cortex-M4 a `volatile` bool is sufficient for single-bit flag, but consider using `std::atomic<bool>` for clarity and future portability

- [ ] **Floodfill solver not yet connected to MotionController**
  - `common/core/algorithm/floodfill.cc` exists but is not called from any app
  - Once motion is reliable, wire up the solver: floodfill → direction decisions → `motion.forward()` / `motion.turn_*()` calls

- [ ] **HC-05 Bluetooth transceiver not tested**
  - Hardware is present (mouse side), but end-to-end link from mouse to receiver has never been validated
  - Need to confirm USART baud rate matches HC-05 default (38400 or 9600), pair the modules, and verify that serial print output (`SEQ_START`, `F_DONE`, etc.) arrives on the receiver side
  - Useful for live PID/IR tuning without a USB cable attached

- [ ] **BNO055 IMU not integrated**
  - Driver exists at `common/core/periph/bno055/` but is not used in any motion loop
  - Plan: use gyro Z-axis to measure actual heading change during turns; compare against encoder-derived angle; use the difference to trim `kWheelbaseMm` or close the loop in real time
  - Also useful for detecting stalls (acceleration spike with no encoder change) and for re-centering heading after a slip
  - Requires I2C init in BSP and a periodic `bno055.update()` call at the same rate as the motion loop

- [ ] **`right_speed_scale` applied to correction term too**
  - In `forward()`: `pid_right.update((base_speed + correction) * right_speed_scale, ...)`
  - This scales the correction by `right_speed_scale`, changing the effective centering gain for the right motor
  - Fix: apply `right_speed_scale` only to `base_speed`: `pid_right.update(base_speed * right_speed_scale + correction, ...)`
