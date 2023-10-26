// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include "midi_function_types.h"
#include "process_midi.h"
#include <printf/printf.h>
#include QMK_KEYBOARD_H
extern MidiDevice midi_device;

#define BANK_SEL_MSB_CC 0
#define BANK_SEL_LSB_CC 32

// new midi keycodes
#define MI_CC_TOG_0 0x8000
#define MI_CC_UP_0 (0x8000 + 128)
#define MI_CC_DWN_0 (0x8000 + 128 * 2)
#define MI_CC_0_0 (0x8000 + 128 * 3)
#define MI_BANK_MSB_0 ((0x8000 + 128 * 3) + 128 * 128)
#define MI_BANK_LSB_0 ((0x8000 + 128 * 4) + 128 * 128)
#define MI_PROG_0 ((0x8000 + 128 * 5) + 128 * 128)
#define MI_BANK_UP ((0x8000 + 128 * 6) + 128 * 128 + 1)
#define MI_BANK_DWN ((0x8000 + 128 * 6) + 128 * 128 + 2)
#define MI_PROG_UP ((0x8000 + 128 * 6) + 128 * 128 + 3)
#define MI_PROG_DWN ((0x8000 + 128 * 6) + 128 * 128 + 4)
#define KC_CUSTOM ((0x8000 + 128 * 6) + 128 * 128 + 5)
#define MI_VELOCITY_0 ((0x8000 + 128 * 6) + 128 * 128 + 5)
#define ENCODER_STEP_1 ( (0x8000 + 128 * 7) + 128 * 128 + 5)
#undef KC_CUSTOM
#define KC_CUSTOM (0x8000 + 128 * 7) + 128 * 128 + 5 + 17


// enum custom_keycodes { MY_CUSTOM_KC = KC_CUSTOM, CUSTOM_KC_2, CUSTOM_KC_3 };

static uint8_t  CCValue[128]    = {};
static uint16_t MidiCurrentBank = 0;
static uint8_t  MidiCurrentProg = 0;
static uint8_t  encoder_step = 1;

//char status_str[32] = "";

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    if (!record->event.pressed) {
        return true;
    }

    if (keycode >= MI_CC_TOG_0 && keycode < (MI_CC_TOG_0 + 128)) { // CC TOGGLE
        uint8_t cc = keycode - MI_CC_TOG_0;

        if (CCValue[cc]) {
            CCValue[cc] = 0;
        } else {
            CCValue[cc] = 127;
        }
        midi_send_cc(&midi_device, midi_config.channel, cc, CCValue[cc]);

        //sprintf(status_str, "CC\nTog\n%d", cc);

    } else if (keycode >= MI_CC_UP_0 && keycode < (MI_CC_UP_0 + 128)) { // CC ++
        uint8_t cc = keycode - MI_CC_UP_0;

        if (CCValue[cc] < 127) {
            if (record->event.key.row == KEYLOC_ENCODER_CW) {
                CCValue[cc] += encoder_step;
                if (CCValue[cc] > 127) {
                    CCValue[cc] = 127;
                }
            } else {
                ++CCValue[cc];
            }
        }
        midi_send_cc(&midi_device, midi_config.channel, cc, CCValue[cc]);

        // sprintf(status_str, "CC\nUp\n%d", cc);

    } else if (keycode >= MI_CC_DWN_0 && keycode < (MI_CC_DWN_0 + 128)) { // CC --
        uint8_t cc = keycode - MI_CC_DWN_0;

        if (CCValue[cc] > 0) {
            if (record->event.key.row == KEYLOC_ENCODER_CCW) {
                if (CCValue[cc] >= encoder_step) {
                    CCValue[cc] -= encoder_step;
                } else {
                    CCValue[cc] = 0;
                }

            } else {
                --CCValue[cc];
            }
        }
        midi_send_cc(&midi_device, midi_config.channel, cc, CCValue[cc]);

        //sprintf(status_str, "CC\nDown\n%d", cc);

    } else if (keycode >= MI_CC_0_0 && keycode < (MI_CC_0_0 + 128 * 128)) { // CC FIXED
        uint8_t cc  = (keycode - MI_CC_0_0) / 127;
        uint8_t val = (keycode - MI_CC_0_0) % 127;

        CCValue[cc] = val;
        midi_send_cc(&midi_device, midi_config.channel, cc, CCValue[cc]);

        //sprintf(status_str, "CC\n%d\n%d", cc, val);

    } else if (keycode >= MI_BANK_MSB_0 && keycode < (MI_BANK_MSB_0 + 128)) { // BANK MSB
        uint8_t val = keycode - MI_BANK_MSB_0;
        uint8_t cc  = BANK_SEL_MSB_CC;

        CCValue[cc] = val;
        midi_send_cc(&midi_device, midi_config.channel, cc, CCValue[cc]);

        MidiCurrentBank &= 0x00FF;
        MidiCurrentBank |= val << 8;

        //sprintf(status_str, "MSB\nbank\n%d", val);

    } else if (keycode >= MI_BANK_LSB_0 && keycode < (MI_BANK_LSB_0 + 128)) { // BANK LSB
        uint8_t val = keycode - MI_BANK_LSB_0;
        uint8_t cc  = BANK_SEL_LSB_CC;

        CCValue[cc] = val;
        midi_send_cc(&midi_device, midi_config.channel, cc, CCValue[cc]);

        MidiCurrentBank &= 0xFF00;
        MidiCurrentBank |= val;

        //sprintf(status_str, "LSB\nbank\n%d", val);

    } else if (keycode >= MI_PROG_0 && keycode < (MI_PROG_0 + 128)) { // PROG CHANGE
        uint8_t val = keycode - MI_PROG_0;

        midi_send_programchange(&midi_device, midi_config.channel, val);
        MidiCurrentProg = val;

        //sprintf(status_str, "PC\n%d", val);

    } else if (keycode >= MI_VELOCITY_0 && keycode < (MI_VELOCITY_0 + 128)) {
        uint8_t val = keycode - MI_VELOCITY_0;
        if (val >= 0 && val < 128) midi_config.velocity = val;

    } else if (keycode >= ENCODER_STEP_1 && keycode < (ENCODER_STEP_1 + 16)) {
        uint8_t val = keycode - ENCODER_STEP_1 + 1;
        if (val >= 1 && val < 17) encoder_step = val;
    }

    else {
        uint8_t lsb = 0;
        uint8_t msb = 0;

        switch (keycode) {
            case MI_BANK_UP:
                if (MidiCurrentBank < 0xFFFF) {
                    ++MidiCurrentBank;
                }
                //sprintf(status_str, "bank\n%d", MidiCurrentBank);
                lsb = MidiCurrentBank & 0xFF;
                msb = (MidiCurrentBank & 0xFF00) >> 8;
                midi_send_cc(&midi_device, midi_config.channel, BANK_SEL_LSB_CC, lsb);
                midi_send_cc(&midi_device, midi_config.channel, BANK_SEL_MSB_CC, msb);

                break;
            case MI_BANK_DWN:
                if (MidiCurrentBank > 0) {
                    --MidiCurrentBank;
                }
                //sprintf(status_str, "bank\n%d", MidiCurrentBank);
                uint8_t lsb = MidiCurrentBank & 0xFF;
                uint8_t msb = (MidiCurrentBank & 0xFF00) >> 8;
                midi_send_cc(&midi_device, midi_config.channel, BANK_SEL_LSB_CC, lsb);
                midi_send_cc(&midi_device, midi_config.channel, BANK_SEL_MSB_CC, msb);
                break;
            case MI_PROG_UP:
                if (MidiCurrentProg < 127) {
                    ++MidiCurrentProg;
                }
                //sprintf(status_str, "PC\n%d", MidiCurrentProg);
                midi_send_programchange(&midi_device, midi_config.channel, MidiCurrentProg);
                break;
            case MI_PROG_DWN:
                if (MidiCurrentProg > 0) {
                    --MidiCurrentProg;
                }
                //sprintf(status_str, "PC\n%d", MidiCurrentProg);
                midi_send_programchange(&midi_device, midi_config.channel, MidiCurrentProg);
                break;
            default:
                //sprintf(status_str, "%d", keycode);
                break;
        }
    }

    return true;
}

oled_rotation_t oled_init_kb(oled_rotation_t rotation) { return OLED_ROTATION_270; }

bool oled_task_user(void) {
    uint8_t layer   = get_highest_layer(layer_state);
    char    str[20] = "";
    sprintf(str, "LAYER\n  %-3d\n\n\n", layer);

    oled_write_P(str, false);

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR(" NUM \n") : PSTR("\n"), false);
    oled_write_P(led_state.caps_lock ? PSTR(" CAP \n") : PSTR("\n"), false);
    oled_write_P(led_state.scroll_lock ? PSTR(" SCR \n") : PSTR("\n"), false);

    return false;
}



