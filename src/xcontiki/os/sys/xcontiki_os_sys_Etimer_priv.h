/*
 * Copyright (c) 2020, Jaroslaw Juda <mail at JaroslawJuda.site>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Based on the Contiki http://contiki-os.org
 * and the Contiki-NG  http://www.contiki-ng.org/
 * projects.
 */

/*
 * File:   xcontiki_os_sys_Etimer_priv.h
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

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
    } timer_flags[XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER] = {0};

    static xcontiki_os_sys_Etimer__etimer_id_t allocate_new_etimer(void) {
        xcontiki_os_sys_Etimer__etimer_id_t i;
        for (i = 1; i < XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; i++) {
            if (0 == timer_flags[i].allocated) {
                timer_flags[i].allocated = 1;
                timer_flags[i].running = 0;
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
        timer_flags[et].allocated = true;
        timer_flags[et].running = true;

    }


#ifdef	__cplusplus
}
#endif
