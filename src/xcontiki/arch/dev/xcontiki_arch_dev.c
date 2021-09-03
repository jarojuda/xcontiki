#include "xcontiki/xcontiki.h"


#if(XCONTIKI_ARCH_DEV_C == 0)
#warning This is only an empty list of the xcontiki_arch_os_dev


xcontiki_os_sys_Protothread__state_t xcontiki_arch_dev__threads(void){
    return XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;
}

void xcontiki_arch_dev__init(void){
    xcontiki_arch_dev_Watchdog__init();
}

#endif

