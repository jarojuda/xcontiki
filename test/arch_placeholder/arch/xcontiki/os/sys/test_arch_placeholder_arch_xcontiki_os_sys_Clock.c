#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "arch/xcontiki/os/sys/arch_xcontiki_os_sys_Clock.h"
#include "mock_arch_xcontiki_os_dev_Watchdog.h"

static long watchdog_calls_counter;
static int number_of_calls_indicating_an_infinite_loop;

void watchdog_periodic_fake(void){
        watchdog_calls_counter++;
        TEST_ASSERT_LESS_THAN(number_of_calls_indicating_an_infinite_loop, watchdog_calls_counter);
}


void setUp(void){
        arch_xcontiki_os_sys_Clock__init();
        watchdog_calls_counter=0;
}

void tearDown(void){

}

void test_whenMaxIntervalIsSet_thenClockWaitShouldNotLoopForever(void){

        arch_xcontiki_os_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;
        number_of_calls_indicating_an_infinite_loop = ARCH_XCONTIKI_OS_SYS_CLOCK__MAX/ARCH_XCONTIKI_OS_SYS_CLOCK__DUMMY_CLOCK_INCREMENT +1;
        //Only do this on a fast PC
        arch_xcontiki_os_sys_Clock__wait(ARCH_XCONTIKI_OS_SYS_CLOCK__MAX);

        printf("%ld\n",watchdog_calls_counter);

}
