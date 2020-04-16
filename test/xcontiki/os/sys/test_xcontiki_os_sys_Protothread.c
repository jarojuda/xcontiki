#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>
#include "mock_arch_xcontiki_os_sys_Clock.h"
#include "mock_tasks_for_test.h"


#define TEST_TASKS_LIST\
  XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task1_that_runs_everytime, 0)\
  XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task2_started_every_second, ARCH_XCONTIKI_OS_SYS_CLOCK__SECOND)\
  XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task3_with_max_interval, ARCH_XCONTIKI_OS_SYS_CLOCK__MAX)\
  XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULER_TASK(task4_started_every_100milisecond, ARCH_XCONTIKI_OS_SYS_CLOCK__SECOND/10ull)\


XCONTIKI_OS_SYS_PROTOTHREAD__SCHEDULE(test_scheduler, TEST_TASKS_LIST);




void setUp(void){
	test_scheduler__init();
}

void tearDown(void){
}

void reset_all_fakes(void){
	FFF_RESET_HISTORY();
	RESET_FAKE(task1_that_runs_everytime);
	RESET_FAKE(task2_started_every_second);
	RESET_FAKE(task3_with_max_interval);
	RESET_FAKE(task4_started_every_100milisecond);
}


void static first_run_with_fake_tasks_returning_status(PT_THREAD status){
	reset_all_fakes();
	task1_that_runs_everytime_fake.return_val = status;
	task2_started_every_second_fake.return_val = status;
	task3_with_max_interval_fake.return_val = status;
	task4_started_every_100milisecond_fake.return_val = status;
	test_scheduler();
	TEST_ASSERT_CALLED(task1_that_runs_everytime);
	TEST_ASSERT_CALLED(task2_started_every_second);
	TEST_ASSERT_CALLED(task3_with_max_interval);
	TEST_ASSERT_CALLED(task4_started_every_100milisecond);


}

void test_whenAfterReset_thenEveryTaskShouldBeStarted(void){

clock_time_fake.return_val = 0;
first_run_with_fake_tasks_returning_status(PT_ENDED);

test_scheduler__init();
clock_time_fake.return_val = CLOCK_TIME_MAX/2ull - 1ull;
first_run_with_fake_tasks_returning_status(PT_ENDED);

test_scheduler__init();
clock_time_fake.return_val = CLOCK_TIME_MAX/2ull ;
first_run_with_fake_tasks_returning_status(PT_ENDED);

test_scheduler__init();
clock_time_fake.return_val = CLOCK_TIME_MAX/2ull +1ull;
first_run_with_fake_tasks_returning_status(PT_ENDED);

test_scheduler__init();
clock_time_fake.return_val = CLOCK_TIME_MAX ;
first_run_with_fake_tasks_returning_status(PT_ENDED);


}


void test_whenTaskWasStarted_andItIsWaiting_thenTaskShouldRunImmediately(void){


//po pierwszym uruchomieniu
//zadania są w trakcie, nie kończą się
clock_time_fake.return_val =0ull;
first_run_with_fake_tasks_returning_status(PT_WAITING);


//czas do przodu tylko o jeden tick
clock_time_fake.return_val +=1ull;

test_scheduler();
//wszystkie zadania powinny się uruchomić kolejny raz niezależnie od interwału

TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
TEST_ASSERT_CALLED_TIMES(2, task2_started_every_second);
TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);
TEST_ASSERT_CALLED_TIMES(2, task4_started_every_100milisecond);

}

void test_whenTaskWasStarted_andItIsYielded_thenTaskShouldRunImmediately(void){


//po pierwszym uruchomieniu
//zadania są w trakcie, nie kończą się
clock_time_fake.return_val =0ull;
task1_that_runs_everytime_fake.return_val = PT_YIELDED;
task2_started_every_second_fake.return_val = PT_YIELDED;
task3_with_max_interval_fake.return_val = PT_YIELDED;
task4_started_every_100milisecond_fake.return_val = PT_YIELDED;
test_scheduler();


//czas do przodu tylko o jeden tick
clock_time_fake.return_val +=1ull;

test_scheduler();
//wszystkie zadania powinny się uruchomić kolejny raz niezależnie od interwału

TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);
TEST_ASSERT_CALLED_TIMES(2, task2_started_every_second);
TEST_ASSERT_CALLED_TIMES(2, task3_with_max_interval);
TEST_ASSERT_CALLED_TIMES(2, task4_started_every_100milisecond);

}

void test_whenTaskWasStarted_andItIsEnded_thenTaskShouldRunAfterInterval(void){


//po pierwszym uruchomieniu
//zadania są w trakcie, nie kończą się
clock_time_fake.return_val =0ull;
task1_that_runs_everytime_fake.return_val = PT_ENDED;
task2_started_every_second_fake.return_val = PT_ENDED;
task3_with_max_interval_fake.return_val = PT_ENDED;
task4_started_every_100milisecond_fake.return_val = PT_ENDED;
test_scheduler();


//czas do przodu o 100 milisekund +jeden tick
clock_time_fake.return_val +=(CLOCK_SECOND/10ull+1ull);

test_scheduler();
//to zadanie zawsze się powinno uruchomić
TEST_ASSERT_CALLED_TIMES(2, task1_that_runs_everytime);

//to nie , musi jeszcze poczekać
TEST_ASSERT_CALLED_TIMES(1, task2_started_every_second);

//to nie
TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);

//na to zadanie przyszła pora
TEST_ASSERT_CALLED_TIMES(2, task4_started_every_100milisecond);



//czas na 1 sekundę od startu +jeden tick
clock_time_fake.return_val =(CLOCK_SECOND+1ull);

test_scheduler();

//to zadanie zawsze się powinno uruchomić
TEST_ASSERT_CALLED_TIMES(3, task1_that_runs_everytime);

//to ma się uruchomić
TEST_ASSERT_CALLED_TIMES(2, task2_started_every_second);

//to nie
TEST_ASSERT_CALLED_TIMES(1, task3_with_max_interval);

//to ma się uruchomić
TEST_ASSERT_CALLED_TIMES(3, task4_started_every_100milisecond);



}
