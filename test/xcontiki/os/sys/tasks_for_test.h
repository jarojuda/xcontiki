#ifndef TASKS_FOR_TEST_H
#define TASKS_FOR_TEST_H

#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task1_that_runs_everytime(void);

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task2_started_in_the_meantime(void);

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task3_with_max_interval(void);

#endif
