#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>
#include "tasks_for_test.h"


static xcontiki_os_sys_Protothread__pt_t pt;
static xcontiki_os_sys_Protothread__pt_t child_pt;
static bool condition;
static uint8_t counter;
static XCONTIKI_OS_SYS_PROTOTHREAD__THREAD result;
static uint8_t stage_indicator;
static uint8_t child_stage_indicator;

void setUp(void){
        memset(&pt, 0, sizeof(pt));
        memset(&child_pt, 0, sizeof(child_pt));
        stage_indicator = 0;
        child_stage_indicator = 0;
}

void tearDown(void){
}

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD child_task_for_test(void){

        XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(child_pt);
        child_stage_indicator = 1;
        XCONTIKI_OS_SYS_PROTOTHREAD__YIELD(child_pt);
        child_stage_indicator = 2;
        XCONTIKI_OS_SYS_PROTOTHREAD__END(child_pt);
}



XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task_for_test(void){

        counter++;

        XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(pt);
        stage_indicator = 1;
        XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_WHILE(pt,counter<2);
        stage_indicator = 2;
        XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL(pt,counter>=4);
        stage_indicator = 3;
        XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_THREAD(pt,child_task_for_test());
        stage_indicator = 4;
        XCONTIKI_OS_SYS_PROTOTHREAD__YIELD(pt);
        stage_indicator = 5;
        XCONTIKI_OS_SYS_PROTOTHREAD__SPAWN(pt,child_pt,child_task_for_test());
        stage_indicator = 6;
        XCONTIKI_OS_SYS_PROTOTHREAD__YIELD_UNTIL(pt,counter>=5);
        stage_indicator = 7;
        XCONTIKI_OS_SYS_PROTOTHREAD__EXIT(pt);
        stage_indicator = 8;

        XCONTIKI_OS_SYS_PROTOTHREAD__END(pt);
}


void test_whenLocalContinuationIsCorrupted_thenTaskShouldEnds_andLocalContinuationShouldResets(void){
        pt.local_continuation = UINT16_MAX;


        result = task_for_test();

        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROTOTHREAD__ENDED, result);
        TEST_ASSERT_EQUAL(0, pt.local_continuation);
}



void test_waiting_for_condition(void){
        counter=0;
        result = task_for_test();
        TEST_ASSERT_EQUAL(XCONTIKI_OS_SYS_PROTOTHREAD__WAITING, result);
        TEST_ASSERT_EQUAL(1, stage_indicator);

        result = task_for_test();
        TEST_ASSERT_EQUAL( XCONTIKI_OS_SYS_PROTOTHREAD__WAITING, result);
        TEST_ASSERT_EQUAL(2, stage_indicator);

        result = task_for_test();
        TEST_ASSERT_EQUAL( XCONTIKI_OS_SYS_PROTOTHREAD__WAITING, result);
        TEST_ASSERT_EQUAL(2, stage_indicator);

        result = task_for_test();
        TEST_ASSERT_EQUAL( XCONTIKI_OS_SYS_PROTOTHREAD__WAITING, result);
        TEST_ASSERT_EQUAL(3, stage_indicator);
        TEST_ASSERT_EQUAL(1, child_stage_indicator);

        result = task_for_test();
        TEST_ASSERT_EQUAL( XCONTIKI_OS_SYS_PROTOTHREAD__WAITING, result);
        TEST_ASSERT_EQUAL(4, stage_indicator);
        TEST_ASSERT_EQUAL(2, child_stage_indicator);
}
