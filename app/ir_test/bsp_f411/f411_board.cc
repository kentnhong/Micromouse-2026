#include "board.h"
#include "ircontroller.h"
#include "irsensor.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"
#include "st_sys_clk.h"
#include "st_timebase.h"

namespace MM
{
namespace Stmf4
{
// Hardware specific periph object creation
}
// Non-hardware specific object creation and initialization
extern "C" void TIM1_IRQHandler()
{
    // Clear the update interrupt flag
    TIM1->SR &= ~TIM_SR_UIF;

    // IrController update goes here
}
}  // namespace MM