#include "board.h"

namespace MM
{
bool board_init()
{
    // Init AHB1 clock
    // Init APB2 clock
    // Init ADC
    return true;
}
};  // namespace MM

// // Check for overrun (we are going to reinit the DMA stream, clear ADC OVR, and restart ADC conversions in the BSP via OVR interrupt)
//     if (base_addr->SR & ADC_SR_OVR)
//     {
//         // Reinitialize the DMA (adjust destination address and NDTR counter)
//         // Clear the ADC OVR bit in ADC_SR register
//         // Trigger the ADC to start the conversion
//     }