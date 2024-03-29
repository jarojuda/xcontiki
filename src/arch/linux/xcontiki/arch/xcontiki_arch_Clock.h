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

/** \addtogroup timers
 * @{
 */

/**
 * \defgroup clock Clock library
 *
 * The clock library is the interface between Contiki and the platform
 * specific clock functionality. The clock library defines a macro,
 * CLOCK_SECOND, to convert seconds into the tick resolution of the platform.
 * Typically this is 1-10 milliseconds, e.g. 4*CLOCK_SECOND could be 512.
 * A 16 bit counter would thus overflow every 1-10 minutes.
 * Platforms use the tick interrupt to maintain a long term count
 * of seconds since startup.
 *
 * Platforms may also implement rtimers for greater time resolution
 * and for real-time interrupts, These use a corresponding RTIMER_SECOND.
 *
 * \note These timers do not necessarily have a common divisor or are phase locked.
 * One may be crystal controlled and the other may not. Low power operation
 * or sleep will often use one for wake and disable the other, then give
 * it a tick correction after wakeup.
 *
 * \note The clock library need in many cases not be used
 * directly. Rather, the \ref timer "timer library", \ref etimer
 * "event timers", or \ref rtimer "rtimer library" should be used.
 *
 * \sa \ref timer "Timer library"
 * \sa \ref etimer "Event timers"
 * \sa \ref rtimer "Realtime library"
 *
 * @{
 */

#ifndef XCONTIKI_ARCH_CLOCK_H
#define XCONTIKI_ARCH_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif


    /**
     * Initialize the clock library.
     *
     * This function initializes the clock library and should be called
     * from the main() function of the system.
     *
     */
    void xcontiki_arch_Clock__init(void);


    /**
     * Get the current clock time.
     *
     * This function returns the current system clock time.
     *
     * \return The current clock time, measured in system ticks.
     */
    xcontiki_arch_Clock__time_t xcontiki_arch_Clock__time(void);


    /**
     * Get the current value of the platform seconds.
     *
     * This could be the number of seconds since startup, or
     * since a standard epoch.
     *
     * \return The value.
     */
    xcontiki_arch_Clock__seconds_t xcontiki_arch_Clock__get_seconds(void);

    /**
     * Set the value of the platform seconds.
     * \param sec   The value to set.
     *
     */
    void xcontiki_arch_Clock__set_seconds(xcontiki_arch_Clock__seconds_t sec);

    /**
     * Wait for a given number of ticks.
     *
     * Do not use it unless you know what you are doing.
      \param t   How many ticks.
     *
     */
    void xcontiki_arch_Clock__wait(xcontiki_arch_Clock__time_t t);

    /**
     * Delay a given number of microseconds.
     * \param dt   How many microseconds to delay.
     *
     * \note Interrupts could increase the delay by a variable amount.
     */
    void xcontiki_arch_Clock__delay_usec(uint32_t dt);


#ifdef __cplusplus
}
#endif

#endif /* XCONTIKI_ARCH_CLOCK_H */
