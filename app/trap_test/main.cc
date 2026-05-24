/**
 * @file main.cc
 * @brief Trap test — button press runs F L F L F R F via MotionController.
 * @author Bex Saw
 */

#include <cstdint>
#include <span>
#include "board.h"
#include "delay.h"
#include "ircontroller.h"

using namespace MM;

static void usart_print(Usart& usart, const char* str)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(str);
    uint32_t len = 0;
    while (str[len]) { ++len; }
    usart.send(std::span<const uint8_t>(data, len));
}

int main()
{
    if (!bsp_init())
    {
        return 1;
    }

    Board& hw = get_board();
    const IrValues no_ir{};

    while (1)
    {
        // Wait for button press (active low)
        while (hw.start_bt.read() != 0) {}
        Utils::delay_ms(50);
        while (hw.start_bt.read() != 0) {}

        usart_print(hw.usart, "SEQ_START\r\n");

        while (!hw.motion.forward(no_ir)) {}
        usart_print(hw.usart, "F_DONE\r\n");

        while (!hw.motion.turn_left()) {}
        usart_print(hw.usart, "L_DONE\r\n");

        while (!hw.motion.forward(no_ir)) {}
        usart_print(hw.usart, "F_DONE\r\n");

        while (!hw.motion.turn_left()) {}
        usart_print(hw.usart, "L_DONE\r\n");

        while (!hw.motion.forward(no_ir)) {}
        usart_print(hw.usart, "F_DONE\r\n");

        while (!hw.motion.turn_right()) {}
        usart_print(hw.usart, "R_DONE\r\n");

        while (!hw.motion.forward(no_ir)) {}
        usart_print(hw.usart, "F_DONE\r\n");

        usart_print(hw.usart, "SEQ_DONE\r\n");
    }

    return 0;
}
