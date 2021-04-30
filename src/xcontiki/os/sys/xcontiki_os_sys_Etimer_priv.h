/* 
 * File:   xcontiki_os_sys_Etimer_priv.h
 * Author: jarojuda
 *
 */

#include "xcontiki_os_sys_Etimer.h"


#ifndef XCONTIKI_OS_SYS_ETIMER_PRIV_H
#error 	Include only from the source file xcontiki_os_sys_Etimer.c
#endif	/* XCONTIKI_OS_SYS_ETIMER_PRIV_H */

#ifdef	__cplusplus
extern "C" {
#endif

    static xcontiki_os_sys_Timer__timer_id_t timer[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER];
    static struct xcontiki_os_sys_Process *process_ptr[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER];

    static struct __PACKED {
        unsigned allocated : 1;
        unsigned running : 1;
        unsigned expired : 1;
    } flags[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER] = {0};

    static xcontiki_os_sys_Etimer__etimer_id_t allocate_new_timer(void) {
        xcontiki_os_sys_Etimer__etimer_id_t i;
        for (i = 1; i < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; i++) {
            if (0 == flags[i].allocated) {
                flags[i].allocated = 1;
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

    static struct xcontiki_os_sys_Etimer *timerlist;

    /*---------------------------------------------------------------------------*/
    static void
    add_timer(struct xcontiki_os_sys_Etimer *etimer) {
        struct xcontiki_os_sys_Etimer *t;

        xcontiki_os_sys_Etimer__request_poll();

        if (etimer->p != XCONTIKI_OS_SYS_PROCESS__NONE) {
            for (t = timerlist; t != NULL; t = t->next) {
                if (t == etimer) {
                    /* Timer already on list, bail out. */
                    etimer->p = xcontiki_os_sys_Process__get_current_process();
                    return;
                }
            }
        }

        /* Timer not on list. */
        etimer->p = xcontiki_os_sys_Process__get_current_process();
        etimer->next = timerlist;
        timerlist = etimer;
    }


#ifdef	__cplusplus
}
#endif



