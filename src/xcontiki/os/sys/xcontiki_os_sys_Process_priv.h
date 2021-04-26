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

#include "xcontiki_os_sys_Process.h"


/* Based on the Contiki http://contiki-os.org
 * and the Contiki-NG  http://www.contiki-ng.org/
 * projects.
 */

/*
 * File:   xcontiki_os_sys_Process_priv.h
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#ifndef XCONTIKI_OS_SYS_PROCESS_PRIV_H
#error Include only from the xcontiki_os_sys_Process.c file
#endif /* XCONTIKI_OS_SYS_PROCESS_PRIV_H */

#ifdef __cplusplus
extern "C" {
#endif


    /*
     * Pointer to the currently running process structure.
     */
    static struct xcontiki_os_sys_Process *list_of_processes = NULL;
    static struct xcontiki_os_sys_Process *current_process_ptr = NULL;

    static xcontiki_os_sys_Process__event_t lastevent;

    /*
     * Structure used for keeping the queue of active events.
     */
    struct event_data {
        xcontiki_os_sys_Process__event_t ev;
        xcontiki_os_sys_Process__data_t data;
        struct xcontiki_os_sys_Process *p;
    };

    static xcontiki_os_sys_Process__num_events_t nevents, fevent;
    static struct event_data events[XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS];

#if XCONTIKI_OS_SYS_PROCESS__CONF_STATS
    static xcontiki_os_sys_Process__num_events_t maximum_number_of_events;
#endif

    static volatile unsigned char poll_requested;

#define STATE_NONE              0
#define STATE_RUNNING           1
#define STATE_CALLED            2
#define STATE_MARKED_TO_EXIT    4

    static void call_process(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data);

#if DEBUG
#include <stdio.h>
#define PRINTF(...)
#else
#define PRINTF(...)
#endif

    static void
    exit_process(struct xcontiki_os_sys_Process *p, struct xcontiki_os_sys_Process *fromprocess) {
        register struct xcontiki_os_sys_Process *q;
        struct xcontiki_os_sys_Process *old_current_process_ptr;
        XCONTIKI_OS_SYS_PROTOTHREAD__THREAD ret;

        if (NULL == p->thread) {
            return;
        }
        /* Make sure the process is in the process list before we try to
        exit it. */
        for (q = list_of_processes; q != p && q != NULL; q = q->next);
        if (q == NULL) {
            return;
        }
        old_current_process_ptr = current_process_ptr;
        p->marked_to_exit = true;
        static bool is_another_marked_to_exit;
        do {
            is_another_marked_to_exit = false;
            for (p = list_of_processes; p != NULL; p = p->next) {
                if (p->marked_to_exit) {
                    if (xcontiki_os_sys_Process__is_running(p)) {
                        /* Process was running */
                        p->running = false;
                        p->called = false;

                        /*
                         * Post a synchronous event to all processes to inform them that
                         * this process is about to exit. This will allow services to
                         * deallocate state associated with this process.
                         */
                        for (q = list_of_processes; q != NULL; q = q->next) {
                            if (p != q) {
                                if (q->running) {
                                    current_process_ptr = q;
                                    q->called = true;
                                    ret = q->thread(XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED, (xcontiki_os_sys_Process__data_t) p);
                                    if (ret == XCONTIKI_OS_SYS_PROTOTHREAD__EXITED || ret == XCONTIKI_OS_SYS_PROTOTHREAD__ENDED) {
                                        q->marked_to_exit = true;
                                        is_another_marked_to_exit = true;
                                    } else {
                                        q->called = false;
                                    }
                                }
                            }
                        }

                        if (p->thread != NULL && p != fromprocess && fromprocess != NULL) {
                            /* Post the exit event to the process that is about to exit. */
                            current_process_ptr = p;
                            p->thread(XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT, NULL);
                        }
                        fromprocess = NULL;
                    }
                    if (p == list_of_processes) {
                        list_of_processes = list_of_processes->next;
                    } else {
                        for (q = list_of_processes; q != NULL; q = q->next) {
                            if (q->next == p) {
                                q->next = p->next;
                                break;
                            }
                        }
                    }
                    p->marked_to_exit = false;
                }
            }
        } while (is_another_marked_to_exit);
        current_process_ptr = old_current_process_ptr;
    }

    /*---------------------------------------------------------------------------*/
    static void
    call_process(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data) {

        register struct xcontiki_os_sys_Process *q;
        XCONTIKI_OS_SYS_PROTOTHREAD__THREAD ret;

        if (NULL == p) {
            return;
        }
        if (NULL == p->thread) {
            return;
        }

#if DEBUG
        if (p->called) {
            printf("process: process '%s' called again with event %d\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), ev);
        }
#endif /* DEBUG */

        if (p->running && !p->called ) {
            PRINTF("process: calling process '%s' with event %d\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), ev);
            current_process_ptr = p;
            p->called = true;
            ret = p->thread(ev, data);
            if (ret == XCONTIKI_OS_SYS_PROTOTHREAD__EXITED ||
                    ret == XCONTIKI_OS_SYS_PROTOTHREAD__ENDED ||
                    ev == XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT) {
                exit_process(p, p);
            } else {
                p->called = false;
            }
        }
    }

    /*---------------------------------------------------------------------------*/
    /*
     * Call each process' poll handler.
     */

    /*---------------------------------------------------------------------------*/
    static void
    do_poll(void) {
        struct xcontiki_os_sys_Process *p;

        poll_requested = 0;
        /* Call the processes that needs to be polled. */
        for (p = list_of_processes; p != NULL; p = p->next) {
            if (p->needspoll) {
                p->running = true;
                p->needspoll = 0;
                call_process(p, XCONTIKI_OS_SYS_PROCESS__EVENT_POLL, NULL);
            }
        }
    }
    /*---------------------------------------------------------------------------*/
    /*
     * Process the next event in the event queue and deliver it to
     * listening processes.
     */

    /*---------------------------------------------------------------------------*/
    static void
    do_event(void) {
        xcontiki_os_sys_Process__event_t ev;
        xcontiki_os_sys_Process__data_t data;
        struct xcontiki_os_sys_Process *receiver;
        struct xcontiki_os_sys_Process *p;

        /*
         * If there are any events in the queue, take the first one and walk
         * through the list of processes to see if the event should be
         * delivered to any of them. If so, we call the event handler
         * function for the process. We only process one event at a time and
         * call the poll handlers inbetween.
         */

        if (nevents > 0) {

            /* There are events that we should deliver. */
            ev = events[fevent].ev;

            data = events[fevent].data;
            receiver = events[fevent].p;

            /* Since we have seen the new event, we move pointer upwards
               and decrease the number of events. */
            fevent = (fevent + 1) % XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS;
            --nevents;

            /* If this is a broadcast event, we deliver it to all events, in
               order of their priority. */
            if (receiver == XCONTIKI_OS_SYS_PROCESS__BROADCAST) {
                for (p = list_of_processes; p != NULL; p = p->next) {

                    /* If we have been requested to poll a process, we do this in
                       between processing the broadcast event. */
                    if (poll_requested) {
                        do_poll();
                    }
                    call_process(p, ev, data);
                }
            } else {
                /* This is not a broadcast event, so we deliver it to the
                   specified process. */
                /* If the event was an INIT event, we should also update the
                   state of the process. */
                if (ev == XCONTIKI_OS_SYS_PROCESS__EVENT_INIT) {
                    receiver->running = true;
                }

                /* Make sure that the process actually is running. */
                call_process(receiver, ev, data);
            }
        }
    }

#ifdef __cplusplus
}
#endif
