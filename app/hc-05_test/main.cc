
#include <array>
#include <cstdio>
#include "board.h"

using namespace MM;

uint8_t rx_byte = 0;

// Buffer size for outgoing debug messages
constexpr size_t kTxBufSize = 64;
// Delay loop count for ~1 second delay (adjust as needed for your MCU)
constexpr uint32_t kDelayLoopCount = 1000000;

int main(int argc, char** argv)
{
    bsp_init();
    Board& board = get_board();

    // Counter for debug messages
    uint32_t debug_counter = 0;
    std::array<uint8_t, kTxBufSize> tx_buf{};

    while (1)
    {
        /**
		* @note TODO: Replace this with actual messages we want to send over Bluetooth.
		* For demonstration, we're sending a simple debug message with a counter.
		*/
        int msg_len = std::snprintf(
            reinterpret_cast<char*>(
                tx_buf
                    .data()),  // Cast to char* for snprintf - it write into buffer as bytes
            tx_buf.size(), "[HC-05 DEBUG] Count: %lu\r\n",
            static_cast<unsigned long>(debug_counter++));

        // Only send if formatting was successful and fits in buffer
        if (msg_len > 0 && static_cast<size_t>(msg_len) < tx_buf.size())
        {
            board.usart.send(std::span<const uint8_t>(tx_buf.data(), msg_len));
        }

        for (volatile uint32_t i = 0; i < kDelayLoopCount; ++i);
    }

    return 0;
}
