#include "unity.h"
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
        process_1_internals(ev,data);
        XCONTIKI_OS_SYS_PROCESS__END();
}


void test_process_start(void){
        xcontiki_os_sys_Process__start(&process_1, NULL);
        TEST_ASSERT_CALLED(process_1_internals);
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROCESS__EVENT_INIT ,process_1_internals_fake.arg0_val);
        TEST_ASSERT_EQUAL(NULL ,process_1_internals_fake.arg1_val);

}
