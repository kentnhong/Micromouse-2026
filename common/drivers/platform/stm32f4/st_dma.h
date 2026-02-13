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

struct StDmaParams
{
};

class HwDma : public Dma
{
public:
    explicit HwDma(StDmaParams& params);

    /**
    * @brief Starts DMA transfer from peripheral to memory
    * 
    * @return true successful transfer, false otherwise
    */
    bool start() override;

    /**
    * @brief Stops DMA transfer after it is complete
    * 
    * @return true successful stop, false otherwise
    */
    bool stop() override;
};
};  // namespace Stmf4
};  // namespace MM