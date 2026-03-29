#include <array>
#include <cstdio>
#include "adc.h"
#include "board.h"
#include "delay.h"

using namespace MM;

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

        if (!result)
        {
            break;
        }

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

        if (!result)
        {
            break;
        }

        uint32_t sample_sum = 0;
        for (size_t i = 0; i < samples.size(); i++)
        {
            sample_sum += samples[i];
        }
        uint16_t sample_avg =
            static_cast<uint16_t>(sample_sum / samples.size());

        std::array<char, 32> uart_line{};
        int chars_written = std::snprintf(uart_line.data(), uart_line.size(),
                                          "IR Sensor 1: %u\r\n",
                                          static_cast<unsigned>(sample_avg));
        if (chars_written <= 0)
        {
            result = false;
            break;
        }

        size_t tx_len = static_cast<size_t>(chars_written);
        if (tx_len >= uart_line.size())
        {
            tx_len = uart_line.size() - 1;
        }

        result &= board.usart.send(std::span<const uint8_t>(
            reinterpret_cast<const uint8_t*>(uart_line.data()), tx_len));

        if (!result)
        {
            break;
        }
    }
}