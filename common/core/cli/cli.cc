#include "cli.h"

#include "../../drivers/time/delay.h"

namespace MM
{

Cli::Cli(CliParams params_)
    : led1(params_.led1),
      led2(params_.led2),
      led3(params_.led3),
      search_bt(params_.search_bt),
      zoom_bt(params_.zoom_bt),
      floodfill(params_.floodfill),
      ir_controller(params_.ir_controller)
{
    floodfill.set_mode(Floodfill::Mode::SEARCH);
    set_leds(true, false, false);
}

bool Cli::run_floodfill_step()
{
    // If no IR controller is wired in this app yet, still run floodfill.
    if (ir_controller == nullptr)
    {
        floodfill.update();
        return true;
    }

    ir_controller->update();
    if (!ir_controller->is_sequence_done())
    {
        return false;
    }

    floodfill.process_ir_data(ir_controller->get_ir_vals());
    // TODO(kent): Replace direct floodfill stepping with a motion pipeline
    // (floodfill next move -> MotionController execute -> completion ack).
    floodfill.update();
    return true;
}

void Cli::set_leds(bool led1_on, bool led2_on, bool led3_on)
{
    led1.set(led1_on);
    led2.set(led2_on);
    led3.set(led3_on);
}

void Cli::update_button(Gpio& button, bool& last_raw_pressed,
                        bool& stable_pressed, bool& press_event,
                        uint32_t& last_change_ms, uint32_t now_ms) const
{
    const bool raw_pressed = kButtonsActiveLow ? !button.read() : button.read();

    if (raw_pressed != last_raw_pressed)
    {
        last_raw_pressed = raw_pressed;
        last_change_ms = now_ms;
    }

    if (((now_ms - last_change_ms) >= kDebounceMs) &&
        (stable_pressed != last_raw_pressed))
    {
        stable_pressed = last_raw_pressed;
        if (stable_pressed)
        {
            press_event = true;
        }
    }
}

bool Cli::consume_press_event(bool& press_event)
{
    if (!press_event)
    {
        return false;
    }
    press_event = false;
    return true;
}

void Cli::update()
{
    const uint32_t now_ms{Utils::get_ms_ticks()};

    update_button(search_bt, search_last_raw_pressed, search_stable_pressed,
                  search_press_event, search_last_change_ms, now_ms);
    update_button(zoom_bt, zoom_last_raw_pressed, zoom_stable_pressed,
                  zoom_press_event, zoom_last_change_ms, now_ms);

    switch (state)
    {
        case State::WAIT_SEARCH_TRIGGER:
            if (consume_press_event(search_press_event))
            {
                state = State::SEARCH_START_DELAY;
                transition_start_ms = now_ms;
            }
            break;

        case State::SEARCH_START_DELAY:
            if ((now_ms - transition_start_ms) >= kStartDelayMs)
            {
                floodfill.set_mode(Floodfill::Mode::SEARCH);
                state = State::SEARCH_ACTIVE;
            }
            break;

        case State::SEARCH_ACTIVE:
            if (run_floodfill_step() && floodfill.is_searched())
            {
                state = State::WAIT_ZOOM_TRIGGER;
            }
            break;

        case State::WAIT_ZOOM_TRIGGER:
            if (consume_press_event(zoom_press_event))
            {
                state = State::ZOOM_START_DELAY;
                transition_start_ms = now_ms;
            }
            break;

        case State::ZOOM_START_DELAY:
            if ((now_ms - transition_start_ms) >= kStartDelayMs)
            {
                floodfill.set_mode(Floodfill::Mode::ZOOMING);
                state = State::ZOOM_ACTIVE;
            }
            break;

        case State::ZOOM_ACTIVE:
            run_floodfill_step();
            break;
    }

    switch (state)
    {
        case State::WAIT_SEARCH_TRIGGER:
        case State::SEARCH_START_DELAY:
        case State::SEARCH_ACTIVE:
            set_leds(true, false, false);
            break;

        case State::WAIT_ZOOM_TRIGGER:
            set_leds(false, false, true);
            break;

        case State::ZOOM_START_DELAY:
        case State::ZOOM_ACTIVE:
            set_leds(false, true, false);
            break;
    }
}

}  // namespace MM
