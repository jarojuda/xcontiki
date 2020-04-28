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


/*
 * Pointer to the currently running process structure.
 */
struct xcontiki_os_sys_Process *xcontiki_os_sys_Process__list = NULL;
struct xcontiki_os_sys_Process *xcontiki_os_sys_Process__current = NULL;

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
xcontiki_os_sys_Process__num_events_t xcontiki_os_sys_process_maxevents;
#endif

static volatile unsigned char poll_requested;

#define STATE_NONE        0
#define STATE_RUNNING     1
#define STATE_CALLED      2

static void call_process(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data);

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
xcontiki_os_sys_Process__event_t
xcontiki_os_sys_Process__alloc_event(void)
{
  return lastevent++;
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__start(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__data_t data)
{
  struct xcontiki_os_sys_Process *q;

  /* First make sure that we don't try to start a process that is
     already running. */
  for(q = xcontiki_os_sys_Process__list; q != p && q != NULL; q = q->next);

  /* If we found the process on the process list, we bail out. */
  if(q == p) {
    return;
  }
  /* Put on the procs list.*/
  p->next = xcontiki_os_sys_Process__list;
  xcontiki_os_sys_Process__list = p;
  p->state = STATE_RUNNING;
  XCONTIKI_OS_SYS_PROTOTHREAD__INIT(&p->pt);

  PRINTF("process: starting '%s'\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p));

  /* Post a synchronous initialization event to the process. */
  xcontiki_os_sys_Process__post_synch(p, XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, data);
}
/*---------------------------------------------------------------------------*/
static void
exit_process(struct xcontiki_os_sys_Process *p, struct xcontiki_os_sys_Process *fromprocess)
{
  register struct xcontiki_os_sys_Process *q;
  struct xcontiki_os_sys_Process *old_current = xcontiki_os_sys_Process__current;

  PRINTF("process: exit_process '%s'\n", XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p));

  /* Make sure the process is in the process list before we try to
     exit it. */
  for(q = xcontiki_os_sys_Process__list; q != p && q != NULL; q = q->next);
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
    for(q = xcontiki_os_sys_Process__list; q != NULL; q = q->next) {
      if(p != q) {
        call_process(q, XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED, (xcontiki_os_sys_Process__data_t)p);
      }
    }

    if(p->thread != NULL && p != fromprocess) {
      /* Post the exit event to the process that is about to exit. */
      xcontiki_os_sys_Process__current = p;
      p->thread(&p->pt, XCONTIKI_OS_SYS_PROCESS__EVENT_EXIT, NULL);
    }
  }

  if(p == xcontiki_os_sys_Process__list) {
    xcontiki_os_sys_Process__list = xcontiki_os_sys_Process__list->next;
  } else {
    for(q = xcontiki_os_sys_Process__list; q != NULL; q = q->next) {
      if(q->next == p) {
        q->next = p->next;
        break;
      }
    }
  }

  xcontiki_os_sys_Process__current = old_current;
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
    xcontiki_os_sys_Process__current = p;
    p->state = STATE_CALLED;
    ret = p->thread(&p->pt, ev, data);
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
void
xcontiki_os_sys_Process__exit(struct xcontiki_os_sys_Process *p)
{
  exit_process(p, XCONTIKI_OS_SYS_PROCESS__CURRENT());
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__init(void)
{
  lastevent = XCONTIKI_OS_SYS_PROCESS__EVENT_MAX;

  nevents = fevent = 0;
#if XCONTIKI_OS_SYS_PROCESS__CONF_STATS
  xcontiki_os_sys_process_maxevents = 0;
#endif /* XCONTIKI_OS_SYS_PROCESS__CONF_STATS */

  xcontiki_os_sys_Process__current = xcontiki_os_sys_Process__list = NULL;
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
  for(p = xcontiki_os_sys_Process__list; p != NULL; p = p->next) {
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
      for(p = xcontiki_os_sys_Process__list; p != NULL; p = p->next) {

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
/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Process__run(void)
{
  /* Process poll events. */
  if(poll_requested) {
    do_poll();
  }

  /* Process one event from the queue */
  do_event();

  return nevents + poll_requested;
}
/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Process__nevents(void)
{
  return nevents + poll_requested;
}
/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Process__post(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data)
{
  xcontiki_os_sys_Process__num_events_t snum;

  if(XCONTIKI_OS_SYS_PROCESS__CURRENT() == NULL) {
    PRINTF("xcontiki_os_sys_process_post: NULL process posts event %d to process '%s', nevents %d\n",
           ev, XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), nevents);
  } else {
    PRINTF("xcontiki_os_sys_process_post: Process '%s' posts event %d to process '%s', nevents %d\n",
           XCONTIKI_OS_SYS_PROCESS__NAME_STRING(XCONTIKI_OS_SYS_PROCESS__CURRENT()), ev,
           p == XCONTIKI_OS_SYS_PROCESS__BROADCAST ? "<broadcast>" : XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), nevents);
  }

  if(nevents == XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS) {
#if DEBUG
    if(p == XCONTIKI_OS_SYS_PROCESS__BROADCAST) {
      printf("soft panic: event queue is full when broadcast event %d was posted from %s\n", ev, XCONTIKI_OS_SYS_PROCESS__NAME_STRING(xcontiki_os_sys_Process__current));
    } else {
      printf("soft panic: event queue is full when event %d was posted to %s from %s\n", ev, XCONTIKI_OS_SYS_PROCESS__NAME_STRING(p), XCONTIKI_OS_SYS_PROCESS__NAME_STRING(xcontiki_os_sys_Process__current));
    }
#endif /* DEBUG */
    return XCONTIKI_OS_SYS_PROCESS__ERR_FULL;
  }

  snum = (xcontiki_os_sys_Process__num_events_t)(fevent + nevents) % XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS;
  events[snum].ev = ev;
  events[snum].data = data;
  events[snum].p = p;
  ++nevents;

#if XCONTIKI_OS_SYS_PROCESS__CONF_STATS
  if(nevents > xcontiki_os_sys_process_maxevents) {
    xcontiki_os_sys_process_maxevents = nevents;
  }
#endif /* XCONTIKI_OS_SYS_PROCESS__CONF_STATS */

  return XCONTIKI_OS_SYS_PROCESS__ERR_OK;
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__post_synch(struct xcontiki_os_sys_Process *p, xcontiki_os_sys_Process__event_t ev, xcontiki_os_sys_Process__data_t data)
{
  struct xcontiki_os_sys_Process *caller = xcontiki_os_sys_Process__current;

  call_process(p, ev, data);
  xcontiki_os_sys_Process__current = caller;
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Process__poll(struct xcontiki_os_sys_Process *p)
{
  if(p != NULL) {
    if(p->state == STATE_RUNNING ||
       p->state == STATE_CALLED) {
      p->needspoll = 1;
      poll_requested = 1;
    }
  }
}
/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Process__is_running(struct xcontiki_os_sys_Process *p)
{
  return p->state != STATE_NONE;
}
/*---------------------------------------------------------------------------*/
/** @} */
