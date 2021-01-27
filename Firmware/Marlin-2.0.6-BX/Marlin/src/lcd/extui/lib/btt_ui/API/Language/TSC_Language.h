#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "../../Variants/variants.h"

enum
{
  TSC_ENGLISH = 0,
  TSC_CHINESE,
  TSC_RUSSIAN,
  TSC_JAPANESE,
  TSC_ARMENIAN,
  TSC_GERMAN,
  TSC_CZECH,
  TSC_SPAIN,
  TSC_FRENCH,
  TSC_PORTUGUESE,
  TSC_ITALIAN,
  TSC_POLISH,
  TSC_SLOVAK,
  TSC_DUTCH,
  TSC_HUNGARIAN,
  TSC_TURKISH,
  TSC_GREEK,
  TSC_SLOVENIAN,
  TSC_CATALAN,
  TSC_TRAD_CHINESE,
  
  TSC_LANGUAGE_NUM,
};

enum {
#define X_WORD(NAME) LABEL_##NAME ,
#include "Language.inc"
#undef  X_WORD

  //add new keywords in 'Language.inc' file only
  //keep the following always at the end of this list
  LABEL_NUM,
  LABEL_BACKGROUND,
  LABEL_DYNAMIC,
  LABEL_CUSTOM_VALUE,
};

uint8_t * textSelect(uint16_t sel);

#ifdef __cplusplus
}
#endif

#endif
