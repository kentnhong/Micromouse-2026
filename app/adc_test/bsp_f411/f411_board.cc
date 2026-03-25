#include "board.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"

namespace MM
{
namespace Stmf4
{
    StGpioSettings ir_led_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL, GpioOspeed::MEDIUM, GpioPupd::NO_PULL, };
};
};

namespace MM
{
bool board_init()
{
    // Init AHB1 clock
    // Init APB2 clock
    // Init ADC
    // Init DMA
    return true;
}
};  // namespace MM

// Interrupt Handler for ADC overrun
extern "C" void ADC_IRQHandler()
{
    // Possible ISR for handling ADC OVR
    /*
     * stop timer-triggered conversions
     * stop or disable the ADC/DMA path
     * clear the ADC overrun flag
     * reset your IR sequencing state
     * reset or re-arm DMA
     * turn all LEDs off
     * restart from sensor 0 / ambient sample 1
     */
}

// Restart adc dma stream

// Start adc dma stream

// // Check for overrun (we are going to reinit the DMA stream, clear ADC OVR, and restart ADC conversions in the BSP via OVR interrupt)
//     if (base_addr->SR & ADC_SR_OVR)
//     {
//         // Reinitialize the DMA (adjust destination address and NDTR counter)
//         // Clear the ADC OVR bit in ADC_SR register
//         // Trigger the ADC to start the conversion
//     }