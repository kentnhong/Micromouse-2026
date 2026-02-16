/**
 * @file st_dma.h
 * @author Kent Hong
 * @brief DMA Driver for the STM32F411
 * 
 */
#pragma once

#include "dma.h"
#include "stm32f4xx.h"

namespace MM
{
namespace Stmf4
{

enum class DmaChSel : uint8_t
{
    CH0 = 0,
    CH1,
    CH2,
    CH3,
    CH4,
    CH5,
    CH6,
    CH7
};

// Add burst transfer config for memory (MBURST) and periph (PBURST)?

enum class DmaPriority : uint8_t
{
    LOW = 0,
    MEDIUM,
    HIGH,
    VERY_HIGH
};

enum class DmaWidth : uint8_t
{
    BYTE = 0,
    HALF_WORD,
    WORD
};

enum class DmaDataDir : uint8_t
{
    PERIPH_TO_MEM = 0,
    MEM_TO_PERIPH,
    MEM_TO_MEM
};

struct StDmaSettings
{
    DmaChSel channel;
    DmaPriority priority;
    DmaWidth width;
    DmaDataDir data_dir;
};

struct StDmaParams
{
    StDmaSettings settings;
    DMA_TypeDef* base_addr;
    DMA_Stream_TypeDef* stream_base_addr;
};

class HwDma : public Dma
{
public:
    explicit HwDma(const StDmaParams& params_);

    /**
     * @brief DMA register configuration
     * 
     * @return true initialization successful, false otherwise
     */
    bool init();

    /**
    * @brief Starts DMA transfer from peripheral to memory
    * @param source Source address
    * @param destination Destination address
    * @param num_items Number of data items to be transferred
    * 
    * @return true successful transfer, false otherwise
    */
    bool start(uintptr_t source, uintptr_t destination,
               size_t num_items) override;

    /**
    * @brief Stops DMA transfer in emergency and clears flags
    * 
    * @return true successful stop, false otherwise
    */
    bool abort() override;

private:
    /**
     * @brief Clears DMA flags (used before and after transaction)
     * 
     * @return true 
     * @return false 
     */
    bool reset();

    /**
     * @brief Checks if the selected address is aligned with the width
     * 
     * @param addr 
     * @return true 
     * @return false 
     */
    bool is_aligned(uintptr_t addr)
    {
        return (addr & (static_cast<uint8_t>(settings.width) - 1u)) == 0u;
    }

    StDmaSettings settings;
    DMA_TypeDef* base_addr;
    DMA_Stream_TypeDef* stream_base_addr;

    static constexpr uintptr_t kSramBase = 0x20000000u;
    static constexpr uintptr_t kSramEnd = 0x20020000u;

    static constexpr uintptr_t kFlashBase = 0x08000000u;
    static constexpr uintptr_t kFlashEnd = 0x0807FFFFu;

    static constexpr uintptr_t kPeriphBase = 0x40000000u;
    static constexpr uintptr_t kPeriphEnd = 0x5003FFFFu;
};
};  // namespace Stmf4
};  // namespace MM