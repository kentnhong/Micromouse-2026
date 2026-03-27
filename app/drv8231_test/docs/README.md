## How the DRV8231 Motor Driver Works

The DRV8231 in this use case is just a generic DC motor driver. It takes 2 GPIO signals (IN1, IN2) from the host (MCU). By setting these pins high or low in our code. The chip switches the motor’s power accordingly no advanced control needed.

Just set IN1/IN2 in your code, and the DRV8231 does the rest (which I done that in the enum class).

Though this motor driver is only support only one wheel meaning if we want to driver motor forward/backward 2 (Left + Right Wheel) should get the I/O assign at synchronous timing.

### DRV8231 Pin Functions

- **IN1, IN2:** Digital inputs from the MCU. Set these high or low to control the motor:
  - IN1=0, IN2=0: Motor coasts (freewheels)
  - IN1=0, IN2=1: Motor spins forward
  - IN1=1, IN2=0: Motor spins reverse
  - IN1=1, IN2=1: Motor brakes (fast stop)
- **OUT1, OUT2:** Outputs to the motor. The DRV8231 switches these based on IN1/IN2.
- **PWM:** We can apply a PWM signal to IN1 or IN2 for speed control (for precise speed & based on the state machine we make our mice make decision it will drive indifferent speed).
