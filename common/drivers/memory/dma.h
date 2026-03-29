/**
 * @file dma.h
 * @author Kent Hong
 * @brief Interface for DMA driver
 * 
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace MM
{
class Dma
{
public:
    /**
     * @brief Arms DMA for Peripheral to Memory transfer
     * 
     * @param destination Destination address
     * @param num_items Number of data items to be transferred
     * @return true successful arm, false otherwise
     */
    virtual bool arm_p2m(uintptr_t destination, size_t num_items) = 0;

    /**
     * @brief Arms DMA for Memory to Peripheral transfer
     * 
     * @param source Source address
     * @param num_items Number of data items to be transferred
     * @return true successful arm, false otherwise
     */
    virtual bool arm_m2p(uintptr_t source, size_t num_items) = 0;

    /**
    * @brief Arms DMA for Memory to Memory transfer
    * @param source Source address
    * @param destination Destination address
    * @param num_items Number of data items to be transferred
    * 
    * @return true successful arm, false otherwise
    */
    virtual bool arm_m2m(uintptr_t source, uintptr_t destination,
                         size_t num_items) = 0;

    /**
     * @brief Start DMA transfer
     * 
     * @return true Transfer successfully started, false otherwise
     */
    virtual bool start() = 0;

    /**
    * @brief Checks whether an active transfer completed.
    *
    * @return true transfer completed and driver transitioned to READY,
    *         false transfer still in progress or error
    */
    virtual bool complete() = 0;

    /**
    * @brief Stops DMA transfer in emergency and clears flags
    *
    * @return true successful stop, false otherwise
    */
    virtual bool abort() = 0;

    /**
     * @brief Destroy the Dma object
     * 
     */
    ~Dma() = default;
};
};  // namespace MM