// Copyright 2023 3yx3yx (@3yx3yx)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

#define MIDI_ADVANCED
#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 0


//#define ENCODER_DEFAULT_POS 0x3

#define TAP_CODE_DELAY 10
#define MEDIA_KEY_DELAY 10

#define DYNAMIC_KEYMAP_LAYER_COUNT 12


#define ENCODER_MAP_KEY_DELAY 10
#define ENCODERS_PAD_A { C14, B5}
#define ENCODERS_PAD_B { C13, B4}
//#define ENCODER_RESOLUTION 4

#define EEPROM_I2C_24LC256


#ifdef OLED_ENABLE
#    define OLED_DISPLAY_128X32
#define I2C1_SCL_PIN        B6
#define I2C1_SDA_PIN        B7
#define OLED_BRIGHTNESS 128
#define OLED_TIMEOUT 0

#endif
