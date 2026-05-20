# TODO - PID/Encoder debugging (sample_ticks stuck / PID not responding)

## Encoder + measurement validation
- [ ] Inspect `app/pid_controller/bsp_f411/f411_board.cc` for actual encoder TIM configuration.
- [ ] In debugger, break in `common/drivers/platform/stm32f4/st_encoder.cc:HwEncoder::get_ticks()` and record `base_addr->CNT` and computed `current_ticks` for multiple samples.
- [ ] Verify what `sample_ticks` equals at breakpoint in `common/core/control/sample/enc_sample.cc`.
- [ ] Check for wrap-around behavior (0..65535) if the timer CNT is 16-bit; confirm whether negative/constant deltas correlate with wrap.

## PID observability (P/I/D build-up)
- [ ] Add temporary debugger-visible variables in `common/core/control/pid/pid.cc` to expose:
  - [ ] `error`
  - [ ] `p = gains.kp * error`
  - [ ] `state.integral`
  - [ ] `i = gains.ki * state.integral`
  - [ ] `derivative = (error - state.prev_error)/dt_sec`
  - [ ] `d = gains.kd * derivative`
  - [ ] unclamped `p+i+d` and clamped `final_drive`

## Fix measurement logic
- [ ] If encoder returns wrong sign/scale due to CNT wrap or casting, change `HwEncoder::get_ticks()` to return **delta ticks** using modulo arithmetic and `prev_ticks`.
- [ ] Ensure `reset_ticks()` and sampling flow are consistent (delta since last reset).

## Validate + tune
- [ ] Re-run PID debug: `measured_ticks` should vary with motor motion (not constant).
- [ ] Confirm PID output no longer saturates/clamps unexpectedly.
- [ ] Tune gains: start with non-zero `ki` (and `kd` when ready) only after measurement is correct.

