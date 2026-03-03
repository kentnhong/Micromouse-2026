#include "st_dma.h"

namespace
{
// Adrresses
constexpr uintptr_t kSramBase = 0x20000000u;
constexpr uintptr_t kSramEnd = 0x20020000u;
constexpr uintptr_t kFlashBase = 0x08000000u;
constexpr uintptr_t kFlashEnd = 0x0807FFFFu;
constexpr uintptr_t kPeriphBase = 0x40000000u;
constexpr uintptr_t kPeriphEnd = 0x5003FFFFu;

// Counter threshold for DMA_SxNDTR
constexpr uint16_t kMinCount = 1;
constexpr uint16_t kMaxCount = 65535;

// Bit Masks
constexpr uint32_t kNdtrMask = 0xFFFF0000u;
constexpr uint16_t kNumItemsMask = 0xFFFFu;

// LIFCR and HIFCR flag base positions
constexpr uint8_t kFlagBase[4] = {0, 6, 16, 22};

static const std::array<DMA_Stream_TypeDef*, 8> dma_lisr = {
    DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3,
    DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3};
static const std::array<DMA_Stream_TypeDef*, 8> dma_hisr = {
    DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7,
    DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7};

static inline uint32_t clear_mask(uint8_t g)
{
    uint32_t b = kFlagBase[g];
    return (1u << (b + 0)) | (1u << (b + 2)) | (1u << (b + 3)) |
           (1u << (b + 4)) | (1u << (b + 5));
}
};  // namespace

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

constexpr uint8_t kFlagBase[4] = {0, 6, 16, 22};
static std::array<DMA_Stream_TypeDef*, 8> dma_lisr = {
    DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3,
    DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3};
static std::array<DMA_Stream_TypeDef*, 8> dma_hisr = {
    DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7,
    DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7};

static inline uint32_t clear_mask(uint8_t g)
{
    uint32_t b = kFlagBase[g];
    return (1u << (b + 0)) | (1u << (b + 2)) | (1u << (b + 3)) |
           (1u << (b + 4)) | (1u << (b + 5));
}

bool HwDma::init()
{
    // Disable stream before modifying other CR fields
    stream_base_addr->CR &= ~DMA_SxCR_EN;
    while (stream_base_addr->CR & DMA_SxCR_EN);

    // Clear DMA flags
    if (!clear_flags())
        return false;

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

    // Mem 2 Mem
    if (settings.data_dir == DmaDataDir::MEM_TO_MEM)
    {
        stream_base_addr->CR |= DMA_SxCR_MINC;
        stream_base_addr->CR |= DMA_SxCR_PINC;
    }
    // Periph 2 Mem or Mem 2 Periph
    else
    {
        stream_base_addr->CR |= DMA_SxCR_MINC;
        stream_base_addr->CR &= ~DMA_SxCR_PINC;
    }

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

    state = DmaState::READY;
    return true;
}

bool HwDma::arm(uintptr_t source, uintptr_t destination, size_t num_items)
{
    // Check if dma is initialized; allow re-arm from ARMED state for overrun recovery
    if (state != DmaState::READY && state != DmaState::ARMED)
        return false;

    // Disable stream and wait until it is actually disabled
    stream_base_addr->CR &= ~DMA_SxCR_EN;
    while (stream_base_addr->CR & DMA_SxCR_EN);

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
    if (num_items < kMinCount || num_items > kMaxCount)
    {
        return false;
    }
    stream_base_addr->NDTR = (stream_base_addr->NDTR & kNdtrMask) |
                             (static_cast<uint32_t>(num_items) & kNumItemsMask);

    state = DmaState::ARMED;
    return true;
}

bool HwDma::start()
{
    // Check if DMA is armed
    if (state != DmaState::ARMED)
        return false;

    // Clear DMA flags
    if (!clear_flags())
        return false;

    /* Enable dma stream in DMA_SxCR */
    stream_base_addr->CR |= DMA_SxCR_EN;

    state = DmaState::BUSY;
    return true;
}

bool HwDma::abort()
{
    // Check if DMA is in a transfer
    if (state != DmaState::BUSY)
        return false;

    // Clear EN in DMA_SxCR to disable stream
    stream_base_addr->CR &= ~DMA_SxCR_EN;

    // Wait until EN in DMA_SxCR is 0
    while (stream_base_addr->CR & DMA_SxCR_EN);

    // DMA_SxNDTR contains # of remaining data items after stream was stopped (for debugging)
    [[maybe_unused]] uint16_t remaining =
        static_cast<uint16_t>(stream_base_addr->NDTR);

    // Clear flags in LIFCR or HIFCR
    if (!clear_flags())
        return false;

    state = DmaState::READY;
    return true;
}

bool HwDma::clear_flags()
{
    // Check if stream_base_addr is in dma_lisr
    for (size_t i = 0; i < dma_lisr.size(); i++)
    {
        if (stream_base_addr == dma_lisr[i])
        {
            DMA_TypeDef* expected = (i < 4) ? DMA1 : DMA2;
            if (base_addr != expected)
                return false;
            uint32_t g = static_cast<uint32_t>(i % 4);
            base_addr->LIFCR = clear_mask(g);
            return true;
        }
    }

    // Check if stream_base_addr is in dma_hisr
    for (size_t i = 0; i < dma_hisr.size(); i++)
    {
        if (stream_base_addr == dma_hisr[i])
        {
            DMA_TypeDef* expected = (i < 4) ? DMA1 : DMA2;
            if (base_addr != expected)
                return false;
            uint32_t g = static_cast<uint32_t>(i % 4);
            base_addr->HIFCR = clear_mask(g);
            return true;
        }
    }

    return false;
}

};  // namespace Stmf4
};  // namespace MM