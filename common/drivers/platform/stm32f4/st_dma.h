/**
 * @file st_dma.h
 * @author Kent Hong
 * @brief DMA Driver for the STM32F411
 * 
 */
#pragma once

#include <array>
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

// Add burst transfer config for memory (MBURST) and periph (PBURST)? 8-16 beats might be good for us

// Recommended High Priority for direct mode
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
    uint32_t periph_addr = 0;
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
     * @brief Arms DMA for Peripheral to Memory transfer
     * 
     * @param destination Destination address
     * @param num_items Number of data items to be transferred
     * @return true successful arm, false otherwise
     */
    bool arm_p2m(uintptr_t destination, size_t num_items) override;

    /**
     * @brief Arms DMA for Memory to Peripheral transfer
     * 
     * @param source Source address
     * @param num_items Number of data items to be transferred
     * @return true successful arm, false otherwise
     */
    bool arm_m2p(uintptr_t source, size_t num_items) override;

    /**
    * @brief Arms DMA for Memory to Memory transfer
    * @param source Source address
    * @param destination Destination address
    * @param num_items Number of data items to be transferred
    * 
    * @return true successful transfer, false otherwise
    */
    bool arm_m2m(uintptr_t source, uintptr_t destination,
             size_t num_items) override;

    /**
     * @brief Start DMA transfer
     * 
     * @return true Transfer successfully started, false otherwise
     */
    bool start() override;

    /**
    * @brief Checks if active transfer completed and returns DMA to READY.
    *
    * @return true transfer complete, false otherwise
    */
    bool complete() override;

    /**
    * @brief Stops DMA transfer in emergency and clears flags
    * 
    * @return true successful stop, false otherwise
    */
    bool abort() override;

private:
    enum class DmaState : uint8_t
    {
        RESET = 0,  // DMA not initialized
        READY,      // DMA initialized
        ARMED,  // DMA has source & destination addr and num bytes to be sent
        BUSY,   // DMA is in middle of transfer
    };

    /**
     * @brief Clears DMA flags (used before and after transaction)
     * 
     * @return true DMA flags cleared, false otherwise
     */
    bool clear_flags();

    /**
     * @brief Checks if the selected address is aligned with the width
     * 
     * @param addr 
     * @return true aligned, false otherwise
     */
    bool is_aligned(uintptr_t addr);

    DmaState state{
        DmaState::
            RESET};  // state is RESET by default when DMA class is instantiated
    StDmaSettings settings;
    DMA_TypeDef* base_addr;
    DMA_Stream_TypeDef* stream_base_addr;
    uintptr_t periph_addr;
};
};  // namespace Stmf4
};  // namespace MM