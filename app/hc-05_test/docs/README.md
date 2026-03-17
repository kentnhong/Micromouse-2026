# HC-05 Bluetooth USART Test (STM32F411)

This document describes the setup and important considerations for interfacing with the HC-05 Bluetooth module using USART on the STM32F411.

## Hardware Setup
- **USART2** is used for communication with the HC-05 module.
- **TX Pin:** PA11 (USART2_TX)
- **RX Pin:** PC7 (USART2_RX)
- Ensure the HC-05 module is connected to these pins accordingly.
- Module and MCU must share a common ground.

## USART Configuration
- **Baud Rate:** 9600 bps (default for HC-05)
- **Oversampling:** x16
- **Sample Mode:** Majority
- **Clock Source:** HSI 16 MHz (for now but we will switch to HSE later)
- **No hardware flow control** (RTS/CTS not used)
- After connect the Bluetooth module, scan for new devices from the PC and
you will find the module with the device name “HC‐05”, after that, click to connect, if some message appears asking about “Pairing code” just put **“1234”** as default code.
- BLUE LED = ACTIVE (Blinking 500ms period inactive connection, change 1seg
with active connection)

## Example Usage
- Power the board and connect to the HC-05 from a Bluetooth terminal app.
- You should see the message `Hello from HC-05` every second. (For now) but we will sending back a package of other kind of data later on...
- Typing in the terminal will echo each character back.

Datasheet: [Link](https://www.faranux.com/wp-content/uploads/2016/12/Datasheet.pdf)

