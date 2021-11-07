#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Etimer.h"
#include "mock_xcontiki_os_sys_Timer.h"
#include "mock_xcontiki_os_sys_Process.h"


void setUp(void){
        int i;

        for(i=1; i<XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; i++) {
                xcontiki_os_sys_Etimer__remove(i);
        }
}

void tearDown(void){

}

XCONTIKI_OS_SYS_ETIMER__NEW_STATIC(event_timer_for_test);

void test_when_EtimerIsSetFirstTime_thenShouldAllocateNonZeroIndex(void){

        TEST_ASSERT_EQUAL(0, event_timer_for_test.id());
        event_timer_for_test.set(XCONTIKI_ARCH_CLOCK__SECOND);
        TEST_ASSERT_NOT_EQUAL(0, event_timer_for_test.id());

}

void test_when_EtimerIsAllocated_thenItShouldNotChangeIndex(void){

        static xcontiki_os_sys_Etimer__etimer_id_t etimer_id;

        event_timer_for_test.set(XCONTIKI_ARCH_CLOCK__SECOND);
        etimer_id = event_timer_for_test.id();
        TEST_ASSERT_NOT_EQUAL(0, etimer_id);
        event_timer_for_test.set(XCONTIKI_ARCH_CLOCK__SECOND);
        TEST_ASSERT_EQUAL(etimer_id, event_timer_for_test.id());

}

void test_when_EtimerIsTryingAllocate_thenItShouldNotAllocateMoreTimersThanIsAvailableSpace(void){
        int i;
        xcontiki_os_sys_Etimer__etimer_id_t etimer_id;

        for(i=1; i<XCONTIKI_OS_SYS_ETIMER__CONF_ETIMERS_NUMBER; i++) {
                etimer_id = xcontiki_os_sys_Etimer__set(0, XCONTIKI_ARCH_CLOCK__SECOND);
                TEST_ASSERT_NOT_EQUAL(0, etimer_id);
        }
        etimer_id = xcontiki_os_sys_Etimer__set(0, XCONTIKI_ARCH_CLOCK__SECOND);
        TEST_ASSERT_EQUAL(0, etimer_id);
}

XCONTIKI_OS_SYS_PROCESS__NAME(process_1);
XCONTIKI_OS_SYS_PROCESS(process_1, "Process 1");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_1, ev, data){

        XCONTIKI_OS_SYS_PROCESS__BEGIN();

        event_timer_for_test.set(XCONTIKI_ARCH_CLOCK__SECOND);
        while(1){
            XCONTIKI_OS_SYS_PROCESS__WAIT_EVENT();
        }

        XCONTIKI_OS_SYS_PROCESS__END();
}

void test_when_EtimerExpired_thenItShouldGenerateEvent(void){

        xcontiki_os_sys_Process__get_current_process_fake.return_val = &process_1;
        event_timer_for_test.set(XCONTIKI_ARCH_CLOCK__SECOND);

        xcontiki_os_sys_Timer__expired_fake.return_val = false;
        xcontiki_os_sys_Etimer__process.thread(XCONTIKI_OS_SYS_PROCESS__EVENT_POLL, 0);
        xcontiki_os_sys_Etimer__process.thread(XCONTIKI_OS_SYS_PROCESS__EVENT_POLL, 0);
        TEST_ASSERT_NOT_CALLED(xcontiki_os_sys_Process__post_event_via_queue);
        xcontiki_os_sys_Timer__expired_fake.return_val = true;
        xcontiki_os_sys_Etimer__process.thread(XCONTIKI_OS_SYS_PROCESS__EVENT_POLL, 0);

        TEST_ASSERT_CALLED(xcontiki_os_sys_Process__post_event_via_queue);
        TEST_ASSERT_EQUAL(&process_1, xcontiki_os_sys_Process__post_event_via_queue_fake.arg0_val);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_TIMER, xcontiki_os_sys_Process__post_event_via_queue_fake.arg1_val);
        TEST_ASSERT_EQUAL(event_timer_for_test.id(), xcontiki_os_sys_Process__post_event_via_queue_fake.arg2_val);

}

void test_when_ProcessIsExiting_then_EtimerShouldBeRemoved(void){

        RESET_FAKE(xcontiki_os_sys_Timer__remove);
        xcontiki_os_sys_Process__get_current_process_fake.return_val = &process_1;
        event_timer_for_test.set(XCONTIKI_ARCH_CLOCK__SECOND);
        xcontiki_os_sys_Etimer__process.thread(XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED, (xcontiki_os_sys_Process__data_t)&process_1);
        TEST_ASSERT_CALLED(xcontiki_os_sys_Timer__remove);
        TEST_ASSERT_FALSE(event_timer_for_test.is_allocated());
        
}
