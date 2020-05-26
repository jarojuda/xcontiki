#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>
#include "mock_arch_xcontiki_os_sys_Clock.h"
#include "mock_tasks_for_test.h"


#define TEST_TASK_LIST \
        XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task1_that_runs_everytime, 0) \
        XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task2_started_in_the_meantime, ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2ull) \
        XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task3_with_max_interval, ARCH_XCONTIKI_OS_SYS_CLOCK__MAX) \


XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER(tst_scheduler, TEST_TASKS_LIST);


void setUp(void){
        tst_scheduler__init();
}

void tearDown(void){
}

void reset_all_fakes(void){
        FFF_RESET_HISTORY();
        RESET_FAKE(task1_that_runs_everytime);
        RESET_FAKE(task2_started_in_the_meantime);
        RESET_FAKE(task3_with_max_interval);
}


void static first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__THREAD status){
        reset_all_fakes();
        task1_that_runs_everytime_fake.return_val = status;
        task2_started_in_the_meantime_fake.return_val = status;
        task3_with_max_interval_fake.return_val = status;

        tst_scheduler();
        TEST_ASSERT_CALLED(task1_that_runs_everytime);
        TEST_ASSERT_CALLED(task2_started_in_the_meantime);
        TEST_ASSERT_CALLED(task3_with_max_interval);
}

void test_whenAfterReset_thenEveryTaskShouldBeStarted(void){

        arch_xcontiki_os_sys_Clock__time_fake.return_val = 0;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        tst_scheduler__init();
        arch_xcontiki_os_sys_Clock__time_fake.return_val = ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2ull - 1ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        tst_scheduler__init();
        arch_xcontiki_os_sys_Clock__time_fake.return_val = ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        tst_scheduler__init();
        arch_xcontiki_os_sys_Clock__time_fake.return_val = ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2ull +1ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        tst_scheduler__init();
        arch_xcontiki_os_sys_Clock__time_fake.return_val = ARCH_XCONTIKI_OS_SYS_CLOCK__MAX;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);
}


void test_whenTaskWasStarted_andItIsWaiting_thenTaskShouldRunImmediately(void){

        //after the first start
        //tasks are in progress, have not finished
        arch_xcontiki_os_sys_Clock__time_fake.return_val =0ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__WAITING);


        //time forward by one tick
        arch_xcontiki_os_sys_Clock__time_fake.return_val +=1ull;
        tst_scheduler();
        //all tasks should start again regardless of the interval
        TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);
}

void test_whenTaskWasStarted_andItIsYielded_thenTaskShouldRunImmediately(void){

        //after the first start
        //tasks are in progress, have not finished
        arch_xcontiki_os_sys_Clock__time_fake.return_val =0ull;
        task1_that_runs_everytime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        task2_started_in_the_meantime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        task3_with_max_interval_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        tst_scheduler();

        //time forward by one tick
        arch_xcontiki_os_sys_Clock__time_fake.return_val +=1ull;
        tst_scheduler();
        //all tasks should start again regardless of the interval
        TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);

}

void test_whenTaskWasStarted_andItIsEnded_thenTaskShouldRunAfterInterval(void){

        //after the first start
        //tasks are in progress, have not finished
        arch_xcontiki_os_sys_Clock__time_fake.return_val =0ull;
        task1_that_runs_everytime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
        task2_started_in_the_meantime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
        task3_with_max_interval_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
        tst_scheduler();


        //time forward by half of max interval - one tick
        arch_xcontiki_os_sys_Clock__time_fake.return_val +=(ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2ull -1);
        tst_scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task2_started_in_the_meantime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);

        //time forward by one tick
        arch_xcontiki_os_sys_Clock__time_fake.return_val += 1;
        tst_scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(3, task1_that_runs_everytime);
        //this task should run
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);

        //time forward by another tick
        arch_xcontiki_os_sys_Clock__time_fake.return_val += 1;
        tst_scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(4, task1_that_runs_everytime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);


        //time rewound to zero
        arch_xcontiki_os_sys_Clock__time_fake.return_val = 0;
        tst_scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(5, task1_that_runs_everytime);
        //this task should run
        TEST_ASSERT_CALLED_TIMES(3, task2_started_in_the_meantime);
        //this task should run
        TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);
}
