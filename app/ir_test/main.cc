#include <array>
#include <cstdio>
#include "board.h"
using namespace MM;

uint8_t rx_byte = 0;

namespace
{
constexpr uint32_t kPrintEveryNSequences = 10;
}

int main(int argc, char* argv[])
{
    [[maybe_unused]] bool result = true;

    result &= board_init();
    Board& board = get_board();

    uint32_t sequence_counter = 0;
    std::array<uint8_t, 40> tx_buf{};

    while (1)
    {
        if (!board.ir_controller.is_sequence_done())
        {
            continue;
        }

        sequence_counter++;
        if ((sequence_counter % kPrintEveryNSequences) != 0)
        {
            continue;
        }

        const IrValues& vals = board.ir_controller.get_ir_vals();
        int msg_len =
            std::snprintf(reinterpret_cast<char*>(tx_buf.data()), tx_buf.size(),
                          "L:%u FL:%u FR:%u R:%u\r\n", vals.left,
                          vals.front_left, vals.front_right, vals.right);

        if (msg_len > 0)
        {
            const size_t len = static_cast<size_t>(msg_len);
            board.usart.send(std::span<const uint8_t>(
                tx_buf.data(), (len < tx_buf.size()) ? len : tx_buf.size()));
        }
    }

    return 0;
}