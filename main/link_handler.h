#ifndef LINK_HANDLER_H_
#define LINK_HANDLER_H_

#include "mode.h"

void lh_init(mode_e mode, bool force_pair);
void lh_send_status();

#endif //LINK_HANDLER_H_