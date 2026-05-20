TODO LIST:

**HAL Driver replacement:**
- st_pwm_hal.cc / st_enc_hal.cc
- testing it on the NUCLEO & custom HW

**HIGHER ABSTRACTION NEED TO FIX**
- PID controller math is fine
    - Encoder  & PWM replace the method of measuring the ticks and outputting the PWM 
- Drv8231 logic need to check again if the PWM pulsing out is clean
- Trapezoial profile 