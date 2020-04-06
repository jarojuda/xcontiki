#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>

static xcontiki_os_sys_Protothread__pt_t pt_ctrl;






void setUp(void){
    memset(&pt_ctrl, 0, sizeof(pt_ctrl));
}

void tearDown(void){

}

void test_protothread_init(void){



TEST_FAIL();


}

