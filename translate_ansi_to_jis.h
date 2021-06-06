// Keycodes to use jis keycode in us keymap

#pragma once

enum mod_shift {
  SHFT = 0,
  USFT
};

bool process_record_user_a2j(uint16_t keycode, keyrecord_t *record);
