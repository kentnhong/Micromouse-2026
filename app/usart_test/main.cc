#include <array>
#include "board.h"
#include "st_usart.h"  // for debugging purposes

using namespace MM;

std::array<uint8_t, 20> tx_buf{"XuperMouse!\r\n"};
uint8_t rx_byte;

int main(int argc, char** argv)
{
    bsp_init();
    Board board = get_board();

    while (1)
    {
        board.usart.send(tx_buf);
        // Busy wait
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }

    return 0;
}