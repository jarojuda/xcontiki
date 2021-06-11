/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * \file
 * Timer library header file.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

/** \addtogroup timers
 * @{ */

/**
 * \defgroup timer Timer library
 *
 * The timer library provides functions for setting, resetting and
 * restarting timers, and for checking if a timer has expired. An
 * application must "manually" check if its timers have expired; this
 * is not done automatically.
 *
 * A timer is declared as a \c struct \c timer and all access to the
 * timer is made by a pointer to the declared timer.
 *
 * \note The timer library is not able to post events when a timer
 * expires. The \ref etimer "Event timers" should be used for this
 * purpose.
 *
 * \note The timer library uses the \ref clock "Clock library" to
 * measure time. Intervals should be specified in the format used by
 * the clock library.
 *
 * \sa \ref etimer "Event timers"
 *
 * @{
 */

#ifndef XCONTIKI_OS_SYS_TIMER_H
#define XCONTIKI_OS_SYS_TIMER_H

/**
 * A timer.
 *
 * This structure is used for declaring a timer. The timer must be set
 * with timer_set() before it can be used.
 *
 * \hideinitializer
 */

#ifndef XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER
#define XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER (32u)
#endif

typedef uint8_t xcontiki_os_sys_Timer__timer_id_t;

xcontiki_os_sys_Timer__timer_id_t xcontiki_os_sys_Timer__set(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t interval);
void xcontiki_os_sys_Timer__reset(xcontiki_os_sys_Timer__timer_id_t t);
void xcontiki_os_sys_Timer__restart(xcontiki_os_sys_Timer__timer_id_t t);
bool xcontiki_os_sys_Timer__expired(xcontiki_os_sys_Timer__timer_id_t t);
bool xcontiki_os_sys_Timer__expired_after(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t interval);
xcontiki_arch_Clock__time_t xcontiki_os_sys_Timer__remaining(xcontiki_os_sys_Timer__timer_id_t t);
void xcontiki_os_sys_Timer__set_interval(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t interval);
xcontiki_arch_Clock__time_t xcontiki_os_sys_Timer__get_interval(xcontiki_os_sys_Timer__timer_id_t t);
void xcontiki_os_sys_Timer__set_start(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t start);
xcontiki_arch_Clock__time_t xcontiki_os_sys_Timer__get_start(xcontiki_os_sys_Timer__timer_id_t t);
void xcontiki_os_sys_Timer__remove(xcontiki_os_sys_Timer__timer_id_t t);
bool xcontiki_os_sys_Timer__is_allocated(xcontiki_os_sys_Timer__timer_id_t t);

#define XCONTIKI_OS_SYS_TIMER__NEW_STATIC(timer) \
\
static xcontiki_os_sys_Timer__timer_id_t  _##timer##_id;\
static void timer##set(xcontiki_arch_Clock__time_t t){\
    _##timer##_id=xcontiki_os_sys_Timer__set(_##timer##_id, t);\
}\
static void timer##reset(void){\
    xcontiki_os_sys_Timer__reset(_##timer##_id);\
}\
static void timer##restart(void){\
    xcontiki_os_sys_Timer__restart(_##timer##_id);\
}\
static bool timer##expired(void){\
    return xcontiki_os_sys_Timer__expired(_##timer##_id);\
}\
static bool timer##expired_after(xcontiki_arch_Clock__time_t t){\
    return xcontiki_os_sys_Timer__expired_after(_##timer##_id, t);\
}\
static xcontiki_arch_Clock__time_t timer##remaining(void){\
    return xcontiki_os_sys_Timer__remaining(_##timer##_id);\
}\
static void timer##remove(void){\
    xcontiki_os_sys_Timer__remove(_##timer##_id);\
}\
\
static bool timer##is_allocated(void){\
    return xcontiki_os_sys_Timer__is_allocated(_##timer##_id);\
}\
\
static const struct {\
    void (* const set)(xcontiki_arch_Clock__time_t);\
    void (* const reset)(void);\
    void (* const restart)(void);\
    bool (* const expired)(void);\
    bool (* const expired_after)(xcontiki_arch_Clock__time_t);\
    xcontiki_arch_Clock__time_t (* const remaining)(void);\
    void (* const remove)(void);\
    bool (* const is_allocated)(void);\
} timer = {\
    timer##set,\
    timer##reset,\
    timer##restart,\
    timer##expired,\
    timer##expired_after,\
    timer##remaining,\
    timer##remove,\
    timer##is_allocated,\
};\


#endif /* XCONTIKI_OS_SYS_TIMER_H */

/** @} */
/** @} */
