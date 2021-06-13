#include QMK_KEYBOARD_H
#include "translate_ansi_to_jis.h"

uint16_t find(uint16_t kc) {
  // case ANSI: return JIS; break;
  switch (kc) {
    case KC_EQL:  return LSFT(KC_MINS); // 187
    case KC_LBRC: return KC_RBRC; // 219
    case KC_BSLS: return KC_INT3; // 220
    case KC_RBRC: return KC_NUHS; // 221
    case KC_QUOT: return LSFT(KC_7); // 222
    case KC_GRV:  return LSFT(KC_LBRC); // 192
    case KC_RPRN: return LSFT(KC_9); //  48 + 512
    case KC_AT:   return KC_LBRC; //  50 + 512
    case KC_CIRC: return KC_EQL; //  54 + 512
    case KC_AMPR: return LSFT(KC_6); //  55 + 512
    case KC_ASTR: return LSFT(KC_QUOT); //  56 + 512
    case KC_LPRN: return LSFT(KC_8); //  57 + 512
    case KC_PLUS: return LSFT(KC_SCLN); // 187 + 512
    case KC_UNDS: return LSFT(KC_INT1); // 189 + 512
    case KC_LCBR: return LSFT(KC_RBRC); // 219 + 512
    case KC_PIPE: return LSFT(KC_INT3); // 220 + 512
    case KC_RCBR: return LSFT(KC_NUHS); // 221 + 512
    case KC_COLN: return KC_QUOT; // 221 + 512
    case KC_DQT:  return LSFT(KC_2); // 221 + 512
    case KC_TILD: return LSFT(KC_EQL); // 221 + 512
    default: return -1;
  }
}

bool process_record_user_a2j(uint16_t mods_and_keycode, keyrecord_t *record) {
  uint8_t mod_state = get_mods();

  uint8_t mods_from_keycode = mods_and_keycode >> 8; // Drop keycode bits
  uint8_t keycode = mods_and_keycode; // Drop mod bits

  bool shifted = (mod_state | mods_from_keycode) & MOD_MASK_SHIFT;
  uint16_t target_keycode = shifted ? LSFT((uint16_t) keycode) : keycode;
  uint16_t translated_keycode = find(target_keycode);
  if (translated_keycode == -1) {
    return true;
  }

  if (record->event.pressed) {
    if (mod_state & MOD_MASK_SHIFT) {
      del_mods(MOD_MASK_SHIFT);
      register_code16(translated_keycode);
      set_mods(mod_state);
    } else {
      register_code16(translated_keycode);
    }
  } else {
    unregister_code16(translated_keycode);
  }
  return false;
}
