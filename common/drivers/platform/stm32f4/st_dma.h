/**
 * @file st_dma.h
 * @author Kent Hong
 * @brief DMA Driver for the STM32F411
 * 
 */
#pragma once

#include "dma.h"

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

enum class DmaWidth : uint8_t
{
    BYTE = 0,
    HALF_WORD,
    WORD
};

enum class DmaInc : uint8_t
{
    FIXED = 0,
    INCREMENT
};

enum class DmaMode : uint8_t
{
    NORMAL = 0,
    CIRCULAR
};

enum class DmaDataDir : uint8_t
{
    PERIPH_TO_MEM = 0,
    MEM_TO_PERIPH,
    MEM_TO_MEM
};

struct StDmaParams
{
    DmaChSel channel;
    DmaWidth width;
    DmaInc inc;
    DmaMode mode;
    DmaDataDir data_dir;
};

class HwDma : public Dma
{
public:
    explicit HwDma(StDmaParams& params);

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
    * @brief Stops DMA transfer after it is complete
    * 
    * @return true successful stop, false otherwise
    */
    bool stop() override;
};
};  // namespace Stmf4
};  // namespace MM