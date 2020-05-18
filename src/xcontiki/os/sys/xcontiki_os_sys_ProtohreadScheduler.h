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
 * File:   xcontiki_os_sys-ProthreadScheduler.h
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#ifndef XCONTIKI_OS_SYS_PROTOHREADSCHEDULER_H
#define XCONTIKI_OS_SYS_PROTOHREADSCHEDULER_H

#include <xcontiki/xcontiki.h>


#ifdef __cplusplus
extern "C" {
#endif

    enum {
        number_of_tasks = 0
#define XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task, interval)\
        +1
#undef XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK
    };

    enum {
#define XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task, interval)\
        __FILE##task,
#undef XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK
    };

    
    static const  arch_xcontiki_os_sys_Clock__time_t interval[number_of_tasks] = {
#define XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task, interval)\
        interval,
#undef XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK
    };

    static XCONTIKI_OS_SYS_PROTOTHREAD__THREAD last_states[number_of_tasks];
    static arch_xcontiki_os_sys_Clock__time_t last_ticks[number_of_tasks];
    static arch_xcontiki_os_sys_Clock__time_t prev_diff[number_of_tasks];

static XCONTIKI_OS_SYS_PROTOTHREAD__THREAD call_task(uint8_t task_number){
    switch(task_number){
#define XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task, interval)\
    case __FILE##task: return task();
#undef XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK      
        
        default:
            return XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
    }
}    
    
    
    
    static void scheduler(void) {
        static uint8_t i;
        arch_xcontiki_os_sys_Clock__time_t diff;

        for (i = 0; i < number_of_tasks; i++) {
            if (last_states[i] >= XCONTIKI_OS_SYS_PROTOTHREAD__FIRST_RUN) {
                last_ticks[i] = arch_xcontiki_os_sys_Clock__time();
                prev_diff[i] = 0;
                last_states[i] = call_task(i);
            } else if (0 == interval[i] || last_states[i] < XCONTIKI_OS_SYS_PROTOTHREAD__EXITED) {
                last_states[i] = call_task(i);
            } else {
                diff = arch_xcontiki_os_sys_Clock__time() - last_ticks[i];
                if (diff >= interval[i] || diff < prev_diff[i]) {
                    last_ticks[i] = arch_xcontiki_os_sys_Clock__time();
                    prev_diff[i] = 0;
                    last_states[i] = call_task(i);
                } else {
                    prev_diff[i] = diff;
                }
            }
        }
    }

    static void scheduler__init(void) {
        uint8_t i;
        for (i = 0; i <= number_of_tasks; i++) {
            last_states[i] = XCONTIKI_OS_SYS_PROTOTHREAD__FIRST_RUN;
            last_ticks[i] = 0;
        }
    }

#ifdef __cplusplus
}
#endif

#endif /* XCONTIKI_OS_SYS_PROTOHREADSCHEDULER_H */

