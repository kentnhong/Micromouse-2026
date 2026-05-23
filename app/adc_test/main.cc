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
        std::array<uint8_t, 128> tx_buf{};
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

        const int msg_len =
            std::snprintf(reinterpret_cast<char*>(tx_buf.data()), tx_buf.size(),
                          "IR Sensor 1\r\n----------\r\nRaw Samples: %u %u %u "
                          "%u\r\nSample Average: %u\r\n",
                          static_cast<unsigned>(samples[0]),
                          static_cast<unsigned>(samples[1]),
                          static_cast<unsigned>(samples[2]),
                          static_cast<unsigned>(samples[3]),
                          static_cast<unsigned>(sample_avg));

        if (msg_len > 0)
        {
            const size_t len = static_cast<size_t>(msg_len);
            result &= board.usart.send(std::span<const uint8_t>(
                tx_buf.data(), (len < tx_buf.size()) ? len : tx_buf.size()));
        }

        MM::Utils::delay_ms(500);
    }
}