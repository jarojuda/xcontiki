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
 * \addtogroup etimer
 * @{
 */

/**
 * \file
 * Event timer library implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 */

#include "xcontiki/xcontiki.h"



static struct xcontiki_os_sys_Etimer *timerlist;
static arch_xcontiki_os_sys_Clock__time_t next_expiration;

XCONTIKI_OS_SYS_PROCESS(etimer_process, "Event timer");
/*---------------------------------------------------------------------------*/
static void
update_time(void)
{
  arch_xcontiki_os_sys_Clock__time_t tdist;
  arch_xcontiki_os_sys_Clock__time_t now;
  struct xcontiki_os_sys_Etimer *t;

  if(timerlist == NULL) {
    next_expiration = 0;
  } else {
    now = arch_xcontiki_os_sys_Clock__time();
    t = timerlist;
    /* Must calculate distance to next time into account due to wraps */
    tdist = t->timer.start + t->timer.interval - now;
    for(t = t->next; t != NULL; t = t->next) {
      if(t->timer.start + t->timer.interval - now < tdist) {
        tdist = t->timer.start + t->timer.interval - now;
      }
    }
    next_expiration = now + tdist;
  }
}
/*---------------------------------------------------------------------------*/
XCONTIKI_OS_SYS_PROCESS__THREAD(etimer_process, ev, data)
{
  struct xcontiki_os_sys_Etimer *t, *u;

  XCONTIKI_OS_SYS_PROCESS__BEGIN();

  timerlist = NULL;

  while(1) {
    XCONTIKI_OS_SYS_PROCESS__YIELD();

    if(ev == XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED) {
      struct xcontiki_os_sys_Process *p = data;

      while(timerlist != NULL && timerlist->p == p) {
        timerlist = timerlist->next;
      }

      if(timerlist != NULL) {
        t = timerlist;
        while(t->next != NULL) {
          if(t->next->p == p) {
            t->next = t->next->next;
          } else {
            t = t->next;
          }
        }
      }
      continue;
    } else if(ev != XCONTIKI_OS_SYS_PROCESS__EVENT_POLL) {
      continue;
    }

again:

    u = NULL;

    for(t = timerlist; t != NULL; t = t->next) {
      if(xcontiki_os_sys_Timer__expired(&t->timer)) {
        if(xcontiki_os_sys_Process__post(t->p, XCONTIKI_OS_SYS_PROCESS__EVENT_TIMER, t) == XCONTIKI_OS_SYS_PROCESS__ERR_OK) {

          /* Reset the process ID of the event timer, to signal that the
             etimer has expired. This is later checked in the
             etimer_expired() function. */
          t->p = XCONTIKI_OS_SYS_PROCESS__NONE;
          if(u != NULL) {
            u->next = t->next;
          } else {
            timerlist = t->next;
          }
          t->next = NULL;
          update_time();
          goto again;
        } else {
          xcontiki_os_sys_Etimer__request_poll();
        }
      }
      u = t;
    }
  }

  XCONTIKI_OS_SYS_PROCESS__END();
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__request_poll(void)
{
  xcontiki_os_sys_Process__poll(&etimer_process);
}
/*---------------------------------------------------------------------------*/
static void
add_timer(struct xcontiki_os_sys_Etimer *xcontiki_os_sys_Timer__timer)
{
  struct xcontiki_os_sys_Etimer *t;

  xcontiki_os_sys_Etimer__request_poll();

  if(xcontiki_os_sys_Timer__timer->p != XCONTIKI_OS_SYS_PROCESS__NONE) {
    for(t = timerlist; t != NULL; t = t->next) {
      if(t == xcontiki_os_sys_Timer__timer) {
        /* Timer already on list, bail out. */
        xcontiki_os_sys_Timer__timer->p = XCONTIKI_OS_SYS_PROCESS__CURRENT();
        update_time();
        return;
      }
    }
  }

  /* Timer not on list. */
  xcontiki_os_sys_Timer__timer->p = XCONTIKI_OS_SYS_PROCESS__CURRENT();
  xcontiki_os_sys_Timer__timer->next = timerlist;
  timerlist = xcontiki_os_sys_Timer__timer;

  update_time();
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__set(struct xcontiki_os_sys_Etimer *et, arch_xcontiki_os_sys_Clock__time_t interval)
{
  xcontiki_os_sys_Timer__set(&et->timer, interval);
  add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__reset_with_new_interval(struct xcontiki_os_sys_Etimer *et, arch_xcontiki_os_sys_Clock__time_t interval)
{
  xcontiki_os_sys_Timer__reset(&et->timer);
  et->timer.interval = interval;
  add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__reset(struct xcontiki_os_sys_Etimer *et)
{
  xcontiki_os_sys_Timer__reset(&et->timer);
  add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__restart(struct xcontiki_os_sys_Etimer *et)
{
  xcontiki_os_sys_Timer__restart(&et->timer);
  add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__adjust(struct xcontiki_os_sys_Etimer *et, int timediff)
{
  et->timer.start += timediff;
  update_time();
}
/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Etimer__expired(struct xcontiki_os_sys_Etimer *et)
{
  return et->p == XCONTIKI_OS_SYS_PROCESS__NONE;
}
/*---------------------------------------------------------------------------*/
arch_xcontiki_os_sys_Clock__time_t
xcontiki_os_sys_Etimer__expiration_time(struct xcontiki_os_sys_Etimer *et)
{
  return et->timer.start + et->timer.interval;
}
/*---------------------------------------------------------------------------*/
arch_xcontiki_os_sys_Clock__time_t
etimer_start_time(struct xcontiki_os_sys_Etimer *et)
{
  return et->timer.start;
}
/*---------------------------------------------------------------------------*/
int
xcontiki_os_sys_Etimer__pending(void)
{
  return timerlist != NULL;
}
/*---------------------------------------------------------------------------*/
arch_xcontiki_os_sys_Clock__time_t
xcontiki_os_sys_Etimer__next_expiration_time(void)
{
  return xcontiki_os_sys_Etimer__pending() ? next_expiration : 0;
}
/*---------------------------------------------------------------------------*/
void
xcontiki_os_sys_Etimer__stop(struct xcontiki_os_sys_Etimer *et)
{
  struct xcontiki_os_sys_Etimer *t;

  /* First check if et is the first event timer on the list. */
  if(et == timerlist) {
    timerlist = timerlist->next;
    update_time();
  } else {
    /* Else walk through the list and try to find the item before the
       et timer. */
    for(t = timerlist; t != NULL && t->next != et; t = t->next) {
    }

    if(t != NULL) {
      /* We've found the item before the event timer that we are about
         to remove. We point the items next pointer to the event after
         the removed item. */
      t->next = et->next;

      update_time();
    }
  }

  /* Remove the next pointer from the item to be removed. */
  et->next = NULL;
  /* Set the timer as expired */
  et->p = XCONTIKI_OS_SYS_PROCESS__NONE;
}
/*---------------------------------------------------------------------------*/
/** @} */
