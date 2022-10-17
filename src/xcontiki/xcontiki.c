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
 * File:   xcontiki.c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

static bool first_run = true;
static void init(void);

xcontiki_os_sys_Protothread__state_t xcontiki__main(void) {

    if (first_run) {
        init();
        first_run = false;
    }

    xcontiki_arch_Watchdog__periodic();
    xcontiki_os_sys_ProtothreadsScheduler__scheduler();
    xcontiki_os_sys_Etimer__request_poll();
    if (xcontiki_os_sys_Process__number_of_events_waiting() > 0) {
        xcontiki_os_sys_Process__process_next_event();
    }
    if (xcontiki_os_sys_Process__number_of_events_waiting() > 0) {
        return XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
    }
    return XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;
}

void xcontiki__init(void) {
    first_run = true;

}

static void init(void) {
    xcontiki_os_sys_ProtothreadsScheduler__init();
    xcontiki_arch_Watchdog__start();
    xcontiki_arch_Clock__init();
    xcontiki_os_sys_Process__initialize_the_process_module();
    xcontiki_os_sys_Process__start(&xcontiki_os_sys_Etimer__process, 0);
}
