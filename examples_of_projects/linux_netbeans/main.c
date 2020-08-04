/*
 * Copyright (c) 2020, Jaroslaw Juda <mail at JaroslawJuda.site>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Based on the Contiki http://contiki-os.org
 * and the Contiki-NG  http://www.contiki-ng.org/
 * projects.
 */

/*
 * File:   main.c
 *
 * Author: Jaroslaw Juda <mail at JaroslawJuda.site>
 *
 */

#include "xcontiki/xcontiki.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>
#include <stdio.h>

/*
 *
 */



static xcontiki_os_sys_Protothread__pt_t pt;
static xcontiki_os_sys_Protothread__pt_t child_pt;
static bool condition;
static uint8_t counter;
static XCONTIKI_OS_SYS_PROTOTHREAD__THREAD result;
static uint8_t stage_indicator;
static uint8_t child_stage_indicator;

void setUp(void) {
    memset(&pt, 0, sizeof (pt));
    memset(&child_pt, 0, sizeof (child_pt));
    stage_indicator = 0;
    child_stage_indicator = 0;
}

void tearDown(void) {
}

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD child_task_for_test(void) {

    XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(child_pt);
    child_stage_indicator = 1;
    XCONTIKI_OS_SYS_PROTOTHREAD__YIELD(child_pt);
    child_stage_indicator = 2;
    XCONTIKI_OS_SYS_PROTOTHREAD__END(child_pt);
}

XCONTIKI_OS_SYS_PROTOTHREAD__THREAD task_for_test(void) {

    counter++;

    XCONTIKI_OS_SYS_PROTOTHREAD__BEGIN(pt);
    stage_indicator = 1;
    XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_WHILE(pt, counter < 2);
    stage_indicator = 2;
    XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_UNTIL(pt, counter >= 4);
    stage_indicator = 3;
    XCONTIKI_OS_SYS_PROTOTHREAD__WAIT_THREAD(pt, child_task_for_test());
    stage_indicator = 4;
    XCONTIKI_OS_SYS_PROTOTHREAD__YIELD(pt);
    stage_indicator = 5;
    XCONTIKI_OS_SYS_PROTOTHREAD__SPAWN(pt, child_pt, child_task_for_test());
    stage_indicator = 6;
    XCONTIKI_OS_SYS_PROTOTHREAD__YIELD_UNTIL(pt, counter >= 5);
    stage_indicator = 7;
    XCONTIKI_OS_SYS_PROTOTHREAD__EXIT(pt);
    stage_indicator = 8;

    XCONTIKI_OS_SYS_PROTOTHREAD__END(pt);
}

void test_whenLocalContinuationIsCorrupted_thenTaskShouldEnds_andLocalContinuationShouldResets(void) {
    pt.local_continuation = UINT16_MAX;


    result = task_for_test();

}

void test_waiting_for_condition(void) {
    counter = 0;
    result = task_for_test();
    printf("result %d\n", result);
    printf("stage_indicator %d\n", stage_indicator);

    result = task_for_test();
    printf("result %d\n", result);
    printf("stage_indicator %d\n" ,stage_indicator);

    result = task_for_test();
    printf("result %d\n", result);
    printf("stage_indicator %d\n" ,stage_indicator);

    result = task_for_test();
    printf("result %d\n", result);
    printf("stage_indicator %d\n" ,stage_indicator);
    printf("child_stage_indicator %d\n", child_stage_indicator);

    result = task_for_test();
    printf("result %d\n", result);
    printf("stage_indicator %d\n" ,stage_indicator);
    printf("child_stage_indicator %d\n", child_stage_indicator);
}

int main(int argc, char** argv) {
    setUp();
    test_waiting_for_condition();
    return (EXIT_SUCCESS);
}