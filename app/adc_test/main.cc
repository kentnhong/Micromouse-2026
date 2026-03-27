#include "adc.h"
#include "board.h"
#include "delay.h"

#include <array>

#include "stm32f411xe.h"

using namespace MM;

namespace
{
constexpr size_t kNumSamples = 4;
}  // namespace

int main(int argc, char* argv[])
{
    [[maybe_unused]] bool result = true;

    result &= board_init();
    Board& board = get_board();

    // DMA destination buffer to inspect in debugger.
    std::array<uint16_t, kNumSamples> samples{0, 0, 0, 0};

    while (1)
    {
    }
}