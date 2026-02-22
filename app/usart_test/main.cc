#include <array>
#include "board.h"

using namespace MM;

std::array<uint8_t, 17> tx_byte{"XuperMouse Action!\n"};
uint8_t rx_byte;

int main(int argc, char** argv)
{
    bsp_init();
    Board board = get_board();

    while (1)
    {
        board.usart.send(tx_byte);
        while (!board.usart.receive(&rx_byte, 1));
    }

    return 0;
}