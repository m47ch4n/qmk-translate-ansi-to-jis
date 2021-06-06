#include QMK_KEYBOARD_H
#include "translate_ansi_to_jis.h"

const uint16_t translate_map[][5] = {
// ANSI        JIS Shifted       JIS Not Shifted
  {KC_0   ,    KC_9   , SHFT,    KC_0   , USFT}, //  48
  {KC_2   ,    KC_LBRC, USFT,    KC_2   , USFT}, //  50
  {KC_6   ,    KC_EQL , USFT,    KC_6   , USFT}, //  54
  {KC_7   ,    KC_6   , SHFT,    KC_7   , USFT}, //  55
  {KC_8   ,    KC_QUOT, SHFT,    KC_8   , USFT}, //  56
  {KC_9   ,    KC_8   , SHFT,    KC_9   , USFT}, //  57
  {KC_SCLN,    KC_QUOT, USFT,    KC_SCLN, USFT}, // 186
  {KC_EQL ,    KC_SCLN, SHFT,    KC_MINS, SHFT}, // 187
  {KC_MINS,    KC_INT1, SHFT,    KC_MINS, USFT}, // 189
  {KC_LBRC,    KC_RBRC, SHFT,    KC_RBRC, USFT}, // 219
  {KC_BSLS,    KC_INT3, SHFT,    KC_INT3, USFT}, // 220
  {KC_RBRC,    KC_NUHS, SHFT,    KC_NUHS, USFT}, // 221
  {KC_QUOT,    KC_2   , SHFT,    KC_7   , SHFT}, // 222
  {KC_GRV ,    KC_EQL , SHFT,    KC_LBRC, SHFT}, // 192
  {KC_RPRN,    KC_9   , SHFT,    KC_9   , SHFT}, //  48 + 512
  {KC_AT  ,    KC_LBRC, USFT,    KC_LBRC, USFT}, //  50 + 512
  {KC_CIRC,    KC_EQL , USFT,    KC_EQL , USFT}, //  54 + 512
  {KC_AMPR,    KC_6   , SHFT,    KC_6   , SHFT}, //  55 + 512
  {KC_ASTR,    KC_QUOT, SHFT,    KC_QUOT, SHFT}, //  56 + 512
  {KC_LPRN,    KC_8   , SHFT,    KC_8   , SHFT}, //  57 + 512
  {KC_PLUS,    KC_SCLN, SHFT,    KC_SCLN, SHFT}, // 187 + 512
  {KC_UNDS,    KC_INT1, SHFT,    KC_INT1, SHFT}, // 189 + 512
  {KC_LCBR,    KC_RBRC, SHFT,    KC_RBRC, SHFT}, // 219 + 512
  {KC_PIPE,    KC_INT3, SHFT,    KC_INT3, SHFT}, // 220 + 512
  {KC_RCBR,    KC_NUHS, SHFT,    KC_NUHS, SHFT}, // 221 + 512
};
const size_t rows = sizeof(translate_map) / sizeof(translate_map[0]);

size_t find(uint16_t kc) {
  size_t index = 0;
  while (index < rows && translate_map[index][0] != kc)
    index ++;
  return index == rows ? -1 : index;
}

void mod_shift(uint8_t mod_shift) {
  if (mod_shift == SHFT)
    set_mods(MOD_BIT(KC_LSFT));
  else
    del_mods(MOD_BIT(KC_LSFT));
}

static uint64_t key_registered_bits = 0x0000;

void translate(bool pressed, uint8_t mod_state, size_t index) {
  if (pressed) {
    bool offset = !(mod_state & MOD_MASK_SHIFT);
    mod_shift(translate_map[index][2 + offset * 2]);
    register_code(translate_map[index][1 + offset * 2]);
    set_mods(mod_state);
    key_registered_bits |= (uint64_t)1 << (index*2 + offset);
  } else {
    for (uint8_t i = 0; i < 2; i ++) {
      uint64_t bit_mask = (uint64_t)1 << (index*2 + i);
      if (key_registered_bits & bit_mask) {
        unregister_code(translate_map[index][1 + i * 2]);
        key_registered_bits ^= bit_mask;
      }
    }
  }
}

bool process_record_user_a2j(uint16_t keycode, keyrecord_t *record) {
  size_t index = find(keycode);
  if (index == -1)
    return true;

  translate(record->event.pressed, get_mods(), index);
  return false;
}
