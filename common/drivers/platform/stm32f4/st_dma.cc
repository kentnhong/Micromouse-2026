#include "st_dma.h"

namespace
{
// Adrresses
constexpr uintptr_t kSramEnd = 0x20020000u;
constexpr uintptr_t kPeriphEnd = 0x5003FFFFu;

// Counter threshold for DMA_SxNDTR
constexpr uint16_t kMinCount = 1;
constexpr uint16_t kMaxCount = 65535;

// Bit Masks
constexpr uint32_t kNdtrMask = 0xFFFF0000u;
constexpr uint16_t kNumItemsMask = 0xFFFFu;

// LIFCR and HIFCR flag base positions
constexpr uint8_t kFlagBase[4] = {0, 6, 16, 22};

const std::array<DMA_Stream_TypeDef*, 8> dma_lisr = {
    DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3,
    DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3};
const std::array<DMA_Stream_TypeDef*, 8> dma_hisr = {
    DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7,
    DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7};

inline uint32_t clear_mask(uint8_t g)
{
    uint32_t b = kFlagBase[g];
    return (1u << (b + 0)) | (1u << (b + 2)) | (1u << (b + 3)) |
           (1u << (b + 4)) | (1u << (b + 5));
}

bool assign_group(DMA_TypeDef* base_addr, DMA_Stream_TypeDef* stream_base_addr,
                  bool& is_high, uint32_t& group)
{
    for (size_t i = 0; i < dma_lisr.size(); i++)
    {
        if (stream_base_addr == dma_lisr[i])
        {
            DMA_TypeDef* expected = (i < 4) ? DMA1 : DMA2;
            if (base_addr != expected)
                return false;

            is_high = false;
            group = static_cast<uint32_t>(i % 4);
            return true;
        }
    }

    for (size_t i = 0; i < dma_hisr.size(); i++)
    {
        if (stream_base_addr == dma_hisr[i])
        {
            DMA_TypeDef* expected = (i < 4) ? DMA1 : DMA2;
            if (base_addr != expected)
                return false;

            is_high = true;
            group = static_cast<uint32_t>(i % 4);
            return true;
        }
    }

    return false;
}
};  // namespace

namespace MM
{
namespace Stmf4
{
HwDma::HwDma(const StDmaParams& params_)
    : settings{params_.settings},
      base_addr{params_.base_addr},
      stream_base_addr{params_.stream_base_addr},
      periph_addr{params_.periph_addr}
{
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

    // Disable circular mode in DMA_SxCR (TODO: change when we add an option to support circular mode for DMA)
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

    // If using periph to mem or mem to periph data dir, write base_addr into dma periph addr reg
    if (settings.data_dir != DmaDataDir::MEM_TO_MEM)
    {
        if (!is_aligned(periph_addr))
            return false;
        if (periph_addr < PERIPH_BASE || periph_addr > kPeriphEnd)
            return false;
        stream_base_addr->PAR = periph_addr;
    }

    state = DmaState::READY;
    return true;
}

bool HwDma::arm_p2m(uintptr_t destination, size_t num_items)
{
    // Check if dma is initialized; allow re-arm from ARMED state for overrun recovery
    if (state != DmaState::READY && state != DmaState::ARMED)
        return false;

    // Check if data direction is set correctly
    if (settings.data_dir != DmaDataDir::PERIPH_TO_MEM)
        return false;

    // Disable stream and wait until it is actually disabled
    stream_base_addr->CR &= ~DMA_SxCR_EN;
    while (stream_base_addr->CR & DMA_SxCR_EN);

    /* Check if source and destination addr are valid addrs and write the addrs to the respective registers */
    if (destination < SRAM_BASE || destination > kSramEnd)
        return false;
    if (!is_aligned(destination))
        return false;
    // Write destination addr to DMA_SxM0AR
    stream_base_addr->M0AR = static_cast<uint32_t>(destination);

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

bool HwDma::arm_m2p(uintptr_t source, size_t num_items)
{
    // Check if dma is initialized; allow re-arm from ARMED state for overrun recovery
    if (state != DmaState::READY && state != DmaState::ARMED)
        return false;

    // Check if data direction is set correctly
    if (settings.data_dir != DmaDataDir::MEM_TO_PERIPH)
        return false;

    // Disable stream and wait until it is actually disabled
    stream_base_addr->CR &= ~DMA_SxCR_EN;
    while (stream_base_addr->CR & DMA_SxCR_EN);

    /* Check if source and destination addr are valid addrs and write the addrs to the respective registers */
    if (!((source >= SRAM_BASE && source <= kSramEnd) ||
          (source >= FLASH_BASE && source <= FLASH_END)))
        return false;
    if (!is_aligned(source))
        return false;
    // Write source addr to DMA_SxM0AR
    stream_base_addr->M0AR = static_cast<uint32_t>(source);

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

bool HwDma::arm_m2m(uintptr_t source, uintptr_t destination, size_t num_items)
{
    // Check if dma is initialized; allow re-arm from ARMED state for overrun recovery
    if (state != DmaState::READY && state != DmaState::ARMED)
        return false;

    // Check if data direction is set correctly
    if (settings.data_dir != DmaDataDir::MEM_TO_MEM)
        return false;

    // Disable stream and wait until it is actually disabled
    stream_base_addr->CR &= ~DMA_SxCR_EN;
    while (stream_base_addr->CR & DMA_SxCR_EN);

    /* Check if source and destination addr are valid addrs and write the addrs to the respective registers */
    if (!((source >= SRAM_BASE && source <= kSramEnd) ||
          (source >= FLASH_BASE && source <= FLASH_END)))
        return false;
    if (destination < SRAM_BASE || destination > kSramEnd)
        return false;
    if (!is_aligned(destination) || !is_aligned(source))
        return false;
    // Write source addr to DMA_SxPAR
    stream_base_addr->PAR = static_cast<uint32_t>(source);
    // Write destination addr to DMA_SxM0AR
    stream_base_addr->M0AR = static_cast<uint32_t>(destination);

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

// TODO: Verify logic tomorrow
bool HwDma::complete()
{
    if (state != DmaState::BUSY)
        return false;

    bool is_high = false;
    uint32_t group = 0;
    if (!assign_group(base_addr, stream_base_addr, is_high, group))
        return false;

    const uint32_t base_bit = kFlagBase[group];
    const uint32_t tcif_mask = (1u << (base_bit + 5));
    const uint32_t error_mask = (1u << (base_bit + 0)) |
                                (1u << (base_bit + 2)) | (1u << (base_bit + 3));
    const uint32_t status = is_high ? base_addr->HISR : base_addr->LISR;

    if ((status & error_mask) != 0u)
    {
        (void)clear_flags();
        state = DmaState::READY;
        return false;
    }

    const bool stream_disabled = ((stream_base_addr->CR & DMA_SxCR_EN) == 0u);
    const bool ndtr_done = (stream_base_addr->NDTR == 0u);
    const bool tcif_set = ((status & tcif_mask) != 0u);

    if (tcif_set || (stream_disabled && ndtr_done))
    {
        if (!clear_flags())
            return false;

        state = DmaState::READY;
        return true;
    }

    return false;
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
    bool is_high = false;
    uint32_t group = 0;
    if (!assign_group(base_addr, stream_base_addr, is_high, group))
        return false;

    if (is_high)
        base_addr->HIFCR = clear_mask(group);
    else
        base_addr->LIFCR = clear_mask(group);

    return true;
}

bool HwDma::is_aligned(uintptr_t addr)
{
    uintptr_t bytes = 1;

    switch (settings.width)
    {
        case DmaWidth::BYTE:
            break;
        case DmaWidth::HALF_WORD:
            bytes = 2;
            break;
        case DmaWidth::WORD:
            bytes = 4;
    }

    return (addr & (bytes - 1u)) == 0u;
}

};  // namespace Stmf4
};  // namespace MM