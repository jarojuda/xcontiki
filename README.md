# xcontiki

Port of the Contiki OS for the XC compilers.

The XC8 compiler for the 8-bit PIC microcontrollers does not compile the Contiki OS sources.
Są problemy ze wskaźnikami do struktur, strukturami zawierającymi wskaźniki, a nawet tablicami struktur. 

Even the simple program can generate such errors:

for simple assignment like this:

```c
if (diff >= t->interval || diff < t->previous_diff) {
    t->expired = true;
    return true;
}
```
_error: (1466) registers unavailable for code generation of this expression_
```c
while(t->next != NULL) {
          if(t->next->p == p) {
            t->next = t->next->next;
          } else {
            t = t->next;
          }
        }
```
_error: (712) can't generate code for this expression_




This is an attempt to adapt Contiki code to the shortcomings of the compiler and architecture.

1.  Where possible, pointers has been removed.
2.  Recursion was replaced by iteration.
    etc.
