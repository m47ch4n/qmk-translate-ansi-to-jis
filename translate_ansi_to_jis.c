#include QMK_KEYBOARD_H
#include "translate_ansi_to_jis.h"

uint8_t mod_state;

typedef enum {
  SHFT,
  USFT
} mod_shift_t;

void mod_shift(uint8_t mod_shift) {
  switch (mod_shift) {
    case SHFT:
      set_mods(MOD_BIT(KC_LSFT));
      break;
    case USFT:
      del_mods(MOD_BIT(KC_LSFT));
      break;
    default:
      // invalid mod shift type
      break;
  }
}

void translate_shifted(
  keyrecord_t *record,
  uint16_t position_key,
  mod_shift_t position_mod,
  bool *key_registered) {
  if (record->event.pressed) {
    mod_shift(position_mod);
    register_code(position_key);
    set_mods(mod_state);
    *key_registered = true;
  } else {
    if (*key_registered) {
      unregister_code(position_key);
      *key_registered = false;
    }
  }
}

#define TRANSLATE_SHIFTED(POS, POS_MOD) \
  { \
    static bool key_registered; \
    translate_shifted(record, POS, POS_MOD, &key_registered); \
    return false; \
  }

void translate(
  keyrecord_t *record,
  uint16_t down_position_key,
  mod_shift_t down_position_mod,
  bool *key_with_shift_registered,
  uint16_t up_position_key,
  mod_shift_t up_position_mod,
  bool *key_without_shift_registered) {
  if (record->event.pressed) {
    if (mod_state & MOD_MASK_SHIFT) {
      mod_shift(down_position_mod);
      register_code(down_position_key);
      *key_with_shift_registered = true;
    } else {
      mod_shift(up_position_mod);
      register_code(up_position_key);
      *key_without_shift_registered = true;
    }
    set_mods(mod_state);
  } else {
    if (*key_with_shift_registered) {
      unregister_code(down_position_key);
      *key_with_shift_registered = false;
    } else if (*key_without_shift_registered) {
      unregister_code(up_position_key);
      *key_without_shift_registered = false;
    }
  }
}

#define TRANSLATE(DN_POS_KEY, DN_POS_MOD, UP_POS_KEY, UP_POS_MOD) \
  { \
    static bool key_with_shift_registered; \
    static bool key_without_shift_registered; \
    translate(record, \
      DN_POS_KEY, DN_POS_MOD, &key_with_shift_registered, \
      UP_POS_KEY, UP_POS_MOD, &key_without_shift_registered); \
    return false; \
  }

bool process_record_user_taj(uint16_t keycode, keyrecord_t *record) {
  mod_state = get_mods();

  switch (keycode) {
    case KC_2   : TRANSLATE(KC_LBRC, USFT,    KC_2   , USFT);
    case KC_6   : TRANSLATE(KC_EQL , USFT,    KC_6   , USFT);
    case KC_7   : TRANSLATE(KC_6   , SHFT,    KC_7   , USFT);
    case KC_8   : TRANSLATE(KC_QUOT, SHFT,    KC_8   , USFT);
    case KC_9   : TRANSLATE(KC_8   , SHFT,    KC_9   , USFT);
    case KC_0   : TRANSLATE(KC_9   , SHFT,    KC_0   , USFT);
    case KC_MINS: TRANSLATE(KC_INT1, SHFT,    KC_MINS, USFT);
    case KC_EQL : TRANSLATE(KC_SCLN, SHFT,    KC_MINS, SHFT);
    case KC_LBRC: TRANSLATE(KC_RBRC, SHFT,    KC_RBRC, USFT);
    case KC_RBRC: TRANSLATE(KC_NUHS, SHFT,    KC_NUHS, USFT);
    case KC_BSLS: TRANSLATE(KC_INT3, SHFT,    KC_INT3, USFT);
    case KC_SCLN: TRANSLATE(KC_QUOT, USFT,    KC_SCLN, USFT);
    case KC_QUOT: TRANSLATE(KC_2   , SHFT,    KC_7   , SHFT);
    case KC_GRV : TRANSLATE(KC_EQL , SHFT,    KC_LBRC, SHFT);
    case KC_AT  : TRANSLATE_SHIFTED(KC_LBRC, USFT);
    case KC_CIRC: TRANSLATE_SHIFTED(KC_EQL , USFT);
    case KC_AMPR: TRANSLATE_SHIFTED(KC_6   , SHFT);
    case KC_ASTR: TRANSLATE_SHIFTED(KC_QUOT, SHFT);
    case KC_LPRN: TRANSLATE_SHIFTED(KC_8   , SHFT);
    case KC_RPRN: TRANSLATE_SHIFTED(KC_9   , SHFT);
    case KC_PIPE: TRANSLATE_SHIFTED(KC_INT3, SHFT);
    case KC_LCBR: TRANSLATE_SHIFTED(KC_RBRC, SHFT);
    case KC_RCBR: TRANSLATE_SHIFTED(KC_NUHS, SHFT);
    case KC_UNDS: TRANSLATE_SHIFTED(KC_INT1, SHFT);
    case KC_PLUS: TRANSLATE_SHIFTED(KC_SCLN, SHFT);
    default:
      return true;
  }
}
