
#ifndef __STM32F4xx_HAL_TIM_H
#define __STM32F4xx_HAL_TIM_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        void* Instance;
        struct
        {
            unsigned int Period;
            unsigned int Prescaler;
            unsigned int ClockDivision;
            unsigned int CounterMode;
            unsigned int AutoReloadPreload;
        } Init;
    } TIM_HandleTypeDef;

#define TIM_COUNTERMODE_UP 0
#define TIM_AUTORELOAD_PRELOAD_DISABLE 0
#define TIM_IT_UPDATE 0

    static inline int HAL_TIM_Base_Init(TIM_HandleTypeDef* htim)
    {
        (void)htim;
        return 0;
    }
    static inline int HAL_TIM_Base_Start_IT(TIM_HandleTypeDef* htim)
    {
        (void)htim;
        return 0;
    }
#define __HAL_TIM_DISABLE_IT(htim, flag) ((void)0)
#define __HAL_TIM_ENABLE_IT(htim, flag) ((void)0)

#ifdef __cplusplus
}
#endif

#endif  // __STM32F4xx_HAL_TIM_H
