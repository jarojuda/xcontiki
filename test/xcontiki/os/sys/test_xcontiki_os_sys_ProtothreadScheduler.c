#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>
#include "mock_xcontiki_arch_Clock.h"
#include "mock_tasks_for_test.h"


#define XCONTIKI_OS_SYS_PROTOTHREADSCHEDULER__TASK_LIST \
        XCONTIKI_OS_SYS_PROTOTHREADSCHEDULER__TASK(task1_that_runs_everytime, 0) \
        XCONTIKI_OS_SYS_PROTOTHREADSCHEDULER__TASK(task2_started_in_the_meantime, XCONTIKI_ARCH_CLOCK__MAX/2ull) \
        XCONTIKI_OS_SYS_PROTOTHREADSCHEDULER__TASK(task3_with_max_interval, XCONTIKI_ARCH_CLOCK__MAX) \


#include "xcontiki/os/sys/xcontiki_os_sys_ProtothreadScheduler.c"


void setUp(void){
        scheduler__init();
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

        scheduler();
        TEST_ASSERT_CALLED(task1_that_runs_everytime);
        TEST_ASSERT_CALLED(task2_started_in_the_meantime);
        TEST_ASSERT_CALLED(task3_with_max_interval);
}

void test_whenAfterReset_thenEveryTaskShouldBeStarted(void){

        xcontiki_arch_Clock__time_fake.return_val = 0;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        scheduler__init();
        xcontiki_arch_Clock__time_fake.return_val = XCONTIKI_ARCH_CLOCK__MAX/2ull - 1ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        scheduler__init();
        xcontiki_arch_Clock__time_fake.return_val = XCONTIKI_ARCH_CLOCK__MAX/2ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        scheduler__init();
        xcontiki_arch_Clock__time_fake.return_val = XCONTIKI_ARCH_CLOCK__MAX/2ull +1ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);

        scheduler__init();
        xcontiki_arch_Clock__time_fake.return_val = XCONTIKI_ARCH_CLOCK__MAX;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED);
}


void test_whenTaskWasStarted_andItIsWaiting_thenTaskShouldRunImmediately(void){

        //after the first start
        //tasks are in progress, have not finished
        xcontiki_arch_Clock__time_fake.return_val =0ull;
        first_run_with_fake_tasks_returning_status(XCONTIKI_OS_SYS_PROTOTHREAD__WAITING);


        //time forward by one tick
        xcontiki_arch_Clock__time_fake.return_val +=1ull;
        scheduler();
        //all tasks should start again regardless of the interval
        TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);
}

void test_whenTaskWasStarted_andItIsYielded_thenTaskShouldRunImmediately(void){

        //after the first start
        //tasks are in progress, have not finished
        xcontiki_arch_Clock__time_fake.return_val =0ull;
        task1_that_runs_everytime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        task2_started_in_the_meantime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        task3_with_max_interval_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        scheduler();

        //time forward by one tick
        xcontiki_arch_Clock__time_fake.return_val +=1ull;
        scheduler();
        //all tasks should start again regardless of the interval
        TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);

}

void test_whenTaskWasStarted_andItIsEnded_thenTaskShouldRunAfterInterval(void){

        //after the first start
        //tasks are in progress, have not finished
        xcontiki_arch_Clock__time_fake.return_val =0ull;
        task1_that_runs_everytime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
        task2_started_in_the_meantime_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
        task3_with_max_interval_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__ENDED;
        scheduler();


        //time forward by half of max interval - one tick
        xcontiki_arch_Clock__time_fake.return_val +=(XCONTIKI_ARCH_CLOCK__MAX/2ull -1);
        scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task2_started_in_the_meantime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);

        //time forward by one tick
        xcontiki_arch_Clock__time_fake.return_val += 1;
        scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(3, task1_that_runs_everytime);
        //this task should run
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);

        //time forward by another tick
        xcontiki_arch_Clock__time_fake.return_val += 1;
        scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(4, task1_that_runs_everytime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(2, task2_started_in_the_meantime);
        //this task should wait
        TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);


        //time rewound to zero
        xcontiki_arch_Clock__time_fake.return_val = 0;
        scheduler();
        //this task should always run
        TEST_ASSERT_CALLED_TIMES(5, task1_that_runs_everytime);
        //this task should run
        TEST_ASSERT_CALLED_TIMES(3, task2_started_in_the_meantime);
        //this task should run
        TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);
}
