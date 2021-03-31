#include "xcontiki/xcontiki.h"


static int watchdog_calls_counter;
static int number_of_watchdog_calls_indicating_an_infinite_loop;

static const uint32_t time_points[6] = {
    0,
    XCONTIKI_ARCH_CLOCK__MAX - 1,
    0,
    XCONTIKI_ARCH_CLOCK__MAX / 2 + 1,
    XCONTIKI_ARCH_CLOCK__MAX / 2,
    XCONTIKI_ARCH_CLOCK__MAX / 2 + 1
};

uint32_t get_timer(void) {
    static uint8_t i = 0;
    uint32_t time_point;

    time_point = time_points[i];
    ++i;
    return time_point;
}

static void watchdog_periodic_fake(void) {
    watchdog_calls_counter++;
}

void setUp(void) {
    xcontiki_arch_Clock__init();
    watchdog_calls_counter = 0;
}

void tearDown(void) {
}

void test_whenMaxIntervalIsSet_thenClockWaitShouldNotLoopForever(void) {


    xcontiki_arch_Clock__wait(XCONTIKI_ARCH_CLOCK__MAX);

    xcontiki_arch_Clock__wait(XCONTIKI_ARCH_CLOCK__MAX);
}

