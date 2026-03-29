#include <array>
#include <cstdio>
#include "board.h"
#include "delay.h"

using namespace MM;

uint8_t rx_byte = 0;

namespace
{
constexpr size_t kNumSamples = 4;
constexpr uint32_t kDmaTimeoutMs = 10;
}  // namespace

int main(int argc, char* argv[])
{
    [[maybe_unused]] bool result = true;

    result &= board_init();
    Board& board = get_board();
    result &= board.ir_led.set(1);

    // DMA destination buffer to inspect in debugger.
    std::array<uint16_t, kNumSamples> samples{0, 0, 0, 0};

    while (1)
    {
        // Re-arm DMA to the beginning of samples for each 4-conversion burst.
        result &= board.dma.arm_p2m(reinterpret_cast<uintptr_t>(samples.data()),
                                    kNumSamples);
        result &= board.dma.start();

        // Start one regular-sequence conversion (4 ranks configured in board setup).
        result &= board.adc.convert(true, 4);

        // Wait until DMA finishes transfers, but avoid deadlock on DMA fault.
        const uint32_t start_ms = MM::Utils::get_ms_ticks();
        while (!board.dma.complete())
        {
            const uint32_t elapsed_ms = MM::Utils::get_ms_ticks() - start_ms;
            if (elapsed_ms >= kDmaTimeoutMs)
            {
                result = false;
                break;
            }
        }

        uint32_t sample_sum = 0;
        for (size_t i = 0; i < samples.size(); i++)
        {
            sample_sum += samples[i];
        }
        [[maybe_unused]] uint16_t sample_avg =
            static_cast<uint16_t>(sample_sum / samples.size());

        // std::array<uint8_t, 14> ir_txt{"IR Sensor 1\r\n"};
        // std::array<uint8_t, 13> divider{"----------\r\n"};
        // std::array<uint8_t, 14> raw_sample_txt{"Raw Samples: "};
        // uint8_t raw_lower_avg_0 = static_cast<uint8_t>((samples[0] & 0xFF));
        // uint8_t raw_upper_avg_0 = static_cast<uint8_t>((samples[0] >> 8));
        // uint8_t raw_lower_avg_1 = static_cast<uint8_t>((samples[1] & 0xFF));
        // uint8_t raw_upper_avg_1 = static_cast<uint8_t>((samples[1] >> 8));
        // uint8_t raw_lower_avg_2 = static_cast<uint8_t>((samples[2] & 0xFF));
        // uint8_t raw_upper_avg_2 = static_cast<uint8_t>((samples[2] >> 8));
        // uint8_t raw_lower_avg_3 = static_cast<uint8_t>((samples[3] & 0xFF));
        // uint8_t raw_upper_avg_3 = static_cast<uint8_t>((samples[3] >> 8));
        // std::array<uint8_t, 13> raw_samples{raw_lower_avg_0,
        //                                     raw_upper_avg_0,
        //                                     ' ',
        //                                     raw_lower_avg_1,
        //                                     raw_upper_avg_1,
        //                                     ' ',
        //                                     raw_lower_avg_2,
        //                                     raw_upper_avg_2,
        //                                     ' ',
        //                                     raw_lower_avg_3,
        //                                     raw_upper_avg_3,
        //                                     '\r',
        //                                     '\n'};
        // std::array<uint8_t, 17> avg_sample_txt{"Sample Average: "};
        // uint8_t lower_avg = static_cast<uint8_t>((sample_avg & 0xFF));
        // uint8_t upper_avg = static_cast<uint8_t>((sample_avg >> 8));
        // std::array<uint8_t, 4> adc_avg{lower_avg, upper_avg, '\r', '\n'};

        // result &= board.usart.send(ir_txt);
        // result &= board.usart.send(divider);
        // result &= board.usart.send(raw_sample_txt);
        // result &= board.usart.send(raw_samples);
        // result &= board.usart.send(avg_sample_txt);
        // result &= board.usart.send(adc_avg);

        MM::Utils::DelayMs(50);
    }
}