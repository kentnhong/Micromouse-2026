/**
* @file cli.h
* @author Kent Hong
* @brief Command Line Interface (CLI) for the application.
*/

#pragma once

#include <cstdint>
#include "gpio.h"
#include "ircontroller.h"
#include "motioncontroller.h"
#include "nav.h"

/// NOTE: Button-driven state machine that controls search and zoom runs.
/// Navigation + MotionController execute the physical movement pipeline.

namespace MM
{

struct CliParams
{
    Gpio& led1;
    Gpio& led2;
    Gpio& led3;
    Gpio& search_bt;
    Gpio& zoom_bt;
    Navigation& nav;
    MotionController& motion;
    IrController* ir_controller{nullptr};
};

class Cli
{
public:
    explicit Cli(CliParams params_);

    // Poll buttons and drive the nav+motion pipeline.
    void update();

private:
    bool run_nav_step();

    enum class State
    {
        WAIT_SEARCH_TRIGGER,
        SEARCH_START_DELAY,
        SEARCH_ACTIVE,
        WAIT_ZOOM_TRIGGER,
        ZOOM_START_DELAY,
        ZOOM_ACTIVE,
    };

    /**
    * @brief Set the state of the LEDs based on the current CLI state.
    * @param led1_on Whether LED1 should be on.
    * @param led2_on Whether LED2 should be on.
    * @param led3_on Whether LED3 should be on.
    */
    void set_leds(bool led1_on, bool led2_on, bool led3_on);

    /**
    * @brief Update the state of a button and detect press events.
    * @param button Reference to the GPIO pin for the button.
    * @param last_raw_pressed Reference to the previous raw button state.
    * @param stable_pressed Reference to the stable button state.
    * @param press_event Reference to the press event flag.
    * @param last_change_ms Reference to the timestamp of the last button change.
    * @param now_ms Current timestamp in milliseconds.
    */
    void update_button(Gpio& button, bool& last_raw_pressed,
                       bool& stable_pressed, bool& press_event,
                       uint32_t& last_change_ms, uint32_t now_ms) const;

    static bool consume_press_event(bool& press_event);

    Gpio& led1;
    Gpio& led2;
    Gpio& led3;
    Gpio& search_bt;
    Gpio& zoom_bt;
    Navigation& nav;
    MotionController& motion;
    IrController* ir_controller{nullptr};

    /*          Initialization State           */

    State state{State::WAIT_SEARCH_TRIGGER};
    uint32_t transition_start_ms{0};

    bool search_last_raw_pressed{false};
    bool search_stable_pressed{false};
    bool search_press_event{false};
    uint32_t search_last_change_ms{0};

    bool zoom_last_raw_pressed{false};
    bool zoom_stable_pressed{false};
    bool zoom_press_event{false};
    uint32_t zoom_last_change_ms{0};

    static constexpr uint32_t kDebounceMs{30};
    static constexpr uint32_t kStartDelayMs{500};
    static constexpr bool kButtonsActiveLow{true};
};
}  // namespace MM
