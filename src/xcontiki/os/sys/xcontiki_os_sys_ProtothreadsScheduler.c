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
 * File:   .c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

#ifndef XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

xcontiki_os_sys_Protothread__state_t dumb_thread(void){
    return XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;
}

#define XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST \
XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK(dumb_thread, XCONTIKI_ARCH_CLOCK__MAX)

#endif //XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

enum {
    number_of_tasks = 0
#define XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK(task, interval)\
     +1

    XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

#undef XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK
};

#define XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK(task, interval)\
    xcontiki_os_sys_Protothread__state_t task (void);

XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

#undef XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK

        static const xcontiki_arch_Clock__time_t interval[number_of_tasks] = {
#define XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK(task, interval)\
     interval,

    XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

#undef XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK
};

static xcontiki_os_sys_Protothread__state_t last_states[number_of_tasks];
static xcontiki_arch_Clock__time_t last_ticks[number_of_tasks];
static xcontiki_arch_Clock__time_t prev_diff[number_of_tasks];

static xcontiki_os_sys_Protothread__state_t call_task(uint8_t task_number) {

    enum {
#define XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK(task, interval)\
     _##task,

        XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

#undef XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK
        
    };

    switch (task_number) {
#define XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK(task, interval)\
 case _##task: return task();

            XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK_LIST

#undef XCONTIKI_OS_SYS_PROTOTHREADSSCHEDULER__TASK

        default:
            return XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
    }
}

static bool scheduler_first_run = true;

xcontiki_os_sys_Protothread__state_t xcontiki_os_sys_ProtothreadsScheduler__scheduler(void) {
    static uint8_t i;
    xcontiki_arch_Clock__time_t diff;

    if (scheduler_first_run) {
        for (i = 0; i < number_of_tasks; i++) {
            last_states[i] = XCONTIKI_OS_SYS_PROTOTHREAD__FIRST_RUN;
            last_ticks[i] = 0;
        }
        scheduler_first_run = false;
    }
    for (i = 0; i < number_of_tasks; i++) {
        if (last_states[i] >= XCONTIKI_OS_SYS_PROTOTHREAD__FIRST_RUN) {
            last_ticks[i] = xcontiki_arch_Clock__time();
            prev_diff[i] = 0;
            last_states[i] = call_task(i);
        } else if (0 == interval[i] || last_states[i] < XCONTIKI_OS_SYS_PROTOTHREAD__EXITED) {
            last_states[i] = call_task(i);
        } else {
            diff = xcontiki_arch_Clock__time() - last_ticks[i];
            if (diff >= interval[i] || diff < prev_diff[i]) {
                last_ticks[i] = xcontiki_arch_Clock__time();
                prev_diff[i] = 0;
                last_states[i] = call_task(i);
            } else {
                prev_diff[i] = diff;
            }
        }
    }
    xcontiki_os_sys_Protothread__state_t result;
    result = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
    for (i = 0; i < number_of_tasks; i++) {
        if (XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED == last_states[i]) {
            result = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
            break;
        } else if (XCONTIKI_OS_SYS_PROTOTHREAD__WAITING == last_states[i]) {
            result = XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;
        }
    }
    return result;
}

void xcontiki_os_sys_ProtothreadsScheduler__init(void) {
    scheduler_first_run = true;
}
