#include QMK_KEYBOARD_H
#include "translate_ansi_to_jis.h"

#define HANDLED false
#define NOT_HANDLED true

const uint16_t translate_map[][2] = {
  // ANSI   JIS
  {KC_EQL,  LSFT(KC_MINS)}, // 187
  {KC_LBRC, KC_RBRC      }, // 219
  {KC_BSLS, KC_INT3      }, // 220
  {KC_RBRC, KC_NUHS      }, // 221
  {KC_QUOT, LSFT(KC_7)   }, // 222
  {KC_GRV,  LSFT(KC_LBRC)}, // 192
  {KC_RPRN, LSFT(KC_9)   }, //  48 + 512
  {KC_AT,   KC_LBRC      }, //  50 + 512
  {KC_CIRC, KC_EQL       }, //  54 + 512
  {KC_AMPR, LSFT(KC_6)   }, //  55 + 512
  {KC_ASTR, LSFT(KC_QUOT)}, //  56 + 512
  {KC_LPRN, LSFT(KC_8)   }, //  57 + 512
  {KC_PLUS, LSFT(KC_SCLN)}, // 187 + 512
  {KC_UNDS, LSFT(KC_INT1)}, // 189 + 512
  {KC_LCBR, LSFT(KC_RBRC)}, // 219 + 512
  {KC_PIPE, LSFT(KC_INT3)}, // 220 + 512
  {KC_RCBR, LSFT(KC_NUHS)}, // 221 + 512
  {KC_COLN, KC_QUOT      }, // 221 + 512
  {KC_DQT,  LSFT(KC_2)   }, // 221 + 512
  {KC_TILD, LSFT(KC_EQL) }, // 221 + 512
};
const size_t rows = sizeof(translate_map) / sizeof(translate_map[0]);

uint16_t find(uint16_t kc) {
  for(size_t index = 0; index < rows; index ++) {
    if (translate_map[index][0] == kc) return translate_map[index][1];
  }
  return -1;
}

// Assumes that multiple symbolic keys will never be used at the same time.
uint16_t ansi_keycode_of_registered_key = 0;

bool process_record_user_a2j(uint16_t mods_and_keycode, keyrecord_t *record) {
  uint8_t mod_state = get_mods();

  uint8_t mods_from_keycode = mods_and_keycode >> 8; // Drop keycode bits
  uint8_t keycode = mods_and_keycode; // Drop mod bits

  if (record->event.pressed) {
    bool shifted = (mod_state | mods_from_keycode) & MOD_MASK_SHIFT;
    uint16_t ansi_keycode = shifted ? LSFT((uint16_t) keycode) : keycode;
    uint16_t translated_keycode = find(ansi_keycode);
    if (translated_keycode == -1) {
      return NOT_HANDLED;
    }

    if (ansi_keycode_of_registered_key != 0) {
      // unregister the previous symbolic key
      unregister_code16(find(ansi_keycode_of_registered_key));
    }
    ansi_keycode_of_registered_key = ansi_keycode;

    if (mod_state & MOD_MASK_SHIFT) {
      del_mods(MOD_MASK_SHIFT);
      register_code16(translated_keycode);
      set_mods(mod_state);
    } else {
      register_code16(translated_keycode);
    }
    return HANDLED;
  } else { // released
    // if keycode is the same as previous one without mods.
    if (keycode == (uint8_t) ansi_keycode_of_registered_key) {
      unregister_code16(find(ansi_keycode_of_registered_key));
      ansi_keycode_of_registered_key = 0;
      return HANDLED;
    } else {
      return NOT_HANDLED;
    }
  }
}
