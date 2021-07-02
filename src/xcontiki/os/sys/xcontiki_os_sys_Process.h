/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 */

/**
 * \addtogroup threads
 * @{
 */

/**
 * \defgroup process Contiki processes
 *
 * A process in Contiki consists of a single protothread.
 *
 * @{
 */

/**
 * \file
 * Header file for the Contiki process interface.
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */
#ifndef XCONTIKI_OS_SYS_PROCESS_H
#define XCONTIKI_OS_SYS_PROCESS_H

#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"

typedef unsigned char xcontiki_os_sys_Process__event_t;
typedef uintptr_t xcontiki_os_sys_Process__data_t;
typedef unsigned char xcontiki_os_sys_Process__num_events_t;

/**
 * \name Return values
 * @{
 */

/**
 * \brief      Return value indicating that an operation was successful.
 *
 *             This value is returned to indicate that an operation
 *             was successful.
 */
#define XCONTIKI_OS_SYS_PROCESS__ERR_OK        0
/**
 * \brief      Return value indicating that the event queue was full.
 *
 *             This value is returned from xcontiki_os_sys_process_post() to indicate
 *             that the event queue was full and that an event could
 *             not be posted.
 */
#define XCONTIKI_OS_SYS_PROCESS__ERR_FULL      1
/* @} */

#define XCONTIKI_OS_SYS_PROCESS__NONE          NULL

#ifndef XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS
#define XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS 32
#endif /* XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS */

#define XCONTIKI_OS_SYS_PROCESS__EVENT_NONE            0x80
#define XCONTIKI_OS_SYS_PROCESS__EVENT_INIT            0x81
#define XCONTIKI_OS_SYS_PROCESS__EVENT_POLL            0x82
#define XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT            0x83
#define XCONTIKI_OS_SYS_PROCESS__EVENT_SERVICE_REMOVED 0x84
#define XCONTIKI_OS_SYS_PROCESS__EVENT_CONTINUE        0x85
#define XCONTIKI_OS_SYS_PROCESS__EVENT_MSG             0x86
#define XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED          0x87
#define XCONTIKI_OS_SYS_PROCESS__EVENT_TIMER           0x88
#define XCONTIKI_OS_SYS_PROCESS__EVENT_COM             0x89
#define XCONTIKI_OS_SYS_PROCESS__EVENT_MAX             0x8a

#define XCONTIKI_OS_SYS_PROCESS__BROADCAST NULL
#define XCONTIKI_OS_SYS_PROCESS__ZOMBIE ((struct xcontiki_os_sys_Process *)0x1)

/**
 * \name Process protothread functions
 * @{
 */

/**
 * Define the beginning of a process.
 *
 * This macro defines the beginning of a process, and must always
 * appear in a XCONTIKI_OS_SYS_PROCESS__THREAD() definition. The XCONTIKI_OS_SYS_PROCESS__END() macro
 * must come at the end of the process.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__BEGIN()             XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(process_pt)

/**
 * Define the end of a process.
 *
 * This macro defines the end of a process. It must appear in a
 * XCONTIKI_OS_SYS_PROCESS__THREAD() definition and must always be included. The
 * process exits when the XCONTIKI_OS_SYS_PROCESS__END() macro is reached.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__END()               XCONTIKI_OS_SYS_PROTOTHREAD__END(process_pt)

/**
 * Wait for an event to be posted to the process.
 *
 * This macro blocks the currently running process until the process
 * receives an event.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT()        XCONTIKI_OS_SYS_PROCESS__YIELD()

/**
 * Wait for an event to be posted to the process, with an extra
 * condition.
 *
 * This macro is similar to XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT() in that it blocks the
 * currently running process until the process receives an event. But
 * XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT_UNTIL() takes an extra condition which must be
 * true for the process to continue.
 *
 * \param c The condition that must be true for the process to continue.
 * \sa XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL()
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT_UNTIL(c) XCONTIKI_OS_SYS_PROCESS__YIELD_UNTIL(c)

/**
 * Yield the currently running process.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__YIELD()             XCONTIKI_OS_SYS_PROTOTHREAD__YIELD(process_pt)

/**
 * Yield the currently running process until a condition occurs.
 *
 * This macro is different from XCONTIKI_OS_SYS_PROCESS__WAIT_UNTIL() in that
 * XCONTIKI_OS_SYS_PROCESS__YIELD_UNTIL() is guaranteed to always yield at least
 * once. This ensures that the process does not end up in an infinite
 * loop and monopolizing the CPU.
 *
 * \param c The condition to wait for.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__YIELD_UNTIL(c)      XCONTIKI_OS_SYS_PROTOTHREAD__YIELD_UNTIL(process_pt, c)

/**
 * Wait for a condition to occur.
 *
 * This macro does not guarantee that the process yields, and should
 * therefore be used with care. In most cases, XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT(),
 * XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT_UNTIL(), XCONTIKI_OS_SYS_PROCESS__YIELD() or
 * XCONTIKI_OS_SYS_PROCESS__YIELD_UNTIL() should be used instead.
 *
 * \param c The condition to wait for.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__WAIT_UNTIL(c)       XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL(process_pt, c)
#define XCONTIKI_OS_SYS_PROCESS__WAIT_WHILE(c)       XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_WHILE(process_pt, c)

/**
 * Exit the currently running process.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__EXIT()              XCONTIKI_OS_SYS_PROTOTHREAD__EXIT(process_pt)

/**
 * Spawn a protothread from the process.
 *
 * \param pt The protothread state (xcontiki_os_sys_Protothread__pt_t) for the new protothread
 * \param thread The call to the protothread function.
 * \sa XCONTIKI_OS_SYS_PROTOTHREAD__SPAWN()
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__PROTOTHREAD__SPAWN(pt, thread)   XCONTIKI_OS_SYS_PROTOTHREAD__SPAWN(process_pt, pt, thread)

/**
 * Yield the process for a short while.
 *
 * This macro yields the currently running process for a short while,
 * thus letting other processes run before the process continues.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__PAUSE()             do {    \
  xcontiki_os_sys_Process__post_event_via_queue(XCONTIKI_OS_SYS_PROCESS__CURRENT(), XCONTIKI_OS_SYS_PROCESS__EVENT_CONTINUE, NULL); \
  XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT_UNTIL(ev == XCONTIKI_OS_SYS_PROCESS__EVENT_CONTINUE);               \
} while(0)

/** @} end of protothread functions */

/**
 * \name Poll and exit handlers
 * @{
 */
/**
 * Specify an action when a process is polled.
 *
 * \note This declaration must come immediately before the
 * XCONTIKI_OS_SYS_PROCESS__BEGIN() macro.
 *
 * \param handler The action to be performed.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__POLLHANDLER(handler) if(ev == XCONTIKI_OS_SYS_PROCESS__EVENT_POLL) { handler; }

/**
 * Specify an action when a process exits.
 *
 * \note This declaration must come immediately before the
 * XCONTIKI_OS_SYS_PROCESS__BEGIN() macro.
 *
 * \param handler The action to be performed.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__EXITHANDLER(handler) if(ev == XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT) { handler; }

/** @} */

/**
 * \name Process declaration and definition
 * @{
 */

/**
 * Define the body of a process.
 *
 * This macro is used to define the body (protothread) of a
 * process. The process is called whenever an event occurs in the
 * system, A process always start with the XCONTIKI_OS_SYS_PROCESS__BEGIN() macro and
 * end with the XCONTIKI_OS_SYS_PROCESS__END() macro.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__THREAD(name, ev, data) \
                static\
                xcontiki_os_sys_Protothread__state_t process_thread_##name##_internal(\
                                        xcontiki_os_sys_Process__event_t,\
				                                xcontiki_os_sys_Process__data_t);\
                static xcontiki_os_sys_Protothread__pt_t process_pt;\
                static \
                xcontiki_os_sys_Protothread__state_t process_thread_##name(\
				       xcontiki_os_sys_Process__event_t ev,\
				       xcontiki_os_sys_Process__data_t data){\
               static xcontiki_os_sys_Protothread__state_t process_thread_state;\
               process_pt = process_thread_##name##_pt;\
               process_thread_state = process_thread_##name##_internal(ev,data);\
               process_thread_##name##_pt=process_pt;\
               return process_thread_state;\
             }\
static xcontiki_os_sys_Protothread__state_t process_thread_##name##_internal(\
                                        xcontiki_os_sys_Process__event_t ev,\
				        xcontiki_os_sys_Process__data_t data)

/**
 * Declare the name of a process.
 *
 * This macro is typically used in header files to declare the name of
 * a process that is implemented in the C file.
 *
 * \hideinitializer
 */
#define XCONTIKI_OS_SYS_PROCESS__NAME(name) extern struct xcontiki_os_sys_Process name

/**
 * Declare a process.
 *
 * This macro declares a process. The process has two names: the
 * variable of the process structure, which is used by the C program,
 * and a human readable string name, which is used when debugging.
 * A configuration option allows removal of the readable name to save RAM.
 *
 * \param name The variable name of the process structure.
 * \param strname The string representation of the process' name.
 *
 * \hideinitializer
 */

#define XCONTIKI_OS_SYS_PROCESS(name, strname)    \
  static xcontiki_os_sys_Protothread__state_t process_thread_##name(\
                                        xcontiki_os_sys_Process__event_t,\
                                        xcontiki_os_sys_Process__data_t);   \
  static xcontiki_os_sys_Protothread__pt_t process_thread_##name##_pt={0}; \
  struct xcontiki_os_sys_Process name = { NULL,  \
                          process_thread_##name }


/** @} */

#define XCONTIKI_OS_SYS_PROCESS__NAME_STRING(process) ""

struct xcontiki_os_sys_Process {
    struct xcontiki_os_sys_Process *next;
    xcontiki_os_sys_Protothread__state_t(* thread)(xcontiki_os_sys_Process__event_t, xcontiki_os_sys_Process__data_t);
    unsigned running:1;
    unsigned called:1;
    unsigned needspoll :1;
    unsigned marked_to_exit:1;
};

/**
 * \name Functions called from application programs
 * @{
 */

/**
 * Start a process.
 *
 * \param p A pointer to a process structure.
 *
 * \param data An argument that can be passed to the new
 * process
 *
 */
void xcontiki_os_sys_Process__start(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__data_t data);

/**
 * Post an asynchronous event.
 *
 * This function posts an asynchronous event to one or more
 * processes. The handing of the event is deferred until the target
 * process is scheduled by the kernel. An event can be broadcast to
 * all processes, in which case all processes in the system will be
 * scheduled to handle the event.
 *
 * \param ev The event to be posted.
 *
 * \param data The auxiliary data to be sent with the event
 *
 * \param p The process to which the event should be posted, or
 * XCONTIKI_OS_SYS_PROCESS__BROADCAST if the event should be posted to all processes.
 *
 * \retval XCONTIKI_OS_SYS_PROCESS__ERR_OK The event could be posted.
 *
 * \retval XCONTIKI_OS_SYS_PROCESS__ERR_FULL The event queue was full and the event could
 * not be posted.
 */
int8_t xcontiki_os_sys_Process__post_event_via_queue(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data);

/**
 * Post a synchronous event to a process.
 *
 * \param p A pointer to the process' process structure.
 *
 * \param ev The event to be posted.
 *
 * \param data A pointer to additional data that is posted together
 * with the event.
 */
void xcontiki_os_sys_Process__post_synchronous_event(struct xcontiki_os_sys_Process *p,
        xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data);

/**
 * \brief      Cause a process to exit
 * \param p    The process that is to be exited
 *
 *             This function causes a process to exit. The process can
 *             either be the currently executing process, or another
 *             process that is currently running.
 *
 * \sa XCONTIKI_OS_SYS_PROCESS__CURRENT()
 */
void xcontiki_os_sys_Process__exit(struct xcontiki_os_sys_Process *p);


/**
 * Get a pointer to the currently running process.
 *
 * This macro get a pointer to the currently running
 * process. Typically, this macro is used to post an event to the
 * current process with xcontiki_os_sys_process_post().
 *
 * \hideinitializer
 */
struct xcontiki_os_sys_Process * xcontiki_os_sys_Process__get_current_process(void);

/**
 * Switch context to another process
 *
 * This function switch context to the specified process and executes
 * the code as if run by that process. Typical use of this function is
 * to switch context in services, called by other processes. Each
 * XCONTIKI_OS_SYS_PROCESS__CONTEXT_BEGIN() must be followed by the
 * XCONTIKI_OS_SYS_PROCESS__CONTEXT_END() macro to end the context switch.
 *
 * Example:
 \code
 XCONTIKI_OS_SYS_PROCESS__CONTEXT_BEGIN(&test_process);
 etimer_set(&timer, CLOCK_SECOND);
 XCONTIKI_OS_SYS_PROCESS__CONTEXT_END(&test_process);
 \endcode
 *
 * \param p    The process to use as context
 *
 * \sa XCONTIKI_OS_SYS_PROCESS__CONTEXT_END()
 * \sa XCONTIKI_OS_SYS_PROCESS__CURRENT()
 */
#define XCONTIKI_OS_SYS_PROCESS__CONTEXT_BEGIN(p) {\
struct xcontiki_os_sys_Process *tmp_current = XCONTIKI_OS_SYS_PROCESS__CURRENT();\
xcontiki_os_sys_Process__current = p

/**
 * End a context switch
 *
 * This function ends a context switch and changes back to the
 * previous process.
 *
 * \param p    The process used in the context switch
 *
 * \sa XCONTIKI_OS_SYS_PROCESS__CONTEXT_START()
 */
#define XCONTIKI_OS_SYS_PROCESS__CONTEXT_END(p) current_process_ptr = tmp_current; }

/**
 * \brief      Allocate a global event number.
 * \return     The allocated event number
 *
 *             In Contiki, event numbers above 128 are global and may
 *             be posted from one process to another. This function
 *             allocates one such event number.
 *
 * \note       There currently is no way to deallocate an allocated event
 *             number.
 */
xcontiki_os_sys_Process__event_t xcontiki_os_sys_Process__alloc_event(void);

/** @} */

/**
 * \name Functions called from device drivers
 * @{
 */

/**
 * Request a process to be polled.
 *
 * This function typically is called from an interrupt handler to
 * cause a process to be polled.
 *
 * \param p A pointer to the process' process structure.
 */
void xcontiki_os_sys_Process__poll(struct xcontiki_os_sys_Process *p);

/** @} */

/**
 * \name Functions called by the system and boot-up code
 * @{
 */

/**
 * \brief      Initialize the process module.
 *
 *             This function initializes the process module and should
 *             be called by the system boot-up code.
 */
void xcontiki_os_sys_Process__initialize_the_process_module(void);

/**
 * Run the system once - call poll handlers and process one event.
 *
 * This function should be called repeatedly from the main() program
 * to actually run the Contiki system. It calls the necessary poll
 * handlers, and processes one event. The function returns the number
 * of events that are waiting in the event queue so that the caller
 * may choose to put the CPU to sleep when there are no pending
 * events.
 *
 * \return The number of events that are currently waiting in the
 * event queue.
 */
int xcontiki_os_sys_Process__process_next_event(void);


/**
 * Check if a process is running.
 *
 * This function checks if a specific process is running.
 *
 * \param p The process.
 * \retval Non-zero if the process is running.
 * \retval Zero if the process is not running.
 */
bool xcontiki_os_sys_Process__is_running(struct xcontiki_os_sys_Process *p);

/**
 *  Number of events waiting to be processed.
 *
 * \return The number of events that are currently waiting to be
 * processed.
 */
int xcontiki_os_sys_Process__number_of_events_waiting(void);

/** @} */


#endif /* XCONTIKI_OS_SYS_XCONTIKI_OS_SYS_PROCESS__H */

/** @} */
/** @} */
