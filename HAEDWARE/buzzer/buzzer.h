#ifndef __BUZZER_H
#define __BUZZER_H

#include "sys.h"

#define beep PEout(5)

void Buzzer_Init(void);
void Buzzer_s(u8 second);
#endif

