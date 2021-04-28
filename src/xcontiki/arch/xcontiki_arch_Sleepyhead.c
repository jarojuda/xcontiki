#include "xcontiki/xcontiki.h"

void xcontiki_arch_Sleepyhead__set_next_wake_up(xcontiki_arch_Clock__time_t time){
    
}

void xcontiki_arch_Sleepyhead__wake_up(void){
    xcontiki_os_sys_Timer__timer_id_t i;
    for(i=1;i<XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER;i++){
    }
}
