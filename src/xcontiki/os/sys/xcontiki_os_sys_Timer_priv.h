/*
 * File:   xcontiki_os_sys_Timer_priv.h
 * Author: jarojuda
 *
 */

#ifndef XCONTIKI_OS_SYS_TIMER_PRIV_H
#error Include only from the xcontiki_os_sys_Timer.c file
#endif	/* XCONTIKI_OS_SYS_TIMER_PRIV_H */

#ifdef	__cplusplus
extern "C" {
#endif


    static xcontiki_arch_Clock__time_t start[XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER];
    static xcontiki_arch_Clock__time_t interval[XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER];
    static xcontiki_arch_Clock__time_t previous_diff[XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER];

    static struct {
        unsigned allocated : 1;
        unsigned running : 1;
        unsigned expired : 1;
    } timer_flags[XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER] = {0};

    static xcontiki_os_sys_Timer__timer_id_t allocate_new_timer(void) {
        xcontiki_os_sys_Timer__timer_id_t i;
        for (i = 1; i < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER; i++) {
            if (0 == timer_flags[i].allocated) {
                timer_flags[i].allocated = 1;
                break;
            }
        }
        assert(i < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER && "No more timers to allocate");
        if (i < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER) {
            return i;
        } else {
            PRINTF(("No more timers to allocate\n"));
        }
        return 0;
    }

    void set_next_wake_up(xcontiki_arch_Clock__time_t time) {

    }

    void wake_up(void) {
        xcontiki_os_sys_Timer__timer_id_t t;
        xcontiki_arch_Clock__time_t now;
        xcontiki_arch_Clock__time_t diff;

        now = xcontiki_arch_Clock__time();
        for (t = 1; t < XCONTIKI_OS_SYS_TIMER__CONF_TIMERS_NUMBER; t++) {
            if (timer_flags[t].running) {
                diff = now - start[t];
                if (diff >= interval[t] || diff < previous_diff[t]) {
                    timer_flags[t].expired = true;
                    timer_flags[t].running = false;
                } else {
                    previous_diff[t] = diff;
                }
            }
        }
    }







#ifdef	__cplusplus
}
#endif
