#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Timer.h"
#include "mock_xcontiki_arch_Clock.h"


void setUp(void){

}

void tearDown(void){

}

XCONTIKI_OS_SYS_TIMER__NEW_STATIC(tmr);

void test_whenIntervalIsSet_thenTheTimerShouldExpireAfterTheSetTime(void){

        xcontiki_arch_Clock__time_t time_points_1[4]=
        {0, 0, 1, 1};
        SET_RETURN_SEQ(xcontiki_arch_Clock__time, time_points_1, 4);
        tmr.set( 1);
        TEST_ASSERT_FALSE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());

        RESET_FAKE(xcontiki_arch_Clock__time);
        xcontiki_arch_Clock__time_t time_points_2[4]=
        {XCONTIKI_ARCH_CLOCK__MAX/2,
         XCONTIKI_ARCH_CLOCK__MAX/2,
         XCONTIKI_ARCH_CLOCK__MAX/2+1,
         XCONTIKI_ARCH_CLOCK__MAX/2+1};
        SET_RETURN_SEQ(xcontiki_arch_Clock__time, time_points_2, 4);
        tmr.set( 1);
        TEST_ASSERT_FALSE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());

}

void test_whenZeroIntervalIsSet_thenTheTimerShouldExpireImmediately(void){

        xcontiki_arch_Clock__time_t time_points_1[3]=
        {0, 0, 1};
        SET_RETURN_SEQ(xcontiki_arch_Clock__time, time_points_1, 3);
        tmr.set(0);
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());

}


void test_whenMaxIntervalIsSet_thenTheTimerShouldExpireAfterTheSetTime(void){

        xcontiki_arch_Clock__time_t time_points_1[6]=
        {0, 0, 1, 1, 0, 0};
        SET_RETURN_SEQ(xcontiki_arch_Clock__time, time_points_1, 6);
        tmr.set( XCONTIKI_ARCH_CLOCK__MAX );
        TEST_ASSERT_FALSE(tmr.expired());
        TEST_ASSERT_FALSE(tmr.expired());
        TEST_ASSERT_FALSE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());

}

void test_whenTheTimerHasExpired_thenTheTimerShouldRemainExpired(void){

        xcontiki_arch_Clock__time_t time_points_1[10]=
        {0, 1, 2, 0, 1, 2, 3};
        SET_RETURN_SEQ(xcontiki_arch_Clock__time, time_points_1, 6);
        tmr.set( 2 );
        TEST_ASSERT_FALSE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());
        TEST_ASSERT_TRUE(tmr.expired());

}
