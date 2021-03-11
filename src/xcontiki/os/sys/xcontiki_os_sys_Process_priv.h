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

#define STATE_NONE        0
#define STATE_RUNNING     1
#define STATE_CALLED      2

static void call_process(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data);

#if DEBUG
#include <stdio.h>
#define PRINTF(...)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
static void
exit_process(struct xcontiki_os_sys_Process *p, struct xcontiki_os_sys_Process *fromprocess)
{
  register struct xcontiki_os_sys_Process *q;
  struct xcontiki_os_sys_Process *old_current = current_process_ptr;

  PRINTF("process: exit_process '%s'\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p));

  /* Make sure the process is in the process list before we try to
     exit it. */
  for(q = list_of_processes; q != p && q != NULL; q = q->next);
  if(q == NULL) {
    return;
  }

  if(xcontiki_os_sys_Process__is_running(p)) {
    /* Process was running */
    p->state = STATE_NONE;

    /*
     * Post a synchronous event to all processes to inform them that
     * this process is about to exit. This will allow services to
     * deallocate state associated with this process.
     */
    for(q = list_of_processes; q != NULL; q = q->next) {
      if(p != q) {
        call_process(q, XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED, (xcontiki_os_sys_Process__data_t)p);
      }
    }

    if(p->thread != NULL && p != fromprocess) {
      /* Post the exit event to the process that is about to exit. */
      current_process_ptr = p;
      p->thread( XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT, NULL);
    }
  }

  if(p == list_of_processes) {
    list_of_processes = list_of_processes->next;
  } else {
    for(q = list_of_processes; q != NULL; q = q->next) {
      if(q->next == p) {
        q->next = p->next;
        break;
      }
    }
  }

  current_process_ptr = old_current;
}
/*---------------------------------------------------------------------------*/
static void
call_process(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data)
{
  int ret;

#if DEBUG
  if(p->state == STATE_CALLED) {
    printf("process: process '%s' called again with event %d\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), ev);
  }
#endif /* DEBUG */

  if((p->state & STATE_RUNNING) &&
     p->thread != NULL) {
    PRINTF("process: calling process '%s' with event %d\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), ev);
    current_process_ptr = p;
    p->state = STATE_CALLED;
    ret = p->thread( ev, data);
    if(ret == XCONTIKI_OS_SYS_PROTOTHREAD__EXITED ||
       ret == XCONTIKI_OS_SYS_PROTOTHREAD__ENDED ||
       ev == XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT) {
      exit_process(p, p);
    } else {
      p->state = STATE_RUNNING;
    }
  }
}

/*---------------------------------------------------------------------------*/
/*
 * Call each process' poll handler.
 */
/*---------------------------------------------------------------------------*/
static void
do_poll(void)
{
  struct xcontiki_os_sys_Process *p;

  poll_requested = 0;
  /* Call the processes that needs to be polled. */
  for(p = list_of_processes; p != NULL; p = p->next) {
    if(p->needspoll) {
      p->state = STATE_RUNNING;
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
do_event(void)
{
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

  if(nevents > 0) {

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
    if(receiver == XCONTIKI_OS_SYS_PROCESS__BROADCAST) {
      for(p = list_of_processes; p != NULL; p = p->next) {

        /* If we have been requested to poll a process, we do this in
           between processing the broadcast event. */
        if(poll_requested) {
          do_poll();
        }
        call_process(p, ev, data);
      }
    } else {
      /* This is not a broadcast event, so we deliver it to the
         specified process. */
      /* If the event was an INIT event, we should also update the
         state of the process. */
      if(ev == XCONTIKI_OS_SYS_PROCESS__EVENT_INIT) {
        receiver->state = STATE_RUNNING;
      }

      /* Make sure that the process actually is running. */
      call_process(receiver, ev, data);
    }
  }
}

#ifdef __cplusplus
}
#endif
