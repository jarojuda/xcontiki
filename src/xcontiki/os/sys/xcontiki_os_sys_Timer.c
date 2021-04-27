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
 * Timer library implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

/**
 * \addtogroup timer
 * @{
 */

#include "xcontiki/xcontiki.h"

#define XCONTIKI_OS_SYS_TIMER_PRIV_H
#include "xcontiki_os_sys_Timer_priv.h"

/*---------------------------------------------------------------------------*/

/**
 * Set a timer.
 *
 * This function is used to set a timer for a time sometime in the
 * future. The function xcontiki_os_sys_Timer__expired() will evaluate to true after
 * the timer has expired.
 *
 * \param t A pointer to the timer
 * \param interval The interval before the timer expires.
 *
 */
xcontiki_os_sys_Timer__timer_id_t
xcontiki_os_sys_Timer__set(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t interval) {
    if(0==t){
        t = allocate_new_timer();
    }
    if (0 == interval) {
        flags[t].expired = true;
        interval[t] = 0;
        flags[t].running = false;
    } else {
    	start[t] = xcontiki_arch_Clock__time();
    	interval[t] = interval;
    	previous_diff[t] = 0;
    	flags[t].running = true;
    	flags[t].expired = false;
    }
    return t;
}
/*---------------------------------------------------------------------------*/

/**
 * Reset the timer with the same interval.
 *
 * This function resets the timer with the same interval that was
 * given to the timer_set() function. The start point of the interval
 * is the exact time that the timer last expired. Therefore, this
 * function will cause the timer to be stable over time, unlike the
 * timer_restart() function. If this is executed before the
 * timer expired, this function has no effect.
 *
 * \param t A pointer to the timer.
 * \sa timer_restart()
 */
void
xcontiki_os_sys_Timer__reset(xcontiki_os_sys_Timer__timer_id_t t) {
    if (xcontiki_os_sys_Timer__expired(t)) {
        start[t] += interval[t];
        previous_diff[t] = 0;
        flags[t].expired = (0 == interval[t]);
        if (false == flags[t].expired) {
            flags[t].running = true;
        }
    }
}
/*---------------------------------------------------------------------------*/

/**
 * Restart the timer from the current point in time
 *
 * This function restarts a timer with the same interval that was
 * given to the timer_set() function. The timer will start at the
 * current time.
 *
 * \note A periodic timer will drift if this function is used to reset
 * it. For periodic timers, use the timer_reset() function instead.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_reset()
 */
void
xcontiki_os_sys_Timer__restart(xcontiki_os_sys_Timer__timer_id_t t) {
    start[t] = xcontiki_arch_Clock__time();
    previous_diff[t] = 0;
    flags[t].expired = (0 == interval[t]);
    if (false == flags[t].expired) {
        flags[t].running = true;
    }
}
/*---------------------------------------------------------------------------*/

/**
 * Set a timer.
 * and
 * Check if a timer has expired.
 *
 * This function tests if a timer has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer
 *
 * \return Non-zero if the timer has expired, zero otherwise.
 *
 */
bool
xcontiki_os_sys_Timer__expired_after(xcontiki_os_sys_Timer__timer_id_t __ram t, xcontiki_arch_Clock__time_t interval) {
    bool result;

    if (false == flags[t].running) {
        xcontiki_os_sys_Timer__set(t, interval);
    }
    result = xcontiki_os_sys_Timer__expired(t);

    return result;
}

/**
 * Check if a timer has expired.
 *
 * This function tests if a timer has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer
 *
 * \return Non-zero if the timer has expired, zero otherwise.
 *
 */
bool
xcontiki_os_sys_Timer__expired(xcontiki_os_sys_Timer__timer_id_t t) {
    bool result;

    if (flags[t].expired) {
        flags[t].running = false;
        return true;
    }
    xcontiki_arch_Clock__time_t diff = (xcontiki_arch_Clock__time() - start[t]);
    if (diff >= interval[t] || diff < previous_diff[t]) {
        flags[t].expired = true;
        result = true;
    } else {
        result = false;
    }
    if (true == result) {
        flags[t].running = false;
    }
    previous_diff[t] = diff;

    return result;
}
/*---------------------------------------------------------------------------*/

/**
 * The time until the timer expires
 *
 * This function returns the time until the timer expires.
 *
 * \param t A pointer to the timer
 *
 * \return The time until the timer expires
 *
 */
xcontiki_arch_Clock__time_t
xcontiki_os_sys_Timer__remaining(xcontiki_os_sys_Timer__timer_id_t t) {
    if (xcontiki_os_sys_Timer__expired(t)) {
        return 0;
    }
    return (xcontiki_arch_Clock__time_t) (interval[t] - previous_diff[t]);
}
/*---------------------------------------------------------------------------*/

/** @} */
