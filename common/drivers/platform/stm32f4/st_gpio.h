
#ifndef ST_GPIO_H
#define ST_GPIO_H

#include <cstdint>
#include "gpio.h"
#include "mcu_support/stm32/f4xx/stm32f4xx.h"
#include "stm32f411xe.h"

#define ST_GPIO_MAX_PINS 16

namespace MM
{
namespace Stmf4
{

enum class GpioMode : uint8_t
{
    GPI = 0,
    GPOUT,
    AF,
    ANALOG
};

enum class GpioOtype : uint8_t
{
    PUSH_PULL = 0,
    OPEN_DRAIN
};

enum class GpioOspeed : uint8_t
{
    LOW = 0,
    MEDIUM,
    HIGH,
    VERY_HIGH
};

enum class GpioPupd : uint8_t
{
    NO_PULL = 0,
    PULL_UP,
    PULL_DOWN,
    RESERVED
};

struct StGpioSettings
{
    GpioMode mode;
    GpioOtype otype;
    GpioOspeed ospeed;
    GpioPupd pupd;
    uint8_t af;
};

struct StGpioParams
{
    uint8_t pin_num;
    GPIO_TypeDef* base_addr;
    StGpioSettings settings;
};

class HwGpio : public MM::Gpio
{
public:
    explicit HwGpio(const StGpioParams& params);
    /**
    * @brief Initialize GPIO pin
    * @return true if successful, false otherwise
    */
    bool init(void);

    /**
    * @brief toggles pin.
    * @return Returns true if success.
    */
    bool toggle(void);

    /** 
     * @brief Set the pin state.
     * @param active The desired state of the pin.
     * @return Returns true if success.
     */
    bool set(const bool active);

    /**
    * @brief Reads input register.
    * @return Returns bool of the input register.
    */
    bool read(void);

private:
    StGpioSettings settings_;
    const uint8_t pin_num_;
    GPIO_TypeDef* const base_addr_;
};

}  // namespace Stmf4
}  // namespace MM

#endif  // ST_GPIO_H
