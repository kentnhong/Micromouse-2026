/**
 * @file main.cc
 * @brief IR threshold test — prints raw sensor values and wall detections via USART.
 *        Use this to verify/tune kThresholdFront and kThresholdSide in floodfill.cc.
 *
 * Output format (every 10 IR sequences, ~100 ms):
 *   L:1234 FL:2100 FR:1850 R:3900 | left:open front:WALL right:WALL
 */

#include <array>
#include <cstdio>
#include "board.h"
#include "delay.h"

using namespace MM;

uint8_t rx_byte = 0;

namespace
{
// Must match values in floodfill.cc — tune them together.
constexpr uint16_t kThresholdFront{2000};
constexpr uint16_t kThresholdSide{2000};

constexpr uint32_t kPrintEveryNSequences{10};
}  // namespace

int main()
{
    if (!board_init())
    {
        return 1;
    }

    Board& board = get_board();
    uint32_t seq_count = 0;
    std::array<uint8_t, 72> tx_buf{};

    while (1)
    {
        if (!board.ir_controller.is_sequence_done())
        {
            continue;
        }

        seq_count++;
        if ((seq_count % kPrintEveryNSequences) != 0)
        {
            continue;
        }

        const IrValues& v = board.ir_controller.get_ir_vals();

        const bool wall_front = (v.front_left > kThresholdFront) ||
                                (v.front_right > kThresholdFront);
        const bool wall_left = v.left > kThresholdSide;
        const bool wall_right = v.right > kThresholdSide;

        int len = std::snprintf(
            reinterpret_cast<char*>(tx_buf.data()), tx_buf.size(),
            "L:%4u FL:%4u FR:%4u R:%4u | L:%s F:%s R:%s\r\n", v.left,
            v.front_left, v.front_right, v.right, wall_left ? "WALL" : "open",
            wall_front ? "WALL" : "open", wall_right ? "WALL" : "open");

        if (len > 0)
        {
            const size_t out_len = static_cast<size_t>(len) < tx_buf.size()
                                       ? static_cast<size_t>(len)
                                       : tx_buf.size();
            board.usart.send(std::span<const uint8_t>(tx_buf.data(), out_len));
        }
    }

    return 0;
}
