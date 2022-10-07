add_library(xcontiki_os_sys STATIC
        xcontiki_os_sys.h
        xcontiki_os_sys_Etimer.c xcontiki_os_sys_Etimer.h xcontiki_os_sys_Etimer_priv.h
        xcontiki_os_sys_Process.c xcontiki_os_sys_Process.h xcontiki_os_sys_Process_priv.h
        xcontiki_os_sys_Protothread.h xcontiki_os_sys_ProtothreadScheduler.inc
        xcontiki_os_sys_Timer.c xcontiki_os_sys_Timer.h xcontiki_os_sys_Timer_priv.h
        )

target_include_directories(xcontiki_os_sys PUBLIC "../../..")
