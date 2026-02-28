# QMK Modules

This repository contains QMK Community Modules for KeyPeek and other applications.

## Included Modules

- `keypeek_layer_notify`: Sends current/default layer state and key press/release events over Raw HID so KeyPeek can track layer changes and key highlights.

## Install

Add this repository into your userspace `modules/` folder, e.g. as `modules/srwi`:

```sh
git submodule add <repo-url> modules/srwi
git submodule update --init --recursive
```

Then enable the module in your keymap `keymap.json`:

```json
{
  "modules": [
    "srwi/keypeek_layer_notify"
  ]
}
```
