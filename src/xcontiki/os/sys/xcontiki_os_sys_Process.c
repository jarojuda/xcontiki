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
 * \addtogroup process
 * @{
 */

/**
 * \file
 *         Implementation of the Contiki process kernel.
 * \author
 *         Adam Dunkels <adam@sics.se>
 *
 */

#include "xcontiki/xcontiki.h"

#define XCONTIKI_OS_SYS_PROCESS_PRIV_H
#include "xcontiki_os_sys_Process_priv.h"

/*---------------------------------------------------------------------------*/
xcontiki_os_sys_Process__event_t
xcontiki_os_sys_Process__alloc_event(void) {
    return lastevent++;
}

/*---------------------------------------------------------------------------*/
struct xcontiki_os_sys_Process * xcontiki_os_sys_Process__get_current_process(void) {
    return current_process_ptr;
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__start(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__data_t data) {
    struct xcontiki_os_sys_Process *q;

    /* First make sure that we don't try to start a process that is
       already running. */
    for (q = list_of_processes; q != p && q != NULL; q = q->next);

    /* If we found the process on the process list, we bail out. */
    if (q == p) {
        return;
    }
    /* Put on the procs list.*/
    p->next = list_of_processes;
    list_of_processes = p;
    p->running = true;
    p->called = false;
    p->marked_to_exit = false;

    PRINTF(("process: starting '%s'\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p)));

    /* Post a synchronous initialization event to the process. */
    xcontiki_os_sys_Process__post_synchronous_event(p, XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, data);
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__exit(struct xcontiki_os_sys_Process *p) {
    exit_process(p, xcontiki_os_sys_Process__get_current_process());
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__initialize_the_process_module(void) {
    lastevent = XCONTIKI_OS_SYS_PROCESS__EVENT_MAX;

    nevents = fevent = 0;
#if XCONTIKI_OS_SYS_PROCESS__CONF_STATS
    xcontiki_os_sys_process_maxevents = 0;
#endif /* XCONTIKI_OS_SYS_PROCESS__CONF_STATS */

    current_process_ptr = list_of_processes = NULL;
}

/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Process__process_next_event(void) {
    /* Process poll events. */
    if (poll_requested) {
        do_poll();
    }

    /* Process one event from the queue */
    do_event();

    return nevents + poll_requested;
}

/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Process__number_of_events_waiting(void) {
    return nevents + poll_requested;
}

/*---------------------------------------------------------------------------*/
int8_t
xcontiki_os_sys_Process__post_event_via_queue(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data) {
    xcontiki_os_sys_Process__num_events_t snum;

    if (xcontiki_os_sys_Process__get_current_process() == NULL) {
        PRINTF(("xcontiki_os_sys_process_post: NULL process posts event %d to process '%s', nevents %d\n",
                ev, XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), nevents));
    } else {
        PRINTF(("xcontiki_os_sys_process_post: Process '%s' posts event %d to process '%s', nevents %d\n",
                XCONTIKI_OS_SYS_PROCESS__NAME_STRING(xcontiki_os_sys_Process__get_current_process()), ev,
                p == XCONTIKI_OS_SYS_PROCESS__BROADCAST ? "<broadcast>" : XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), nevents));
    }

    if (nevents == XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS) {
#if DEBUG
        if (p == XCONTIKI_OS_SYS_PROCESS__BROADCAST) {
            printf("soft panic: event queue is full when broadcast event %d was posted from %s\n", ev, XCONTIKI_OS_SYS_PROCESS__NAME_STRING(current_process_ptr));
        } else {
            printf("soft panic: event queue is full when event %d was posted to %s from %s\n", ev, XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), XCONTIKI_OS_SYS_PROCESS__NAME_STRING(current_process_ptr));
        }
#endif /* DEBUG */
        return XCONTIKI_OS_SYS_PROCESS__ERR_FULL;
    }

    snum = (xcontiki_os_sys_Process__num_events_t) (fevent + nevents) % XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS;
    events_ev[snum] = ev;
    events_data[snum] = data;
    events_destination_process_ptr[snum] = p;
    ++nevents;

#if XCONTIKI_OS_SYS_PROCESS__CONF_STATS
    if (nevents > xcontiki_os_sys_process_maxevents) {
        xcontiki_os_sys_process_maxevents = nevents;
    }
#endif /* XCONTIKI_OS_SYS_PROCESS__CONF_STATS */

    return XCONTIKI_OS_SYS_PROCESS__ERR_OK;
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__post_synchronous_event(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data) {
    struct xcontiki_os_sys_Process *caller = current_process_ptr;

    call_process(p, ev, data);
    current_process_ptr = caller;
}

/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__poll(struct xcontiki_os_sys_Process *p) {
    if (p != NULL) {
        if (p->running || p->called) {
            p->needspoll = 1;
            poll_requested = 1;
        }
    }
}

/*---------------------------------------------------------------------------*/
bool
xcontiki_os_sys_Process__is_running(struct xcontiki_os_sys_Process *p) {
    return (p->running || p->called);
}
/*---------------------------------------------------------------------------*/
/** @} */
