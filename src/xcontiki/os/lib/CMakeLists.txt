add_library(xcontiki_os_lib STATIC
        xcontiki_os_lib.h
        xcontiki_os_lib_assert.c xcontiki_os_lib_assert.h
        xcontiki_os_lib_CircularList.c xcontiki_os_lib_CircularList.h
        xcontiki_os_lib_Ringbuf.c xcontiki_os_lib_Ringbuf.h
        xcontiki_os_lib_RingbufIndex.c xcontiki_os_lib_RingbufIndex.h
        )

target_include_directories(xcontiki_os_lib PUBLIC "../../..")
