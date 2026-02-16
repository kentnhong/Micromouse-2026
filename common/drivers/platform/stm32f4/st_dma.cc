#include "st_dma.h"

namespace MM
{
namespace Stmf4
{
HwDma::HwDma(const StDmaParams& params_)
    : settings{params_.settings},
      base_addr{params_.base_addr},
      stream_base_addr{params_.stream_base_addr}
{
}

bool HwDma::init()
{
    return true;
}

bool HwDma::start(uintptr_t source, uintptr_t destination, size_t num_items)
{
    /* Check if source and destination addr are valid addrs */
    switch (settings.data_dir)
    {
        case DmaDataDir::PERIPH_TO_MEM:
            if (source < kPeriphBase || source > kPeriphEnd)
                return false;
            if (destination < kSramBase || destination > kSramEnd)
                return false;
            if (!is_aligned(destination))
                return false;
            break;
        case DmaDataDir::MEM_TO_PERIPH:
            if (!((source >= kSramBase && source <= kSramEnd) ||
                  (source >= kFlashBase && source <= kFlashEnd)))
                return false;
            if (destination < kPeriphBase || destination > kPeriphEnd)
                return false;
            if (!is_aligned(source))
                return false;
            break;
        case DmaDataDir::MEM_TO_MEM:
            if (!((source >= kSramBase && source <= kSramEnd) ||
                  (source >= kFlashBase && source <= kFlashEnd)))
                return false;
            if (destination < kSramBase || destination > kSramEnd)
                return false;
            if (!is_aligned(destination))
                return false;
            if (!is_aligned(source))
                return false;
    }

    /* Counter threshold for DMA_SxNDTR for the F411 */
    if (num_items < 1 || num_items > 65535)
    {
        return false;
    }

    return true;
}

bool HwDma::abort()
{
    return true;
}

bool HwDma::reset()
{
    return true;
}
};  // namespace Stmf4
};  // namespace MM