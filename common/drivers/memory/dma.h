/**
 * @file dma.h
 * @author Kent Hong
 * @brief Interface for DMA driver
 * 
 */
#pragma once

namespace MM
{
class Dma
{
public:
    /**
    * @brief Starts DMA transfer from peripheral to memory
    * 
    * @return true successful transfer, false otherwise
    */
    virtual bool start() = 0;

    /**
    * @brief Stops DMA transfer after it is complete
    * 
    * @return true successful stop, false otherwise
    */
    virtual bool stop() = 0;

    /**
     * @brief Destroy the Dma object
     * 
     */
    ~Dma() = default;
};
};  // namespace MM