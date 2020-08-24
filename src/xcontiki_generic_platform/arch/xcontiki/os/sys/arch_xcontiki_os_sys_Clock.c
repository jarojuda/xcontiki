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
 * File:   arch_xcontiki_os_sys_Clock.c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

#ifndef ARCH_XCONTIKI_OS_SYS_CLOCK_C
#warning This is only a dummy implementation of the arch_xcontiki_os_sys_Clock module

static volatile arch_xcontiki_os_sys_Clock__seconds_t dummy_clock_seconds;

void arch_xcontiki_os_sys_Clock__init(void) {
    arch_xcontiki_dev_HardwareClock__init();
    dummy_clock_seconds = 0;
}

arch_xcontiki_os_sys_Clock__time_t arch_xcontiki_os_sys_Clock__time(void) {
    return (arch_xcontiki_os_sys_Clock__time_t) arch_xcontiki_dev_HardwareClock__get_clock();
}

arch_xcontiki_os_sys_Clock__seconds_t arch_xcontiki_os_sys_Clock__seconds(void) {
    return dummy_clock_seconds;
}

void arch_xcontiki_os_sys_Clock__set_seconds(arch_xcontiki_os_sys_Clock__seconds_t sec) {
    dummy_clock_seconds = sec;
}

//Do not use it unless you know what you are doing.

void arch_xcontiki_os_sys_Clock__wait(arch_xcontiki_os_sys_Clock__time_t interval) {
    arch_xcontiki_os_sys_Clock__time_t start;
    arch_xcontiki_os_sys_Clock__time_t diff;
    arch_xcontiki_os_sys_Clock__time_t prev_diff;

    if (0 == interval) {
        return;
    }
    start = arch_xcontiki_os_sys_Clock__time();
    prev_diff = 0;
    for (;;) {
        diff = arch_xcontiki_os_sys_Clock__time() - start;
        if (diff == prev_diff) {
            continue;
        }
        //Ensure that the loop will stop if the arch_xcontiki_os_sys_Clock__time() returns discontinuous values
        if (diff >= interval || diff < prev_diff) {
            break;
        }
        prev_diff = diff;
        arch_xcontiki_dev_Watchdog__periodic();
    }
}

void arch_xcontiki_os_sys_Clock__delay_usec(uint16_t dt) {
    uint16_t start;
    uint16_t diff;
    uint16_t prev_diff;
    uint16_t interval;

    start = arch_xcontiki_dev_HardwareClock__get_timer();
#if(ARCH_DEV_HARDWARECLOCK__FREQUENCY>32768ull)
#error This method is suitable only for the clock frequency no greater than 32768 Hz   
#endif

    interval = ((dt * ARCH_XCONTIKI_DEV_HARDWARECLOCK__FREQUENCY)+(ARCH_XCONTIKI_DEV_HARDWARECLOCK__FREQUENCY / 2)) / 1000000ull;
    prev_diff = 0;
    for (;;) {
        diff = arch_xcontiki_dev_HardwareClock__get_timer() - start;
        if (diff == prev_diff) {
            continue;
        }
        //Ensure that the loop will stop if the arch_dev_HardwareClock__get_timer() returns discontinuous values
        if (diff >= interval || diff < prev_diff) {
            break;
        }
        prev_diff = diff;
    }
}

#endif
