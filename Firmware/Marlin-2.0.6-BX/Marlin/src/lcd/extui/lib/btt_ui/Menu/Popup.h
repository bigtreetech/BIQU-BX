#ifndef _POPUP_H_
#define _POPUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../API/UI/GUI.h"
#include "../Variants/variants.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

extern const GUI_RECT doubleBtnRect[];
extern BUTTON bottomDoubleBtn[];

void popupDrawPage(DIALOG_TYPE type, BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
void popupReminder(DIALOG_TYPE type, uint8_t* title, uint8_t* msg);
void showDialog(DIALOG_TYPE type, uint8_t * title, uint8_t * msg, uint8_t *ok_txt, uint8_t* cancel_txt, void (*ok_action)(), void (*cancel_action)(), void (*loop_action)());
void loopPopup(void);

#ifdef __cplusplus
}
#endif

#endif
