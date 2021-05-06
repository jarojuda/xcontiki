/* 
 * File:   xcontiki_os_sys_Etimer_priv.h
 * Author: jarojuda
 *
 */

#include "xcontiki_os_sys_Process.h"




#ifndef XCONTIKI_OS_SYS_ETIMER_PRIV_H
#error 	Include only from the source file xcontiki_os_sys_Etimer.c
#endif	/* XCONTIKI_OS_SYS_ETIMER_PRIV_H */

#ifdef	__cplusplus
extern "C" {
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...)
#else
#define PRINTF(...)
#endif

    static xcontiki_os_sys_Timer__timer_id_t timer[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER];
    static struct xcontiki_os_sys_Process *process_ptr[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER];

    static struct __PACKED {
        unsigned allocated : 1;
        unsigned running : 1;
    } flags[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER] = {0};

    static xcontiki_os_sys_Etimer__etimer_id_t allocate_new_etimer(void) {
        xcontiki_os_sys_Etimer__etimer_id_t i;
        for (i = 1; i < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; i++) {
            if (0 == flags[i].allocated) {
                flags[i].allocated = 1;
                flags[i].running = 0;
                process_ptr[i] = XCONTIKI_OS_SYS_PROCESS__NONE;
                break;
            }
        }
        assert(i < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "No more event timers to allocate");
        if (i < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
            return i;
        } else {
            PRINTF("No more event timers to allocate\n");
        }
        return 0;
    }


    /*---------------------------------------------------------------------------*/
    static void
    add_timer(xcontiki_os_sys_Etimer__etimer_id_t et) {

        xcontiki_os_sys_Etimer__request_poll();
        process_ptr[et] = xcontiki_os_sys_Process__get_current_process();
        flags[et].running = true;
        
    }


#ifdef	__cplusplus
}
#endif

