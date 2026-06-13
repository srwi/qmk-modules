// Copyright 2026 srwi
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>
#include <stdint.h>

// Returns true if `data` was a KeyPeek subscribe/keepalive command (and consumed
// it). Installed automatically; exposed only so a keyboard that defines its own
// via_command_kb / raw_hid_receive_kb (with KEYPEEK_DISABLE_RAW_HID_HANDLER) can
// chain KeyPeek in:  if (keypeek_handle_command(data, length)) return true;
bool keypeek_handle_command(uint8_t *data, uint8_t length);
