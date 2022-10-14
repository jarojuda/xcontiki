#include "xcontiki/xcontiki.h"


#if(XCONTIKI_ARCH_DEV_C == 0)
#warning This is only a dummy implementation of the xcontiki_arch_os_dev


void xcontiki_arch_dev__init(void){
    xcontiki_arch_dev_Watchdog__init();
}

#endif

