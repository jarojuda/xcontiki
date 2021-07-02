/*
 * Copyright (c) 2004-2005, Swedish Institute of Computer Science.
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
 * \addtogroup pt
 * @{
 */

/**
 * \file
 * Protothreads implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */

#ifndef XCONTIKI_OS_SYS_PROTOTHREAD_H
#define XCONTIKI_OS_SYS_PROTOTHREAD_H

typedef struct {
    uint16_t local_continuation;
} xcontiki_os_sys_Protothread__pt_t;


#define XCONTIKI_OS_SYS_PROTOTHREAD__WAITING    0
#define XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED    1
#define XCONTIKI_OS_SYS_PROTOTHREAD__EXITED     2
#define XCONTIKI_OS_SYS_PROTOTHREAD__ENDED      3
#define XCONTIKI_OS_SYS_PROTOTHREAD__FIRST_RUN  4

/**
 * \name Initialization
 * @{
 */

/**
 * Initialize a protothread.
 *
 * Initializes a protothread. Initialization must be done prior to
 * starting to execute the protothread.
 *
 * \param pt A protothread control structure.
 *
 * \sa PT_SPAWN()
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__INIT(pt)   (pt.local_continuation=0);

/** @} */

/**
 * \name Declaration and definition
 * @{
 */

/**
 * Definition of type: state of a protothread.
 *
 * This type is used to declare a protothread. All protothreads must
 * return this type.
 *
 *
 * \hideinitializer
 */
typedef char xcontiki_os_sys_Protothread__state_t;
#define xcontiki_os_sys_Protothread__state_t xcontiki_os_sys_Protothread__state_t



/**
 * Declare the start of a protothread inside the C function
 * implementing the protothread.
 *
 * This macro is used to declare the starting point of a
 * protothread. It should be placed at the start of the function in
 * which the protothread runs. All C statements above the XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN()
 * invokation will be executed each time the protothread is scheduled.
 *
 * \param pt A  protothread control structure.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(pt) { switch(pt.local_continuation) { case 0:

/**
 * Declare the end of a protothread.
 *
 * This macro is used for declaring that a protothread ends. It must
 * always be used together with a matching XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN() macro.
 *
 * \param pt A protothread control structure.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__END(pt) }; pt.local_continuation=0;\
  return XCONTIKI_OS_SYS_PROTOTHREAD__ENDED; }

/** @} */

/**
 * \name Blocked wait
 * @{
 */

/**
 * Block and wait until condition is true.
 *
 * This macro blocks the protothread until the specified condition is
 * true.
 *
 * \param pt A  protothread control structure.
 * \param condition The condition.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL(pt, condition)         \
  do {      \
    pt.local_continuation = __LINE__; case __LINE__:;    \
    if(!(condition)) {    \
      return XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;   \
    }      \
  } while(0)

/**
 * Block and wait while condition is true.
 *
 * This function blocks and waits while condition is true. See
 * XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL().
 *
 * \param pt A protothread control structure.
 * \param cond The condition.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_WHILE(pt, cond)\
  XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL((pt), !(cond))

/** @} */

/**
 * \name Hierarchical protothreads
 * @{
 */

/**
 * Block and wait until a child protothread completes.
 *
 * This macro schedules a child protothread. The current protothread
 * will block until the child protothread completes.
 *
 * \note The child protothread must be manually initialized with the
 * PT_INIT() function before this function is used.
 *
 * \param pt A protothread control structure.
 * \param thread The child protothread with arguments
 *
 * \sa PT_SPAWN()
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_THREAD(pt, thread)\
 XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_WHILE((pt), XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULE(thread))

/**
 * Spawn a child protothread and wait until it exits.
 *
 * This macro spawns a child protothread and waits until it exits. The
 * macro can only be used within a protothread.
 *
 * \param pt A protothread control structure.
 * \param child A child protothread's control structure.
 * \param thread The child protothread with arguments
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__SPAWN(pt, child, thread)  \
  do {      \
    XCONTIKI_OS_SYS_PROTOTHREAD__INIT((child));    \
    XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_THREAD((pt), (thread));  \
  } while(0)

/** @} */

/**
 * \name Exiting and restarting
 * @{
 */

/**
 * Restart the protothread.
 *
 * This macro will block and cause the running protothread to restart
 * its execution at the place of the XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN() call.
 *
 * \param pt A pointer to the protothread control structure.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__RESTART(pt)    \
  do {      \
    XCONTIKI_OS_SYS_PROTOTHREAD__INIT(pt);    \
    return XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;   \
  } while(0)

/**
 * Exit the protothread.
 *
 * This macro causes the protothread to exit. If the protothread was
 * spawned by another protothread, the parent protothread will become
 * unblocked and can continue to run.
 *
 * \param pt A protothread control structure.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__EXIT(pt)    \
  do {      \
    XCONTIKI_OS_SYS_PROTOTHREAD__INIT(pt);    \
    return XCONTIKI_OS_SYS_PROTOTHREAD__EXITED;   \
  } while(0)

/** @} */

/**
 * \name Calling a protothread
 * @{
 */

/**
 * Schedule a protothread.
 *
 * This function schedules a protothread. The return value of the
 * function is non-zero if the protothread is running or zero if the
 * protothread has exited.
 *
 * \param f The call to the C function implementing the protothread to
 * be scheduled
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULE(f) ((f) < XCONTIKI_OS_SYS_PROTOTHREAD__EXITED)

/** @} */

/**
 * \name Yielding from a protothread
 * @{
 */

/**
 * Yield from the current protothread.
 *
 * This function will yield the protothread, thereby allowing other
 * processing to take place in the system.
 *
 * \param pt A protothread control structure.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__YIELD(pt)    \
  do {      \
      pt.local_continuation = __LINE__; return XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED; case __LINE__:;\
  } while(0)

/**
 * \brief      Yield from the protothread until a condition occurs.
 * \param pt   A protothread control structure.
 * \param cond The condition.
 *
 *             This function will yield the protothread, until the
 *             specified condition evaluates to true.
 *
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROTOTHREAD__YIELD_UNTIL(pt, cond)  \
  do {      \
    pt.local_continuation = __LINE__; return XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED; case __LINE__:; \
    if( !(cond)) { \
      return XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED; \
    }  \
  } while(0)

/** @} */

#endif /* XCONTIKI_OS_SYS_PROTOTHREAD_H */

/** @} */
