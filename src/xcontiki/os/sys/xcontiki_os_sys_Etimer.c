/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * \addtogroup etimer
 * @{
 */

/**
 * \file
 * Event timer library implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

#include "xcontiki/xcontiki.h"

#define XCONTIKI_OS_SYS_ETIMER_PRIV_H
#include "xcontiki_os_sys_Etimer_priv.h"

XCONTIKI_OS_SYS_PROCESS(xcontiki_os_sys_Etimer__process, "Event timer");

/*---------------------------------------------------------------------------*/
XCONTIKI_OS_SYS_PROCESS__THREAD(xcontiki_os_sys_Etimer__process, ev, data) {
    static xcontiki_os_sys_Etimer__etimer_id_t et;

    XCONTIKI_OS_SYS_PROCESS__BEGIN();

    while (1) {
        XCONTIKI_OS_SYS_PROCESS__YIELD();

        if (ev == XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED) {
            struct xcontiki_os_sys_Process *p = data;
            for (et = 1; et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; et++) {
                if (flags[et].allocated && process_ptr[et] == p) {
                    flags[et].allocated = 0;
                    flags[et].running = 0;
                    process_ptr[et] = XCONTIKI_OS_SYS_PROCESS__NONE;
                }
            }
            continue;
        } else if (ev != XCONTIKI_OS_SYS_PROCESS__EVENT_POLL) {
            continue;
        }

        for (et = 1; et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; et++) {
            if (flags[et].running && xcontiki_os_sys_Timer__expired(timer[et])) {
                if (xcontiki_os_sys_Process__post_event_via_queue(process_ptr[et], XCONTIKI_OS_SYS_PROCESS__EVENT_TIMER, &timer[et]) == XCONTIKI_OS_SYS_PROCESS__ERR_OK) {

                    /* Reset the process ID of the event timer, to signal that the
                       etimer has expired. This is later checked in the
                       xcontiki_os_sys_Timer__expired() function. */
                    process_ptr[et] = XCONTIKI_OS_SYS_PROCESS__NONE;
                    flags[et].running = 0;
                } else {
                    xcontiki_os_sys_Etimer__request_poll();
                }
            }
        }
    }

    XCONTIKI_OS_SYS_PROCESS__END();
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__request_poll(void) {
    xcontiki_os_sys_Process__poll(&xcontiki_os_sys_Etimer__process);
}

/*---------------------------------------------------------------------------*/
xcontiki_os_sys_Etimer__etimer_id_t
xcontiki_os_sys_Etimer__set(xcontiki_os_sys_Etimer__etimer_id_t et, xcontiki_arch_Clock__time_t interval) {
    assert(et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return 0;
    }
    if (0 == et) {
        et = allocate_new_etimer();
        assert(et != 0 && "No free etimers. Increase number of event timers");
        if (0 == et) { //No free etimer?
            return 0;
        }
    }
    timer[et] = xcontiki_os_sys_Timer__set(timer[et], interval);
    add_timer(et);
    return et;
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__reset_with_new_interval(xcontiki_os_sys_Etimer__etimer_id_t et, xcontiki_arch_Clock__time_t intervl) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return;
    }
    xcontiki_os_sys_Timer__reset(timer[et]);
    xcontiki_os_sys_Timer__set_interval(timer[et], intervl);
    add_timer(et);
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__reset(xcontiki_os_sys_Etimer__etimer_id_t et) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return;
    }
    xcontiki_os_sys_Timer__reset(timer[et]);
    add_timer(et);
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__restart(xcontiki_os_sys_Etimer__etimer_id_t et) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return;
    }
    xcontiki_os_sys_Timer__restart(timer[et]);
    add_timer(et);
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__adjust(xcontiki_os_sys_Etimer__etimer_id_t et, xcontiki_arch_Clock__time_t timediff) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return;
    }
    xcontiki_os_sys_Timer__set_start(timer[et], xcontiki_os_sys_Timer__get_start(timer[et]) + timediff);
}

/*---------------------------------------------------------------------------*/
bool
xcontiki_os_sys_Etimer__expired(xcontiki_os_sys_Etimer__etimer_id_t et) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return 1;
    }
    return process_ptr[et] == XCONTIKI_OS_SYS_PROCESS__NONE;
}

/*---------------------------------------------------------------------------*/
xcontiki_arch_Clock__time_t
xcontiki_os_sys_Etimer__expiration_time(xcontiki_os_sys_Etimer__etimer_id_t et) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return 0;
    }
    return xcontiki_os_sys_Timer__get_start(timer[et]) + xcontiki_os_sys_Timer__get_interval(timer[et]);
}

/*---------------------------------------------------------------------------*/
xcontiki_arch_Clock__time_t
xcontiki_os_sys_Etimer__start_time(xcontiki_os_sys_Etimer__etimer_id_t et) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return 0;
    }
    return xcontiki_os_sys_Timer__get_start(timer[et]);
}

/*---------------------------------------------------------------------------*/
bool
xcontiki_os_sys_Etimer__pending(void) {
    return timerlist != NULL;
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__stop(xcontiki_os_sys_Etimer__etimer_id_t et) {
    assert(et != 0 && et < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER && "Wrong event timer id");
    if (0 == et || et >= XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER) {
        return;
    }
    struct xcontiki_os_sys_Etimer *t;

    /* First check if et is the first event timer on the list. */
    if (et == timerlist) {
        timerlist = timerlist->next;
    } else {
        /* Else walk through the list and try to find the item before the
           et timer. */
        for (t = timerlist; t != NULL && t->next != et; t = t->next) {
        }

        if (t != NULL) {
            /* We've found the item before the event timer that we are about
               to remove. We point the items next pointer to the event after
               the removed item. */
            t->next = et->next;
        }
    }

    /* Remove the next pointer from the item to be removed. */
    et->next = NULL;
    /* Set the timer as expired */
    et->p = XCONTIKI_OS_SYS_PROCESS__NONE;
}
/*---------------------------------------------------------------------------*/
/** @} */
