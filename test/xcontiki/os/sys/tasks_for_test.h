#ifndef TASKS_FOR_TEST
#define TASKS_FOR_TEST

#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task1_that_runs_everytime(void);

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task2_started_every_second(void);

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task3_with_max_interval(void);

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task4_started_every_100milisecond(void);

#endif
