#include "unity.h"
#include "xcontiki/xcontiki.h"
#include "xcontiki/os/sys/xcontiki_os_sys_Protothread.h"
#include <string.h>
#include "tasks_for_test.h"


static xcontiki_os_sys_Protothread__pt_t pt;


void setUp(void){
  memset(&pt, 0, sizeof(pt));
}

void tearDown(void){
}
