/**
 * @file ircontroller.h
 * @author Kent Hong
 * @brief IR Controller interface that controls 4 IR Sensors in sequence on the Xupermouse
 */
#pragma once

#include <array>
#include <cstdint>
#include "irsensor.h"

namespace MM
{

struct IrControllerParams
{
    std::array<IrSensor*, 4> ir_sequence;
};

class IrController
{
public:
    /**
     * @brief IR Controller Constructor
     * 
     * @param params_ A struct of an array of your IrSensor objects
     */
    explicit IrController(IrControllerParams& params_);

    /**
     * @brief Wrapper for the update() function in IrSensor that switches to different IR Sensor after one finishes its states.
     * 
     * @return true update success, false otherwise
     */
    bool update();

    /**
     * @brief IR Controller Destructor
     * 
     */
    ~IrController() = default;

private:
    // Store reference to 4 IR Sensors
    std::array<IrSensor*, 4> ir_sequence;
};
}  // namespace MM
