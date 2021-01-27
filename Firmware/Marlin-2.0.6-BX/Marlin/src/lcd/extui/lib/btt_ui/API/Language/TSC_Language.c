#include "TSC_Language.h"
#include "../../includes.h"
#include "tsc_language_en.h"
#include "tsc_language_cn.h"
#include "tsc_language_ru.h"
#include "tsc_language_jp.h"
#include "tsc_language_de.h"
#include "tsc_language_am.h"
#include "tsc_language_cz.h"
#include "tsc_language_es.h"
#include "tsc_language_fr.h"
#include "tsc_language_pt.h"
#include "tsc_language_it.h"
#include "tsc_language_pl.h"
#include "tsc_language_sk.h"
#include "tsc_language_du.h"
#include "tsc_language_hu.h"
#include "tsc_language_tr.h"
#include "tsc_language_gr.h"
#include "tsc_language_sl.h"
#include "tsc_language_ca.h"
#include "tsc_language_tc.h"
//
// Add new Keywords in Language.inc file Only
//
const char *const en_pack[LABEL_NUM] = {
  #define X_WORD(NAME) EN_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const cn_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CN_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const ru_pack[LABEL_NUM] = {
  #define X_WORD(NAME) RU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const jp_pack[LABEL_NUM] = {
  #define X_WORD(NAME) JP_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const am_pack[LABEL_NUM] = {
  #define X_WORD(NAME) AM_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const de_pack[LABEL_NUM] = {
  #define X_WORD(NAME) DE_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const cz_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CZ_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const es_pack[LABEL_NUM] = {
  #define X_WORD(NAME) ES_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const fr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) FR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const pt_pack[LABEL_NUM] = {
  #define X_WORD(NAME) PT_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const it_pack[LABEL_NUM] = {
  #define X_WORD(NAME) IT_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const pl_pack[LABEL_NUM] = {
  #define X_WORD(NAME) PL_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const sk_pack[LABEL_NUM] = {
  #define X_WORD(NAME) SK_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const du_pack[LABEL_NUM] = {
  #define X_WORD(NAME) DU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const hu_pack[LABEL_NUM] = {
  #define X_WORD(NAME) HU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const tr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) TR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const gr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) GR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const sl_pack[LABEL_NUM] = {
  #define X_WORD(NAME) SL_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const ca_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CA_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const tc_pack[LABEL_NUM] = {
  #define X_WORD(NAME) TC_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

uint8_t *textSelect(uint16_t sel)
{
  switch(infoSettings.language)
  {
    case TSC_ENGLISH:       return (uint8_t *)en_pack[sel];
    case TSC_CHINESE:       return (uint8_t *)cn_pack[sel];
    case TSC_RUSSIAN:       return (uint8_t *)ru_pack[sel];
    case TSC_JAPANESE:      return (uint8_t *)jp_pack[sel];
    case TSC_ARMENIAN:      return (uint8_t *)am_pack[sel];
    case TSC_GERMAN:        return (uint8_t *)de_pack[sel];
    case TSC_CZECH:         return (uint8_t *)cz_pack[sel];
    case TSC_SPAIN:         return (uint8_t *)es_pack[sel];
    case TSC_FRENCH:        return (uint8_t *)fr_pack[sel];
    case TSC_PORTUGUESE:    return (uint8_t *)pt_pack[sel];
    case TSC_ITALIAN:       return (uint8_t *)it_pack[sel];
    case TSC_POLISH:        return (uint8_t *)pl_pack[sel];
    case TSC_SLOVAK:        return (uint8_t *)sk_pack[sel];
    case TSC_DUTCH:         return (uint8_t *)du_pack[sel];
    case TSC_HUNGARIAN:     return (uint8_t *)hu_pack[sel];
    case TSC_TURKISH:       return (uint8_t *)tr_pack[sel];
    case TSC_GREEK:         return (uint8_t *)gr_pack[sel];
    case TSC_SLOVENIAN:     return (uint8_t *)sl_pack[sel];
    case TSC_CATALAN:       return (uint8_t *)ca_pack[sel];
    case TSC_TRAD_CHINESE:  return (uint8_t *)tc_pack[sel];

    default:                return NULL;
  }
}
