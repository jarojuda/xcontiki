#include "unity.h"
#define XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS (3)
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Process.h"
#include "mock_processes_for_test.h"


XCONTIKI_OS_SYS_PROCESS__NAME(process_1);
XCONTIKI_OS_SYS_PROCESS__NAME(process_2);
XCONTIKI_OS_SYS_PROCESS__NAME(process_3);

XCONTIKI_OS_SYS_PROCESS(process_1, "Process 1");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_1, ev, data){

        XCONTIKI_OS_SYS_PROCESS__POLLHANDLER( xcontiki_os_sys_Process__exit(&process_2) );
        XCONTIKI_OS_SYS_PROCESS__EXITHANDLER( XCONTIKI_OS_SYS_PROCESS__EXIT() );

        XCONTIKI_OS_SYS_PROCESS__BEGIN();
        return process_1_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}

XCONTIKI_OS_SYS_PROCESS(process_2, "Process 2");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_2, ev, data){

        XCONTIKI_OS_SYS_PROCESS__POLLHANDLER(process_2_pollhandler());
        XCONTIKI_OS_SYS_PROCESS__EXITHANDLER( XCONTIKI_OS_SYS_PROCESS__EXIT() );

        XCONTIKI_OS_SYS_PROCESS__BEGIN();
        return process_2_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}

XCONTIKI_OS_SYS_PROCESS(process_3, "Process 3");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_3, ev, data){

        XCONTIKI_OS_SYS_PROCESS__POLLHANDLER(process_3_pollhandler());
        XCONTIKI_OS_SYS_PROCESS__EXITHANDLER( XCONTIKI_OS_SYS_PROCESS__EXIT() );

        XCONTIKI_OS_SYS_PROCESS__BEGIN();
        return process_3_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}

void setUp(void){
        xcontiki_os_sys_Process__initialize_the_process_module();
        process_1_internals_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        process_2_internals_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        process_3_internals_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        xcontiki_os_sys_Process__start(&process_1, NULL);
        xcontiki_os_sys_Process__start(&process_2, NULL);
        xcontiki_os_sys_Process__start(&process_3, NULL);
}

void tearDown(void){

}


void test_process_start(void){

        TEST_ASSERT_CALLED(process_1_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, process_1_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL, process_1_internals_fake.arg1_val);

        TEST_ASSERT_CALLED(process_2_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, process_2_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL, process_2_internals_fake.arg1_val);

        TEST_ASSERT_CALLED(process_3_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, process_3_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL, process_3_internals_fake.arg1_val);

        TEST_ASSERT(xcontiki_os_sys_Process__is_running(&process_1));
        TEST_ASSERT(xcontiki_os_sys_Process__is_running(&process_2));
        TEST_ASSERT(xcontiki_os_sys_Process__is_running(&process_3));

        //Check the list of processes
        int process_1_counter=0;
        int process_2_counter=0;
        int process_3_counter=0;
        struct xcontiki_os_sys_Process *q;
        for(q = &process_3; q != NULL; q = q->next) {
                if( &process_1==q) {
                        process_1_counter++;
                } else if( &process_2==q) {
                        process_2_counter++;
                } else if( &process_3==q) {
                        process_3_counter++;
                } else{
                        TEST_FAIL_MESSAGE("Unknown process in the list of processes");
                }
        }
        TEST_ASSERT_EQUAL(1, process_1_counter);
        TEST_ASSERT_EQUAL(1, process_2_counter);
        TEST_ASSERT_EQUAL(1, process_3_counter);

        //Start next time. No effect is expected.
        xcontiki_os_sys_Process__start(&process_1, NULL);
        xcontiki_os_sys_Process__start(&process_2, NULL);
        xcontiki_os_sys_Process__start(&process_3, NULL);
        TEST_ASSERT_CALLED_TIMES(1, process_1_internals);
        TEST_ASSERT_CALLED_TIMES(1, process_2_internals);
        TEST_ASSERT_CALLED_TIMES(1, process_3_internals);
}

void test_process_exit(void){
        //The first process tries to exit the process_2
        xcontiki_os_sys_Process__poll( &process_1);
        xcontiki_os_sys_Process__process_next_event();

        //Process_2 stopped
        TEST_ASSERT_FALSE(xcontiki_os_sys_Process__is_running(&process_2));
        //Process_1 and process_3 still running.
        TEST_ASSERT_TRUE (xcontiki_os_sys_Process__is_running(&process_1));
        TEST_ASSERT_TRUE (xcontiki_os_sys_Process__is_running(&process_3));

        //Check the list of processes
        int process_1_counter=0;
        int process_2_counter=0;
        int process_3_counter=0;
        struct xcontiki_os_sys_Process *q;
        for(q = &process_3; q != NULL; q = q->next) {
                if( &process_1==q) {
                        process_1_counter++;
                } else if( &process_2==q) {
                        process_2_counter++;
                } else if( &process_3==q) {
                        process_3_counter++;
                } else{
                        TEST_FAIL_MESSAGE("Unknown process in the list of processes");
                }
        }
        //Process_2 should be removed from the list
        TEST_ASSERT_EQUAL(0, process_2_counter);
        TEST_ASSERT_EQUAL(1, process_1_counter);
        TEST_ASSERT_EQUAL(1, process_3_counter);

        //Process_3 should receive the EXITED EVENT and process_2 as the data
        TEST_ASSERT_CALLED_TIMES(2, process_3_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_EXITED, process_3_internals_fake.arg0_history[1]);
        TEST_ASSERT_EQUAL(&process_2, process_3_internals_fake.arg1_history[1]);

}

void test_process_post_broadcast_event(void){
        const char* msg="Message";
        xcontiki_os_sys_Process__post_event_via_queue(XCONTIKI_OS_SYS_PROCESS__BROADCAST, XCONTIKI_OS_SYS_PROCESS__EVENT_MSG, (void*)msg);
        xcontiki_os_sys_Process__process_next_event();
        TEST_ASSERT_CALLED_TIMES(2, process_1_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_MSG, process_1_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(msg, process_1_internals_fake.arg1_val);

        TEST_ASSERT_CALLED_TIMES(2, process_2_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_MSG, process_2_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(msg, process_2_internals_fake.arg1_val);

        TEST_ASSERT_CALLED_TIMES(2, process_3_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_MSG, process_3_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(msg, process_3_internals_fake.arg1_val);

}
