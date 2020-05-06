#include "unity.h"
#define XCONTIKI_OS_SYS_PROCESS__CONF_NUMEVENTS (3)
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Process.h"
#include "mock_processes_for_test.h"


void setUp(void){
        xcontiki_os_sys_Process__init();
}

void tearDown(void){

}


XCONTIKI_OS_SYS_PROCESS(process_1, "Process 1");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_1, ev, data){

        XCONTIKI_OS_SYS_PROCESS__POLLHANDLER(process_1_pollhandler());
        XCONTIKI_OS_SYS_PROCESS__EXITHANDLER(process_1_exithandler());

        XCONTIKI_OS_SYS_PROCESS__BEGIN();
        return process_1_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}

XCONTIKI_OS_SYS_PROCESS(process_2, "Process 2");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_2, ev, data){

        XCONTIKI_OS_SYS_PROCESS__POLLHANDLER(process_2_pollhandler());
        XCONTIKI_OS_SYS_PROCESS__EXITHANDLER(process_2_exithandler());

        XCONTIKI_OS_SYS_PROCESS__BEGIN();
        return process_2_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}

XCONTIKI_OS_SYS_PROCESS(process_3, "Process 3");
XCONTIKI_OS_SYS_PROCESS__THREAD(process_3, ev, data){

        XCONTIKI_OS_SYS_PROCESS__POLLHANDLER(process_3_pollhandler());
        XCONTIKI_OS_SYS_PROCESS__EXITHANDLER(process_3_exithandler());

        XCONTIKI_OS_SYS_PROCESS__BEGIN();
        return process_3_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}



void test_process_start(void){

        process_1_internals_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        process_2_internals_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        process_3_internals_fake.return_val = XCONTIKI_OS_SYS_PROTOTHREAD__YIELDED;
        xcontiki_os_sys_Process__start(&process_1, NULL);
        xcontiki_os_sys_Process__start(&process_2, NULL);
        xcontiki_os_sys_Process__start(&process_3, NULL);

        TEST_ASSERT_CALLED(process_1_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, process_1_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL, process_1_internals_fake.arg1_val);

        TEST_ASSERT_CALLED(process_2_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, process_2_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL, process_2_internals_fake.arg1_val);

        TEST_ASSERT_CALLED(process_3_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT, process_3_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL, process_3_internals_fake.arg1_val);

        //Start only once
        xcontiki_os_sys_Process__start(&process_1, NULL);
        xcontiki_os_sys_Process__start(&process_2, NULL);
        xcontiki_os_sys_Process__start(&process_3, NULL);
        TEST_ASSERT_CALLED_TIMES(1, process_1_internals);
        TEST_ASSERT_CALLED_TIMES(1, process_2_internals);
        TEST_ASSERT_CALLED_TIMES(1, process_3_internals);

        int process_1_counter=0;
        int process_2_counter=0;
        int process_3_counter=0;
        struct xcontiki_os_sys_Process *q;
        for(q = xcontiki_os_sys_Process__list; q != NULL; q = q->next) {
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


        TEST_ASSERT(xcontiki_os_sys_Process__is_running(&process_1));
        TEST_ASSERT(xcontiki_os_sys_Process__is_running(&process_2));
        TEST_ASSERT(xcontiki_os_sys_Process__is_running(&process_3));


}
