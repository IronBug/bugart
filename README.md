Bugart (Bogart clone, adopted for Linux)
----------------------------------------

This is a code adoption of **Bogart** (https://github.com/tyler/Bogart) code, made for **Linux**. Written in **C**.

Code was checked on **Linux Void**, built with **musl**, with **GCC 7.2.0**. But should work with **glibc** as well.

Dependencies: 
**libevent**, **hiredis**

The code has some things to be done yet. Also it lacks the original simplicity, since there's no lambdas in C on Linux. 
Perhaps, I will try to write it in some better way, but this is the matter of spare time.

Code was checked with valgrind for absence of memory leaks. Although valgrind is indignant about some libevent inner calls, but I did not dig into this much.
