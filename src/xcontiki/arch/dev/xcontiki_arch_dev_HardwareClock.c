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
 * File:   xcontiki_arch_dev_HardwareClock.c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

#if(XCONTIKI_ARCH_DEV_HARDWARECLOCK_C == 0)
#warning This is only a dummy implementation of the arch_dev_HardwareClock module

volatile static struct {
    uint16_t timer;
    unsigned timer_overflow : 1;
} hardware_timer_mock;

static uint16_t ticks;

void xcontiki_arch_dev_HardwareClock__init(void) {
    memset((void*) &hardware_timer_mock, 0, sizeof (hardware_timer_mock));
    ticks = 0;
}

uint16_t xcontiki_arch_dev_HardwareClock__get_timer(void) {
    hardware_timer_mock.timer++;
    if (0 == hardware_timer_mock.timer) {
        hardware_timer_mock.timer_overflow = 1;
    }
    return hardware_timer_mock.timer;
}

uint32_t xcontiki_arch_dev_HardwareClock__get_clock(void) {
    uint32_t tmp;
    do {
        if (hardware_timer_mock.timer_overflow) {
            ticks++;
            hardware_timer_mock.timer_overflow = 0;
        }
        tmp = xcontiki_arch_dev_HardwareClock__get_timer();
        tmp += ((uint32_t) ticks << 16ul);
    } while (hardware_timer_mock.timer_overflow);
    return tmp;
}

#endif
