add_library(xcontiki_arch OBJECT
        xcontiki_arch.h
        xcontiki_arch_Clock.c xcontiki_arch_Clock.h
        xcontiki_arch_Sleepyhead.c xcontiki_arch_Sleepyhead.h
        xcontiki_arch_Watchdog.c xcontiki_arch_Watchdog.h
        )

include_directories(xcontiki_arch PUBLIC "../.." )
