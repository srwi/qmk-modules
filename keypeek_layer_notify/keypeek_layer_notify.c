// Copyright 2026 srwi
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "raw_hid.h"
#include "usb_descriptor.h"

#include <string.h>

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

#define KEYPEEK_LAYER_PACKET_MARKER 0xFF
#define KEYPEEK_KEY_PACKET_MARKER 0xF1

static void keypeek_send_layer_state(layer_state_t state) {
#if defined(RAW_ENABLE)
    uint8_t data[RAW_EPSIZE] = {0};
    data[0]                  = KEYPEEK_LAYER_PACKET_MARKER;
    data[1]                  = sizeof(layer_state_t);

    memcpy(&data[2], &default_layer_state, sizeof(layer_state_t));
    memcpy(&data[2 + sizeof(layer_state_t)], &state, sizeof(layer_state_t));

    raw_hid_send(data, RAW_EPSIZE);
#else
    (void)state;
#endif
}

static void keypeek_send_key_event(keyrecord_t *record) {
#if defined(RAW_ENABLE)
    uint8_t data[RAW_EPSIZE] = {0};
    data[0]                  = KEYPEEK_KEY_PACKET_MARKER;
    data[1]                  = record->event.key.row;
    data[2]                  = record->event.key.col;
    data[3]                  = record->event.pressed ? 1 : 0;

    raw_hid_send(data, RAW_EPSIZE);
#else
    (void)record;
#endif
}

layer_state_t layer_state_set_keypeek_layer_notify(layer_state_t state) {
    keypeek_send_layer_state(state);
    return state;
}

bool process_record_keypeek_layer_notify(uint16_t keycode, keyrecord_t *record) {
    (void)keycode;
    keypeek_send_key_event(record);
    return true;
}
