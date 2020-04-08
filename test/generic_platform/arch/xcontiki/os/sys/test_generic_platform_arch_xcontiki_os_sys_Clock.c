#include "unity.h"
#include "xcontiki/xcontiki.h"
#include <string.h>
#include "arch/xcontiki/os/sys/arch_xcontiki_os_sys_Clock.h"
#include "mock_arch_xcontiki_os_dev_Watchdog.h"

static int watchdog_calls_counter;
static int number_of_calls_indicating_an_infinite_loop;

void watchdog_periodic_fake(void){
        watchdog_calls_counter++;
        TEST_ASSERT_LESS_THAN(number_of_calls_indicating_an_infinite_loop, watchdog_calls_counter);
}


static int time_points_index;
#define NUMBER_OF_TIME_POINTS 10
static arch_xcontiki_os_sys_Clock__time_t const * time_points_mocked;

void setUp(void){
        arch_xcontiki_os_sys_Clock__init();
        watchdog_calls_counter=0;
        time_points_index=0;
}

void tearDown(void){
}

arch_xcontiki_os_sys_Clock__time_t arch_xcontiki_os_sys_Clock__time(void){
        if(time_points_index>=NUMBER_OF_TIME_POINTS) {
                time_points_index=0;
        }
        return time_points_mocked[time_points_index++];
}

void test_whenMaxIntervalIsSet_thenClockWaitShouldNotLoopForever(void){
        static const arch_xcontiki_os_sys_Clock__time_t time_points[NUMBER_OF_TIME_POINTS]={0,0,1,1,ARCH_XCONTIKI_OS_SYS_CLOCK__MAX-1,0,0,0,0,0};
        time_points_mocked=time_points;

        arch_xcontiki_os_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;
        number_of_calls_indicating_an_infinite_loop=10;

        arch_xcontiki_os_sys_Clock__wait(ARCH_XCONTIKI_OS_SYS_CLOCK__MAX);

}

void test_whenZeroIntervalIsSet_thenClockWaitShouldReturnImmediately(void){

}
