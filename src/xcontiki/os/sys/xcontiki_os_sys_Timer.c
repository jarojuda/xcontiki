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
xcontiki_os_sys_Timer__set(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t intervl) {
    assert(t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "Wrong timer id");
    if (t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return 0;
    }
    if (0 == t) {
        t = allocate_new_timer();
        assert(t != 0 && "No free timers. Increase number of timers");
        if (0 == t) { //No free timer?
            return 0;
        }
    }
    interval[t] = intervl;
    if (0 == intervl) {
        timer_flags[t].expired = true;
        timer_flags[t].running = false;
    } else {
        start[t] = xcontiki_arch_Clock__time();
        previous_diff[t] = 0;
        timer_flags[t].running = true;
        timer_flags[t].expired = false;
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
    assert(t != 0 && t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "Wrong timer id");
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return;
    }
    if (xcontiki_os_sys_Timer__expired(t)) {
        start[t] += interval[t];
        previous_diff[t] = 0;
        timer_flags[t].expired = (0 == interval[t]);
        if (false == timer_flags[t].expired) {
            timer_flags[t].running = true;
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
    assert(t != 0 && t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "Wrong timer id");
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return;
    }
    start[t] = xcontiki_arch_Clock__time();
    previous_diff[t] = 0;
    timer_flags[t].expired = (0 == interval[t]);
    if (false == timer_flags[t].expired) {
        timer_flags[t].running = true;
    }
}
/*---------------------------------------------------------------------------*/


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

    assert(t != 0 && t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "Wrong timer id");
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return true;
    }
    if (timer_flags[t].expired) {
        timer_flags[t].running = false;
        return true;
    }
    xcontiki_arch_Clock__time_t diff = (xcontiki_arch_Clock__time() - start[t]);
    if (diff >= interval[t] || diff < previous_diff[t]) {
        timer_flags[t].expired = true;
        timer_flags[t].running = false;
        return true;
    } else {
        previous_diff[t] = diff;
        return false;
    }
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
    assert(t != 0 && t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "Wrong timer id");
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return 0;
    }
    if (xcontiki_os_sys_Timer__expired(t)) {
        return 0;
    }
    return (xcontiki_arch_Clock__time_t) (interval[t] - previous_diff[t]);
}

/**
 * Remove the timer.
 *
 * This function removes the timer.

 * \param t A pointer to the timer.
 * \sa timer_restart()
 */
void
xcontiki_os_sys_Timer__remove(xcontiki_os_sys_Timer__timer_id_t t) {
    assert(t != 0 && t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "Wrong timer id");
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return;
    }
    timer_flags[t].allocated = false;
    timer_flags[t].expired = true;
    timer_flags[t].running = false;
}
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/

xcontiki_os_sys_Protothread__state_t xcontiki_os_sys_Timer__sleepyhead_thread(void) {
    static xcontiki_os_sys_Protothread__pt_t pt;

    static xcontiki_os_sys_Timer__timer_id_t t;
    static xcontiki_arch_Clock__time_t now;
    static xcontiki_arch_Clock__time_t tdist; //time distance for the nearest expiration
    static xcontiki_arch_Clock__time_t diff;

    XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(pt);

    now = xcontiki_arch_Clock__time();
    tdist = 0;
    for (t = 1; t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER; t++) {
        if (timer_flags[t].running) {
            diff = now - start[t];
            if (diff >= interval[t] || diff < previous_diff[t]) {
                timer_flags[t].expired = true;
                timer_flags[t].running = false;
            } else {
                previous_diff[t] = diff;
                diff = interval[t] - diff; //time distance to the next expiration
                if (tdist < diff) {
                    tdist = diff;
                }
            }
        }
    }

    XCONTIKI_OS_SYS_PROTOTHREAD__END(pt);
}

void xcontiki_os_sys_Timer__set_interval(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t intervl) {
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return;
    }
    interval[t] = intervl;
}

xcontiki_arch_Clock__time_t xcontiki_os_sys_Timer__get_interval(xcontiki_os_sys_Timer__timer_id_t t) {
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return 0;
    }
    return interval[t];
}

void xcontiki_os_sys_Timer__set_start(xcontiki_os_sys_Timer__timer_id_t t, xcontiki_arch_Clock__time_t strt) {
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return;
    }
    start[t] = strt;
}

xcontiki_arch_Clock__time_t xcontiki_os_sys_Timer__get_start(xcontiki_os_sys_Timer__timer_id_t t) {
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return 0;
    }
    return start[t];
}


/**
 * Check if timer was allocated
 *

 * \param t An id of the timer.
 * \sa timer__set()
 */
bool
xcontiki_os_sys_Timer__is_allocated(xcontiki_os_sys_Timer__timer_id_t t) {
    if (0 == t || t >= XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
        return false;
    }
    return (0!=timer_flags[t].allocated);
}


/** @} */
