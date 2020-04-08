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

static volatile arch_xcontiki_os_sys_Clock__time_t dummy_clock;
static volatile arch_xcontiki_os_sys_Clock__seconds_t dummy_clock_seconds;

void arch_xcontiki_os_sys_Clock__init(void) {
    dummy_clock = 0;
    dummy_clock_seconds = 0;
}

arch_xcontiki_os_sys_Clock__time_t arch_xcontiki_os_sys_Clock__time(void) {
    dummy_clock += ARCH_XCONTIKI_OS_SYS_CLOCK__DUMMY_CLOCK_INCREMENT;
    return dummy_clock;
}

arch_xcontiki_os_sys_Clock__seconds_t arch_xcontiki_os_sys_Clock__seconds(void) {
    return dummy_clock_seconds;
}

void arch_xcontiki_os_sys_Clock__set_seconds(arch_xcontiki_os_sys_Clock__seconds_t sec) {
    dummy_clock_seconds = sec;
}

void arch_xcontiki_os_sys_Clock__wait(arch_xcontiki_os_sys_Clock__time_t interval) {
    static arch_xcontiki_os_sys_Clock__time_t start;
    //TODO: ensure that the loop stops if the arch_xcontiki_os_sys_Clock__time() returns discontinuous values
    start = arch_xcontiki_os_sys_Clock__time();
    while ((arch_xcontiki_os_sys_Clock__time_t) (arch_xcontiki_os_sys_Clock__time() - start) < interval) {
        arch_xcontiki_os_dev_Watchdog__periodic();
    };
}

void arch_xcontiki_os_sys_Clock__delay_usec(uint16_t dt) {

}

#endif
