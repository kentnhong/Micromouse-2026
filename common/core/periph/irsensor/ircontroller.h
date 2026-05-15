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

enum class IrControllerStates : uint8_t
{
    LEFT = 0,
    FRONT_LEFT,
    FRONT_RIGHT,
    RIGHT
};
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
    std::array<IrSensor*, 4>& ir_sequence;
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
     * @brief Reset the IR Controller Sequence to handle corrupted or missing data
     * 
     * @return true IR Controller reset success, false otherwise
     */
    bool reset();

    /**
     * @brief Get a struct of the current ir sensor vals for one sequence
     * 
     * @return IrValues& 
     */
    const IrValues& get_ir_vals() const;

    /**
     * @brief Checks if IR Sensor Sequence is finished (went through all 4 sensors)
     * 
     * @return true sequence finished, false otherwise
     */
    bool is_sequence_done();

    /**
     * @brief IR Controller Destructor
     * 
     */
    ~IrController() = default;

private:
    std::array<IrSensor*, 4>&
        ir_sequence;    // Store references as pointers to 4 IR Sensors
    IrValues& ir_vals;  // Store ir vals for all 4 sensors
    IrControllerStates current_state = IrControllerStates::LEFT;
    bool sequence_done = false;
};
}  // namespace MM
