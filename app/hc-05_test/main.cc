
#include <array>
#include "board.h"
#include "st_usart.h"

using namespace MM;

// Send Mesasge to Bluetooth every second
std::array<uint8_t, 20> tx_buf{"Hello from HC-05\r\n"};
uint8_t rx_byte = 0;

int main(int argc, char** argv)
{
	bsp_init();
	Board& board = get_board();

	while (1)
	{
		board.usart.send(tx_buf);

        // TODO: Add a more descriptive message and a counter to show that the program is running and not stuck.

		// Busy wait for a while
		for (volatile uint32_t i = 0; i < 1000000; i++);
	}

	return 0;
}
