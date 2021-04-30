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
 * Event timer header file.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

/** \addtogroup timers
 * @{ */

/**
 * \defgroup etimer Event timers
 *
 * Event timers provides a way to generate timed events. An event
 * timer will post an event to the process that set the timer when the
 * event timer expires.
 *
 * An event timer is declared as a \c struct \c etimer and all access
 * to the event timer is made by a pointer to the declared event
 * timer.
 *
 * \sa \ref timer "Simple timer library"
 * \sa \ref clock "Clock library" (used by the timer library)
 *
 * It is \e not safe to manipulate event timers within an interrupt context.
 * @{
 */

#ifndef XCONTIKI_OS_SYS_ETIMER_H
#define XCONTIKI_OS_SYS_ETIMER_H

#include "xcontiki_os_sys_Timer.h"

#ifndef XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER
#define XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER (32u)
#endif

typedef uint8_t xcontiki_os_sys_Etimer__etimer_id_t;
#define xcontiki_os_sys_Etimer__etimer_id_t xcontiki_os_sys_Etimer__etimer_id_t


/**
 * A timer.
 *
 * This structure is used for declaring a timer. The timer must be set
 * with etimer_set() before it can be used.
 *
 * \hideinitializer
 */
struct xcontiki_os_sys_Etimer {
  xcontiki_os_sys_Timer__timer_id_t timer;
  struct xcontiki_os_sys_Etimer *next;
  struct xcontiki_os_sys_Process *p;
};

/**
 * \name Functions called from application programs
 * @{
 */

/**
 * \brief      Set an event timer.
 * \param et   A pointer to the event timer
 * \param interval The interval before the timer expires.
 *
 *             This function is used to set an event timer for a time
 *             sometime in the future. When the event timer expires,
 *             the event PROCESS_EVENT_TIMER will be posted to the
 *             process that called the etimer_set() function.
 *
 */
void xcontiki_os_sys_Etimer__set(struct xcontiki_os_sys_Etimer *et, xcontiki_arch_Clock__time_t interval);

/**
 * \brief      Reset an event timer with the same interval as was
 *             previously set.
 * \param et   A pointer to the event timer.
 *
 *             This function resets the event timer with the same
 *             interval that was given to the event timer with the
 *             etimer_set() function. The start point of the interval
 *             is the exact time that the event timer last
 *             expired. Therefore, this function will cause the timer
 *             to be stable over time, unlike the etimer_restart()
 *             function. If this is executed before the timer expired,
 *             this function has no effect.
 *
 * \sa etimer_restart()
 */
void xcontiki_os_sys_Etimer__reset(struct xcontiki_os_sys_Etimer *et);

/**
 * \brief      Reset an event timer with a new interval.
 * \param et   A pointer to the event timer.
 * \param interval The interval before the timer expires.
 *
 *             This function very similar to etimer_reset. Opposed to
 *             etimer_reset it is possible to change the timout.
 *             This allows accurate, non-periodic timers without drift.
 *
 * \sa etimer_reset()
 */
void xcontiki_os_sys_Etimer__reset_with_new_interval(struct xcontiki_os_sys_Etimer *et, xcontiki_arch_Clock__time_t interval);

/**
 * \brief      Restart an event timer from the current point in time
 * \param et   A pointer to the event timer.
 *
 *             This function restarts the event timer with the same
 *             interval that was given to the etimer_set()
 *             function. The event timer will start at the current
 *             time.
 *
 *             \note A periodic timer will drift if this function is
 *             used to reset it. For periodic timers, use the
 *             etimer_reset() function instead.
 *
 * \sa etimer_reset()
 */
void xcontiki_os_sys_Etimer__restart(struct xcontiki_os_sys_Etimer *et);

/**
 * \brief      Adjust the expiration time for an event timer
 * \param et   A pointer to the event timer.
 * \param td   The time difference to adjust the expiration time with.
 *
 *             This function is used to adjust the time the event
 *             timer will expire. It can be used to synchronize
 *             periodic timers without the need to restart the timer
 *             or change the timer interval.
 *
 *             \note This function should only be used for small
 *             adjustments. For large adjustments use etimer_set()
 *             instead.
 *
 *             \note A periodic timer will drift unless the
 *             etimer_reset() function is used.
 *
 * \sa etimer_set()
 * \sa etimer_reset()
 */
void xcontiki_os_sys_Etimer__adjust(struct xcontiki_os_sys_Etimer *et, int td);

/**
 * \brief      Get the expiration time for the event timer.
 * \param et   A pointer to the event timer
 * \return     The expiration time for the event timer.
 *
 *             This function returns the expiration time for an event timer.
 */
xcontiki_arch_Clock__time_t xcontiki_os_sys_Etimer__expiration_time(struct xcontiki_os_sys_Etimer *et);

/**
 * \brief      Get the start time for the event timer.
 * \param et   A pointer to the event timer
 * \return     The start time for the event timer.
 *
 *             This function returns the start time (when the timer
 *             was last set) for an event timer.
 */
xcontiki_arch_Clock__time_t xcontiki_os_sys_Etimer__start_time(struct xcontiki_os_sys_Etimer *et);

/**
 * \brief      Check if an event timer has expired.
 * \param et   A pointer to the event timer
 * \return     Non-zero if the timer has expired, zero otherwise.
 *
 *             This function tests if an event timer has expired and
 *             returns true or false depending on its status.
 */
int xcontiki_os_sys_Etimer__expired(struct xcontiki_os_sys_Etimer *et);

/**
 * \brief      Stop a pending event timer.
 * \param et   A pointer to the pending event timer.
 *
 *             This function stops an event timer that has previously
 *             been set with etimer_set() or etimer_reset(). After
 *             this function has been called, the event timer will not
 *             emit any event when it expires.
 *
 */
void xcontiki_os_sys_Etimer__stop(struct xcontiki_os_sys_Etimer *et);

/** @} */

/**
 * \name Functions called from timer interrupts, by the system
 * @{
 */

/**
 * \brief      Make the event timer aware that the clock has changed
 *
 *             This function is used to inform the event timer module
 *             that the system clock has been updated. Typically, this
 *             function would be called from the timer interrupt
 *             handler when the clock has ticked.
 */
void xcontiki_os_sys_Etimer__request_poll(void);

/**
 * \brief      Check if there are any non-expired event timers.
 * \return     True if there are active event timers, false if there are
 *             no active timers.
 *
 *             This function checks if there are any active event
 *             timers that have not expired.
 */
int xcontiki_os_sys_Etimer__pending(void);


/** @} */

XCONTIKI_OS_SYS_PROCESS__NAME(xcontiki_os_sys_Etimer__process);
#endif /* XCONTIKI_OS_SYS_ETIMER_H */
/** @} */
/** @} */
