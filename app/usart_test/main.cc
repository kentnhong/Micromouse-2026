#include <array>
#include "board.h"
#include "st_usart.h" // for debugging purposes 

using namespace MM;

std::array<uint8_t, 20> tx_buf{"XuperMouse!\r\n"};
uint8_t rx_byte;
uint8_t raw_freq; // store the raw freq value read from the USART peripheral for debugging purposes

int main(int argc, char** argv)
{
    /**
    TODO: sysclk object & init with the default 16MHz and pass it into the freq for USART (Not work)
    get_freq() after sysclk initialization for 16MHz, its not actually 16MHz.
    */
    bsp_init();
    Board board = get_board();

    while (1)
    {
        board.usart.send(tx_buf);
        // Store the USART clock frequency for debugging
        raw_freq = static_cast<uint8_t>(static_cast<MM::Stmf4::StUsart&>(board.usart).get_clock_freq());
        // Busy wait
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }

    return 0;
}