#include "xcontiki/xcontiki.h"

void process_1_pollhandler(void);
xcontiki_os_sys_Protothread__state_t process_1_internals(xcontiki_os_sys_Process__event_t, xcontiki_os_sys_Process__data_t);
void process_1_exithandler(void);

void process_2_pollhandler(void);
xcontiki_os_sys_Protothread__state_t process_2_internals(xcontiki_os_sys_Process__event_t, xcontiki_os_sys_Process__data_t);
void process_2_exithandler(void);

void process_3_pollhandler(void);
xcontiki_os_sys_Protothread__state_t  process_3_internals(xcontiki_os_sys_Process__event_t, xcontiki_os_sys_Process__data_t);
void process_3_exithandler(void);
