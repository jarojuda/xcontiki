/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* Based on the Contiki http://contiki-os.org
 * and the Contiki-NG  http://www.contiki-ng.org/
 * projects. 
 */

/* 
 * File:   xcontiki.c
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"

static bool first_run = true;
static void init(void);

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD xcontiki__main(void) {

    if (first_run) {
        init();
        first_run = false;
    }

    xcontiki_arch_dev_Watchdog__periodic();
    if (xcontiki_os_sys_Process__number_of_events_waiting() > 0) {
        xcontiki_os_sys_Process__process_next_event();
    }
    if (xcontiki_os_sys_Process__number_of_events_waiting() > 0) {
        return XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
    }
    return XCONTIKI_OS_SYS_PROTOTHREAD__WAITING;
}

void xcontiki__init(void) {
    first_run = true;

}

static void init(void) {
    xcontiki_arch_dev_Watchdog__init();
    xcontiki_arch_dev_Watchdog__start();
    xcontiki_arch_os_sys_Clock__init();
    xcontiki_os_sys_Process__initialize_the_process_module();
}
