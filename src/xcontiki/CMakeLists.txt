add_library(xcontiki STATIC
        xcontiki.c xcontiki.h
        os/xcontiki_os.h)

add_subdirectory(arch)
add_subdirectory(os/lib)
add_subdirectory(os/sys)

target_link_libraries(xcontiki PUBLIC xcontiki_os_sys xcontiki_os_lib xcontiki_arch)

target_include_directories(xcontiki PUBLIC "../")
