/**
 * @file ircontroller.h
 * @author Kent Hong
 * @brief IR Controller interface that controls 4 IR Sensors in sequence on the Xupermouse
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "irsensor.h"

namespace MM
{

// Struct for storing IR Sensor values
struct IrValues
{
    uint16_t left = 0;
    uint16_t front_left = 0;
    uint16_t front_right = 0;
    uint16_t right = 0;
};

struct IrControllerParams
{
    std::array<IrSensor, 4>& ir_sequence;
    IrValues& ir_vals;
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
     * @brief Get a struct of the current ir sensor vals for one sequence
     * 
     * @return IrValues& 
     */
    IrValues& get_ir_vals() const;

    /**
     * @brief IR Controller Destructor
     * 
     */
    ~IrController() = default;

private:
    // Store reference to 4 IR Sensors
    std::array<IrSensor, 4>& ir_sequence;
    IrValues& ir_vals;
};
}  // namespace MM
