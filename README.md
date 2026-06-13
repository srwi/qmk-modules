# QMK Modules

This repository contains QMK Community Modules for KeyPeek and other applications.

## Install

Add this repository into your userspace `modules/` folder, e.g. as `modules/srwi`:

```sh
git submodule add <repo-url> modules/srwi
git submodule update --init --recursive
```

Then enable the module(s) you want in your keymap `keymap.json` (see each
module below for its identifier):

```json
{
  "modules": [
    "srwi/keypeek_layer_notify"
  ]
}
```

## Modules

### `keypeek_layer_notify`

Sends current/default layer state and key press/release events over Raw HID so
KeyPeek can track layer changes and key highlights.

> [!NOTE]
> This module requires **KeyPeek 0.6.0 or newer**. Older releases of KeyPeek will not react to layer changes.

#### Compatibility with custom RAW HID handlers

To receive commands from KeyPeek, the module uses your firmware's RAW HID
handler — `via_command_kb` on QMK or `raw_hid_receive_kb` on Vial. For most keyboards this just works and there's nothing to configure.

The exception is a keyboard that already defines one of those handlers for its
own features. Two pieces of code can't share the same handler, so the firmware
won't build (you'll see a "duplicate symbol" error). If that happens:

1. Add `#define KEYPEEK_DISABLE_RAW_HID_HANDLER` to your `config.h`.
2. Call `keypeek_handle_command()` from your existing handler, for example:

   ```c
   bool via_command_kb(uint8_t *data, uint8_t length) {
       if (keypeek_handle_command(data, length)) {
           return true; // handled by KeyPeek
       }
       // ... your own handling ...
       return false;
   }
   ```
