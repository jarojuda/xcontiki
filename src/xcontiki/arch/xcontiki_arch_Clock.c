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
 * File:   xcontiki_arch_os_sys_Clock.c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

#if(XCONTIKI_ARCH_CLOCK_C == 0)
#warning This is only a dummy implementation of the xcontiki_arch_os_sys_Clock module

static volatile uint32_t hardware_timer_mock;
static volatile xcontiki_arch_Clock__seconds_t dummy_clock_seconds;

#ifndef TEST
 static uint32_t get_timer(void) {
    ++hardware_timer_mock;
    return hardware_timer_mock;
}
#else
uint32_t get_timer(void);
#endif

void xcontiki_arch_Clock__init(void) {
    hardware_timer_mock= 0;
    dummy_clock_seconds = 0;
}

xcontiki_arch_Clock__time_t xcontiki_arch_Clock__time(void) {
    return (xcontiki_arch_Clock__time_t)get_timer();
}

xcontiki_arch_Clock__seconds_t xcontiki_arch_Clock__get_seconds(void) {
    return dummy_clock_seconds;
}

void xcontiki_arch_Clock__set_seconds(xcontiki_arch_Clock__seconds_t sec) {
    dummy_clock_seconds = sec;
}

//Do not use it unless you know what you are doing.

void xcontiki_arch_Clock__wait(xcontiki_arch_Clock__time_t interval) {
    xcontiki_arch_Clock__time_t start;
    xcontiki_arch_Clock__time_t diff;
    xcontiki_arch_Clock__time_t prev_diff;

    if (0 == interval) {
        return;
    }
    start = xcontiki_arch_Clock__time();
    prev_diff = 0;
    for (;;) {
        diff = xcontiki_arch_Clock__time() - start;
        if (diff == prev_diff) {
            continue;
        }
        //Ensure that the loop will stop if the xcontiki_arch_os_sys_Clock__time() returns discontinuous values
        if (diff >= interval || diff < prev_diff) {
            break;
        }
        prev_diff = diff;
        xcontiki_arch_dev_Watchdog__periodic();
    }
}

void xcontiki_arch_Clock__delay_usec(uint32_t dt) {
    uint32_t start;
    uint32_t diff;
    uint32_t prev_diff;
    uint32_t interval;

    start = get_timer();

    interval = ((dt * XCONTIKI_ARCH_CLOCK__FREQUENCY)+(XCONTIKI_ARCH_CLOCK__FREQUENCY / 2)) / 1000000ull;
    prev_diff = 0;
    for (;;) {
        diff = get_timer() - start;
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
