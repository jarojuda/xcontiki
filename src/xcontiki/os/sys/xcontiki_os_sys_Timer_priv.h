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


    static xcontiki_arch_Clock__time_t start[XCONTIKI_OS_SYS_TIMER__CONF_TIMER_NUMBER];
    static xcontiki_arch_Clock__time_t interval[XCONTIKI_OS_SYS_TIMER__CONF_TIMER_NUMBER];
    static xcontiki_arch_Clock__time_t previous_diff[XCONTIKI_OS_SYS_TIMER__CONF_TIMER_NUMBER];
    static struct __PACKED {
        unsigned allocated : 1;
        unsigned running : 1;
        unsigned expired : 1;
    } flags[XCONTIKI_OS_SYS_TIMER__CONF_TIMER_NUMBER];


    static xcontiki_os_sys_Timer__timer_id_t allocate_new_timer(void){
        return 0;
    } 
    
    
#ifdef	__cplusplus
}
#endif



