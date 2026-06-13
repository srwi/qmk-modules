// Copyright 2026 srwi
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "keypeek_layer_notify.h"
#include "raw_hid.h"
#include "usb_descriptor.h"
#include "timer.h"

#include <string.h>

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

#define KEYPEEK_LAYER_PACKET_MARKER 0xFF
#define KEYPEEK_KEY_PACKET_MARKER 0xF1

#define KEYPEEK_SUBSCRIBE_MARKER 0xC0
#define KEYPEEK_SUBSCRIBE_ACTIVE 0xA1
#define KEYPEEK_SUBSCRIBE_INACTIVE 0xA0
#define KEYPEEK_SUBSCRIBE_TIMEOUT_MS 3000

static bool     keypeek_subscribed = false;
static uint32_t keypeek_last_ping  = 0;

static bool keypeek_is_active(void) {
    return keypeek_subscribed && timer_elapsed32(keypeek_last_ping) < KEYPEEK_SUBSCRIBE_TIMEOUT_MS;
}

static void keypeek_send_layer_state(layer_state_t state) {
#if defined(RAW_ENABLE)
    if (!keypeek_is_active()) {
        return;
    }

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
    if (!keypeek_is_active()) {
        return;
    }

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

// Returns true if the command was a KeyPeek subscribe/keepalive (and consumed).
bool keypeek_handle_command(uint8_t *data, uint8_t length) {
    if (length < 1 || data[0] != KEYPEEK_SUBSCRIBE_MARKER) {
        return false;
    }

    bool want_active = (length < 2) || (data[1] != KEYPEEK_SUBSCRIBE_INACTIVE);
    bool was_active  = keypeek_is_active();

    keypeek_subscribed = want_active;
    keypeek_last_ping  = timer_read32();

    if (want_active && !was_active) {
        keypeek_send_layer_state(layer_state);
    }

    return true;
}

// RAW HID receive has no community-module hook, so we override the firmware's
// weak handler. A keyboard that already defines it can set
// KEYPEEK_DISABLE_RAW_HID_HANDLER and call keypeek_handle_command() itself.
#if !defined(KEYPEEK_DISABLE_RAW_HID_HANDLER)
#    if defined(VIAL_ENABLE)
// Vial routes unknown commands here; chain onward to the keymap handler.
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (!keypeek_handle_command(data, length)) {
        raw_hid_receive_user(data, length);
    }
}
#    elif defined(VIA_ENABLE)
// Mainline VIA: returning true consumes the command with no response.
bool via_command_kb(uint8_t *data, uint8_t length) {
    return keypeek_handle_command(data, length);
}
#    endif
#endif

layer_state_t layer_state_set_keypeek_layer_notify(layer_state_t state) {
    keypeek_send_layer_state(state);
    return state;
}

bool process_record_keypeek_layer_notify(uint16_t keycode, keyrecord_t *record) {
    (void)keycode;
    keypeek_send_key_event(record);
    return true;
}
