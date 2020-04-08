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
 * File:   arch_xcontiki_os_dev_Watchdog.c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

#ifndef ARCH_XCONTIKI_OS_DEV_WATCHDOG_C
#warning This is only a dummy implementation of the arch_xcontiki_os_dev_Watchdog module 

void arch_xcontiki_os_dev_Watchdog__init(void) {

}

void arch_xcontiki_os_dev_Watchdog__start(void) {

}

/* Do not use this function from within an interrupt context */
void arch_xcontiki_os_dev_Watchdog__periodic(void) {

}

void arch_xcontiki_os_dev_Watchdog__stop(void) {

}

void arch_xcontiki_os_dev_Watchdog__reboot(void) {

}

#endif
