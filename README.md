# xcontiki
Port of the Contiki OS for the XC compilers.

The XC8 compiler for the 8-bit PIC microcontrollers does not compile the Contiki OS sources.

This is an attempt to adapt Contiki code to the shortcomings of the compiler and architecture.

1. Where possible, pointers has been removed.
2. Recursion was replaced by iteration.
etc.
