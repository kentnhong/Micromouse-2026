/**
* @brief Generic Header for System Clock Drivers
* @file sys_clk.h
* @author Bex Saw
* @date 2/19/26
*/

#pragma once 
#include <cstdint>

namespace MM 
{
/**
 * @class Clock
 * @brief System Clock Driver Class
 * @note  The system clock can switch to an external 4; 26 MHz source, 
 *        monitored for failure. If the external clock fails, it reverts 
 *        to the internal RC and triggers a software interrupt. The clock 
 *        feeds a PLL that can boost frequency up to 100 MHz. Interrupt management 
 *        is available for PLL or oscillator faults. Prescalers configure two AHB buses (max 100 MHz), 
 *        high-speed APB2 (max 100 MHz), and low-speed APB1 (max 50 MHz). A dedicated PLLI2S generates 
 *        I2S master clocks supporting 8 kHz–192 kHz audio sampling rates.
*/

class Clock
{



}

} // namespace MM