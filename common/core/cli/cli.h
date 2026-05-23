/**
* @file cli.h
* @author Kent Hong
* @brief Command Line Interface (CLI) for the application.
*/

#include "floodfill.h"
#include "gpio.h"

/// NOTE: This is the CLI interface where the push button dictate the state of the system.
///        The CLI will be used to control the state of the system

#pragma once

#include <cstdint>

namespace MM
{

struct CliParams
{
    Gpio& led1;
    Gpio& led2;
    Gpio& led3;
    Gpio& search_bt;
    Gpio& zoom_bt;
    Floodfill& floodfill;
    IrController* ir_controller{nullptr};
};

class Cli
{
public:
    explicit Cli(CliParams params_);

    // Poll buttons and update floodfill mode/LED state.
    void update();

private:
    bool run_floodfill_step();

    enum class State
    {
        WAIT_SEARCH_TRIGGER,
        SEARCH_START_DELAY,
        SEARCH_ACTIVE,
        WAIT_ZOOM_TRIGGER,
        ZOOM_START_DELAY,
        ZOOM_ACTIVE,
    };

    void set_leds(bool led1_on, bool led2_on, bool led3_on);
    void update_button(Gpio& button, bool& last_raw_pressed,
                       bool& stable_pressed, bool& press_event,
                       uint32_t& last_change_ms, uint32_t now_ms) const;
    static bool consume_press_event(bool& press_event);

    Gpio& led1;
    Gpio& led2;
    Gpio& led3;
    Gpio& search_bt;
    Gpio& zoom_bt;
    Floodfill& floodfill;
    IrController* ir_controller{nullptr};  // Added ir_controller to the class

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