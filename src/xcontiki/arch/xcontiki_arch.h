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
 * File:   arch_xcontiki.h
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#ifndef XCONTIKI_ARCH_H
#define XCONTIKI_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define __PACKED __attribute__((packed))
#define __rom
#define __ram
#define rom

#ifndef _OMNITARGET
#define _OMNITARGET ((void *)UINTPTR_MAX)
#endif

typedef uint32_t xcontiki_arch_Clock__time_t;
#define XCONTIKI_ARCH_CLOCK__MAX UINT32_MAX
typedef uint32_t xcontiki_arch_Clock__seconds_t;

#define XCONTIKI_ARCH_CLOCK__FREQUENCY (1000ul)
/**
 * A second, measured in system clock time.
 *
 * \hideinitializer
 */
#define XCONTIKI_ARCH_CLOCK__SECOND ((xcontiki_arch_Clock__time_t)XCONTIKI_ARCH_CLOCK__FREQUENCY)

#include "xcontiki/arch/xcontiki_arch_build.h"
#include "xcontiki/arch/xcontiki_arch_Clock.h"
#include "xcontiki/arch/xcontiki_arch_Sleepyhead.h"
#include "xcontiki/arch/xcontiki_arch_Watchdog.h"

#define  PRINTF(a) do{printf a ;} while(0)

#ifdef __cplusplus
}
#endif

#endif /* XCONTIKI_ARCH_H */
