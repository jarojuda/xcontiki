#include "unity.h"
#include "xcontiki/arch/xcontiki_arch_Clock.h"
#include "mock_xcontiki_arch_dev_Watchdog.h"
#include "mock_get_timer_for_test.h"

static int watchdog_calls_counter;
static int number_of_watchdog_calls_indicating_an_infinite_loop;

static void watchdog_periodic_fake(void){
  printf("Watchdog");
        watchdog_calls_counter++;
        TEST_ASSERT_LESS_THAN(number_of_watchdog_calls_indicating_an_infinite_loop, watchdog_calls_counter);
}

void setUp(void){
       xcontiki_arch_Clock__init();
        watchdog_calls_counter=0;
}

void tearDown(void){
}


void test_whenMaxIntervalIsSet_thenClockWaitShouldNotLoopForever(void){

        number_of_watchdog_calls_indicating_an_infinite_loop = 5;
        xcontiki_arch_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;

        uint32_t time_points_1[3]={
                0,
                XCONTIKI_ARCH_CLOCK__MAX-1,
                0
        };
        SET_RETURN_SEQ(get_timer, time_points_1, 3);
        xcontiki_arch_Clock__wait(XCONTIKI_ARCH_CLOCK__MAX);

        RESET_FAKE(get_timer);
        uint32_t time_points_2[3]={
                XCONTIKI_ARCH_CLOCK__MAX/2+1,
                XCONTIKI_ARCH_CLOCK__MAX/2,
                XCONTIKI_ARCH_CLOCK__MAX/2+1
        };
        SET_RETURN_SEQ(get_timer, time_points_2, 3);
        xcontiki_arch_Clock__wait(XCONTIKI_ARCH_CLOCK__MAX);
}

void test_whenZeroIntervalIsSet_thenClockWaitShouldReturnImmediately(void){

        number_of_watchdog_calls_indicating_an_infinite_loop = 1;
        xcontiki_arch_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;

        uint32_t time_points_1[4]={0, 0, 0, 1};
        SET_RETURN_SEQ(get_timer, time_points_1, 4);
        xcontiki_arch_Clock__wait(0);
        TEST_ASSERT_NOT_CALLED(get_timer);
        TEST_ASSERT_NOT_CALLED(xcontiki_arch_dev_Watchdog__periodic);


        RESET_FAKE(get_timer);

        uint32_t time_points_2[4]={
                XCONTIKI_ARCH_CLOCK__MAX/2-1,
                XCONTIKI_ARCH_CLOCK__MAX/2-1,
                XCONTIKI_ARCH_CLOCK__MAX/2,
                XCONTIKI_ARCH_CLOCK__MAX/2
        };
        SET_RETURN_SEQ(get_timer, time_points_2, 4);
        xcontiki_arch_Clock__wait(0);
        TEST_ASSERT_NOT_CALLED(get_timer);
        TEST_ASSERT_NOT_CALLED(xcontiki_arch_dev_Watchdog__periodic);
}





void test_ClockWait(void){
        number_of_watchdog_calls_indicating_an_infinite_loop = 5;
        xcontiki_arch_dev_Watchdog__periodic_fake.custom_fake = watchdog_periodic_fake;

        uint32_t time_points_1[5]={0, 0, 1, 2, 3};
        SET_RETURN_SEQ(get_timer, time_points_1, 5);
        xcontiki_arch_Clock__wait(2);
        TEST_ASSERT_CALLED_TIMES(1, xcontiki_arch_dev_Watchdog__periodic);

        RESET_FAKE(get_timer);

        uint32_t time_points_2[5]={XCONTIKI_ARCH_CLOCK__MAX/2+1, XCONTIKI_ARCH_CLOCK__MAX/2+1,0,0,1};
        SET_RETURN_SEQ(get_timer, time_points_2, 5);
        xcontiki_arch_Clock__wait(XCONTIKI_ARCH_CLOCK__MAX/2);
        TEST_ASSERT_CALLED_TIMES(1, xcontiki_arch_dev_Watchdog__periodic);
}
