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
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // disabled PLL
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
* @brief Setting the system clock to 24 MHz using an external 8 MHz crystal (HSE) and the PLL.
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
    RCC_OscInitStruct.PLL.PLLM = 8;    // 8 MHz / 8 = 1 MHz
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
* @brief Setting the system clock to 64 MHz using an external 8 MHz crystal (HSE) and the PLL.
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
    RCC_OscInitStruct.PLL.PLLM = 8;    // 8 MHz / 8 = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 512;  // 1 MHz * 512 = 512 MHz
    RCC_OscInitStruct.PLL.PLLP =
        RCC_PLLP_DIV8;  // 512 MHz / 8 = 64 MHz (SYSCLK)
    RCC_OscInitStruct.PLL.PLLQ =
        7;  // 512 MHz / 7 ≈ 73.14 MHz (USB, SDIO, RNG - may not be valid for USB)

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

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        return false;
    }
    return true;
}

bool SystemClock_ConfigHSE8()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        return false;
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;  // using external crystal at 8 MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;  // PLL disabled

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return false;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        return false;
    }
    return true;
}

bool HwClk::init(configuration config)
{
    HAL_Init();
    switch (config)
    {
        case configuration::HSI_16MHZ:
            if (!SystemClock_ConfigHSI16())
                return false;
            source = "HSI 16 MHz";
            hz = 16'000'000;
            break;
        case configuration::SYSCLK_HSE_24MHZ:
            if (!SystemClock_ConfigHSE24())
                return false;
            source = "HSE 24 MHz";
            hz = 24'000'000;
            break;
        case configuration::SYSCLK_HSE_64MHZ:
            if (!SystemClock_ConfigHSE64())
                return false;
            source = "HSE 64 MHz";
            hz = 64'000'000;
            break;
        case configuration::HSE_8MHZ:
            if (!SystemClock_ConfigHSE8())
                return false;
            source = "HSE 8 MHz";
            hz = 8'000'000;
            break;
        default:
            return false;
    }

    return true;
}
}  // namespace MM::Stmf4