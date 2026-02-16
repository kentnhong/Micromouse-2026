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
    // Set channel selection in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_CHSEL;
    stream_base_addr->CR |=
        (static_cast<uint32_t>(settings.channel) << DMA_SxCR_CHSEL_Pos);

    // Set current target to Memory 0 in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_CT;

    // Clear DBM in DMA_SxCR to prevent buffer switching
    stream_base_addr->CR &= ~DMA_SxCR_DBM;

    // Set priority level in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_PL;
    stream_base_addr->CR |=
        (static_cast<uint32_t>(settings.priority) << DMA_SxCR_PL_Pos);

    // Set memory and peripheral data size in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_MSIZE;
    stream_base_addr->CR |=
        (static_cast<uint32_t>(settings.width) << DMA_SxCR_MSIZE_Pos);
    stream_base_addr->CR &= ~DMA_SxCR_PSIZE;
    stream_base_addr->CR |=
        (static_cast<uint32_t>(settings.width) << DMA_SxCR_PSIZE_Pos);

    // Set memory increment and clear peripheral increment for PERIPH_TO_MEM transfer (can change later when we need to support other directions)
    stream_base_addr->CR |= DMA_SxCR_MINC;
    stream_base_addr->CR &= ~DMA_SxCR_PINC;

    // Disable circular mode in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_CIRC;

    // Set data transfer direction in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_DIR;
    stream_base_addr->CR |=
        (static_cast<uint32_t>(settings.data_dir) << DMA_SxCR_DIR_Pos);

    // Set DMA as the flow controller in DMA_SxCR
    stream_base_addr->CR &= ~DMA_SxCR_PFCTRL;

    // DMA interrupt enables go here if needed

    // Enable direct mode in DMA_SxFCR
    stream_base_addr->FCR &= ~DMA_SxFCR_DMDIS;

    return true;
}

bool HwDma::start(uintptr_t source, uintptr_t destination, size_t num_items)
{
    /* Check if source and destination addr are valid addrs and write the addrs to the respective registers*/
    // TODO: Clean this up when you have time
    switch (settings.data_dir)
    {
        case DmaDataDir::PERIPH_TO_MEM:
            if (source < kPeriphBase || source > kPeriphEnd)
                return false;
            if (destination < kSramBase || destination > kSramEnd)
                return false;
            if (!is_aligned(destination))
                return false;
            // Write source addr to DMA_SxPAR
            stream_base_addr->PAR = source;
            // Write destination addr to DMA_SxM0AR
            stream_base_addr->M0AR = destination;
            break;
        case DmaDataDir::MEM_TO_PERIPH:
            if (!((source >= kSramBase && source <= kSramEnd) ||
                  (source >= kFlashBase && source <= kFlashEnd)))
                return false;
            if (destination < kPeriphBase || destination > kPeriphEnd)
                return false;
            if (!is_aligned(source))
                return false;
            // Write source addr to DMA_SxM0AR
            stream_base_addr->M0AR = source;
            // Write destination addr to DMA_SxPAR
            stream_base_addr->PAR = destination;
            break;
        case DmaDataDir::MEM_TO_MEM:
            if (!((source >= kSramBase && source <= kSramEnd) ||
                  (source >= kFlashBase && source <= kFlashEnd)))
                return false;
            if (destination < kSramBase || destination > kSramEnd)
                return false;
            if (!is_aligned(destination) || !is_aligned(source))
                return false;
            // Write source addr to DMA_SxPAR
            stream_base_addr->PAR = source;
            // Write destination addr to DMA_SxM0AR
            stream_base_addr->M0AR = destination;
    }

    /* Check if counter is within the threshold for DMA_SxNDTR for the F411 */
    if (num_items < 1 || num_items > 65535)
    {
        return false;
    }
    stream_base_addr->NDTR = num_items;

    /* Enable dma stream in DMA_SxCR */

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