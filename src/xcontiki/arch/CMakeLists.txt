add_library(xcontiki_arch STATIC
        xcontiki_arch.h xcontiki_arch_build.h
        xcontiki_arch_Clock.c xcontiki_arch_Clock.h
        xcontiki_arch_Sleepyhead.c xcontiki_arch_Sleepyhead.h
        os/xcontiki_arch_os.h
        dev/xcontiki_arch_dev.c dev/xcontiki_arch_dev.h
        dev/xcontiki_arch_dev_Watchdog.c dev/xcontiki_arch_dev_Watchdog.h
        )

include_directories(xcontiki_arch PUBLIC "../..")
