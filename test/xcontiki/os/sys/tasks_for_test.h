#ifndef TASKS_FOR_TEST_H
#define TASKS_FOR_TEST_H

#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"

xcontiki_os_sys_Protothread__state_t task1_that_runs_everytime(void);

xcontiki_os_sys_Protothread__state_t task2_started_in_the_meantime(void);

xcontiki_os_sys_Protothread__state_t task3_with_max_interval(void);

#endif
