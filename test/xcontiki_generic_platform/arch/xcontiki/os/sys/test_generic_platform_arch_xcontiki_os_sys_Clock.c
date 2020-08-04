#include "unity.h"
#include "xcontiki/xcontiki.h"
#include <string.h>
#include <stdbool.h>
#include "arch/xcontiki/os/sys/arch_xcontiki_os_sys_Clock.h"
#include "mock_arch_xcontiki_os_dev_Watchdog.h"
#include "mock_arch_dev_HardwareClock.h"

static int watchdog_calls_counter;
static int number_of_watchdog_calls_indicating_an_infinite_loop;

static void watchdog_periodic_fake(void){
        watchdog_calls_counter++;
        TEST_ASSERT_LESS_THAN(number_of_watchdog_calls_indicating_an_infinite_loop, watchdog_calls_counter);
}

void setUp(void){
        arch_xcontiki_os_sys_Clock__init();
        watchdog_calls_counter=0;
}

void tearDown(void){
}


void test_whenMaxIntervalIsSet_thenClockWaitShouldNotLoopForever(void){

        number_of_watchdog_calls_indicating_an_infinite_loop = 5;
        arch_xcontiki_os_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;

        uint32_t time_points_1[3]={0, ARCH_XCONTIKI_OS_SYS_CLOCK__MAX-1, 0};
        SET_RETURN_SEQ(arch_dev_HardwareClock__get_clock, time_points_1, 3);
        arch_xcontiki_os_sys_Clock__wait(ARCH_XCONTIKI_OS_SYS_CLOCK__MAX);

        RESET_FAKE(arch_dev_HardwareClock__get_clock);
        uint32_t time_points_2[3]={
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2+1,
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2,
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2+1
        };

        SET_RETURN_SEQ(arch_dev_HardwareClock__get_clock, time_points_2, 3);
        arch_xcontiki_os_sys_Clock__wait(ARCH_XCONTIKI_OS_SYS_CLOCK__MAX);
}

void test_whenZeroIntervalIsSet_thenClockWaitShouldReturnImmediately(void){

        number_of_watchdog_calls_indicating_an_infinite_loop = 1;
        arch_xcontiki_os_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;

        uint32_t time_points_1[4]={0, 0, 0, 1};
        SET_RETURN_SEQ(arch_dev_HardwareClock__get_clock, time_points_1, 4);
        arch_xcontiki_os_sys_Clock__wait(0);
        TEST_ASSERT_NOT_CALLED( arch_dev_HardwareClock__get_clock);
        TEST_ASSERT_NOT_CALLED(arch_xcontiki_os_dev_Watchdog__periodic);


        RESET_FAKE(arch_dev_HardwareClock__get_clock);

        uint32_t time_points_2[4]={
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2-1,
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2-1,
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2,
                ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2
        };
        SET_RETURN_SEQ(arch_dev_HardwareClock__get_clock, time_points_2, 4);
        arch_xcontiki_os_sys_Clock__wait(0);
        TEST_ASSERT_NOT_CALLED(arch_dev_HardwareClock__get_clock);
        TEST_ASSERT_NOT_CALLED(arch_xcontiki_os_dev_Watchdog__periodic);
}





void test_ClockWait(void){
        number_of_watchdog_calls_indicating_an_infinite_loop = 5;
        arch_xcontiki_os_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;

        uint32_t time_points_1[5]={0, 0, 1, 2, 3};
        SET_RETURN_SEQ(arch_dev_HardwareClock__get_clock, time_points_1, 5);
        arch_xcontiki_os_sys_Clock__wait(2);
        TEST_ASSERT_CALLED_TIMES(1, arch_xcontiki_os_dev_Watchdog__periodic);

        RESET_FAKE(arch_dev_HardwareClock__get_clock);

        uint32_t time_points_2[5]={ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2+1, ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2+1,0,0,1};
        SET_RETURN_SEQ(arch_dev_HardwareClock__get_clock, time_points_2, 5);
        arch_xcontiki_os_sys_Clock__wait(ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/2);
        TEST_ASSERT_CALLED_TIMES(1, arch_xcontiki_os_dev_Watchdog__periodic);
}
