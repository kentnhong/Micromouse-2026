#include "st_sys_clk.h"

extern "C"
{
    extern void IncDelayTicks(void);

    void SysTick_Handler()
    {
        IncDelayTicks();
        HAL_IncTick();
    }
};

namespace MM::Stmf4
{
HwClk::HwClk(Configuration config) : hz(0), config(config)
{
}

/**
* @brief Setting the system clock to 16 MHz using the internal HSI oscillator. 
*       (when reset occurs, the system clock is set to HSI at 16 MHz by default, 
*       -but this function explicitly configures it and disables the PLL)
*/
bool SystemClock_ConfigHSI16()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        return false;
    }

    /* Initializes the RCC Oscillators according to the specified parameters
    in the RCC_OscInitTypeDef structure. */

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // enabled PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;   // 16 MHz / 16 = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 336;  // 1 MHz * 336 = 336 MHz
    RCC_OscInitStruct.PLL.PLLP =
        RCC_PLLP_DIV4;               // 336 MHz / 4 = 84 MHz (SYSCLK)
    RCC_OscInitStruct.PLL.PLLQ = 7;  // 336 MHz / 7 = 48 MHz (USB, SDIO, RNG)

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        return false;
    }
    return true;
}

/**
* @brief Setting the system clock to 24 MHz using an external 24 MHz crystal (HSE) and the PLL.
*        This is a common configuration for STM32F4 boards, providing a stable clock source.
*/
bool SystemClock_ConfigHSE24()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        return false;
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState =
        RCC_HSE_BYPASS;  // using external clock source (e.g., from ST-Link) instead of crystal
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;  // enabled PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 24;   // 24 MHz / 24 = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 192;  // 1 MHz * 192 = 192 MHz
    RCC_OscInitStruct.PLL.PLLP =
        RCC_PLLP_DIV8;               // 192 MHz / 8 = 24 MHz (SYSCLK)
    RCC_OscInitStruct.PLL.PLLQ = 4;  // 192 MHz / 4 = 48 MHz (USB, SDIO, RNG)

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        return false;
    }
    return true;
}

/**
* @brief Setting the system clock to 64 MHz using an external 24 MHz crystal (HSE) and the PLL.
*        This configuration provides higher performance for the system.
*/
bool SystemClock_ConfigHSE64()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        return false;
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState =
        RCC_HSE_BYPASS;  // using external clock source (e.g., from ST-Link) instead of crystal
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;  // enabled PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 24;   // 24 MHz / 24 = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 384;  // 1 MHz * 384 = 384 MHz
    RCC_OscInitStruct.PLL.PLLP =
        RCC_PLLP_DIV6;               // 4384 MHz / 6 = 64 MHz (SYSCLK)
    RCC_OscInitStruct.PLL.PLLQ = 8;  // 384 MHz / 8 = 48 MHz (USB, SDIO, RNG)

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV2;  // 64 MHz / 2 = 32 MHz for PCLK1 or APB1 clock
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        return false;
    }
    return true;
}

/**
* @brief Setting the system clock to 100 MHz using an external 24 MHz crystal (HSE) and the PLL.
*        This configuration provides the max performance for the system.
*/
bool SystemClock_ConfigHSE100()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        return false;
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState =
        RCC_HSE_BYPASS;  // using external clock source (e.g., from ST-Link) instead of crystal
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;  // enabled PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 24;   // 24 MHz / 24 = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 400;  // 1 MHz * 400 = 400 MHz
    RCC_OscInitStruct.PLL.PLLP =
        RCC_PLLP_DIV4;  // 400 MHz / 4 = 100 MHz (SYSCLK)
    RCC_OscInitStruct.PLL.PLLQ =
        10;  // 400 MHz / 10 = 40 MHz (USB, SDIO, RNG - not valid for USB OTG FS which requuires 48MHz)

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        return false;
    }
    return true;
}

bool HwClk::init()
{
    HAL_Init();
    switch (config)
    {
        case Configuration::HSI_16MHZ:
            if (!SystemClock_ConfigHSI16())
                return false;
            config = Configuration::HSI_16MHZ;
            hz = 16'000'000;
            break;
        case Configuration::SYSCLK_HSE_24MHZ:
            if (!SystemClock_ConfigHSE24())
                return false;
            config = Configuration::SYSCLK_HSE_24MHZ;
            hz = 24'000'000;
            break;
        case Configuration::SYSCLK_HSE_64MHZ:
            if (!SystemClock_ConfigHSE64())
                return false;
            config = Configuration::SYSCLK_HSE_64MHZ;
            hz = 64'000'000;
            break;
        case Configuration::SYSCLK_HSE_100MHZ:
            if (!SystemClock_ConfigHSE100())
                return false;
            config = Configuration::SYSCLK_HSE_100MHZ;
            hz = 100'000'000;
            break;
        default:
            return false;
    }

    return true;
}
}  // namespace MM::Stmf4
