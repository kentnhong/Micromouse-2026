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

    const uintptr_t source = reinterpret_cast<uintptr_t>(&ADC1->DR);
    const uintptr_t destination = reinterpret_cast<uintptr_t>(samples.data());

    while (1)
    {
        samples.fill(0u);

        result &= board.adc.en_dma_req();
        // TODO: Continue here, need to figure out how to set_channel and set_cycles after each ADC channel without being hardware specific
        // dma arm() also needs source and destination addresses from the CMSIS header which is hardware specific...Need to restructure driver architecture

        result &= board.dma.arm(source, destination, samples.size());

        result &= board.dma.start();

        board.ir_led.set(false);
        MM::Utils::DelayUs(200);
        board.adc.convert(true);  // ambient #1
        MM::Utils::DelayUs(200);
        board.adc.convert(true);  // ambient #2

        board.ir_led.set(true);
        MM::Utils::DelayUs(200);
        board.adc.convert(true);  // active #1
        MM::Utils::DelayUs(200);
        board.adc.convert(true);  // active #2

        while (!board.dma.complete())
        {
        }

        const uint16_t ambient =
            static_cast<uint16_t>((samples[0] + samples[1]) / 2u);
        const uint16_t active =
            static_cast<uint16_t>((samples[2] + samples[3]) / 2u);
        [[maybe_unused]] int32_t true_signal =
            static_cast<int32_t>(active) - static_cast<int32_t>(ambient);
        if (true_signal < 0)
            true_signal = 0;

        board.ir_led.set(false);
        MM::Utils::DelayUs(200);
    }
}