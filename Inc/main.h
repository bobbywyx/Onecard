#ifndef P2_MAIN_H
#define P2_MAIN_H


#if defined(_WIN32) || defined(__CYGWIN__)
// Windows (x86 or x64)
#include <windows.h>
#define VERSION "0.0.1 on Windows"

#elif defined(__linux__)
// Linux
#include <sys/ioctl.h>
#include <unistd.h>

#define VERSION "0.0.1 on Linux"

#elif defined(__APPLE__) && defined(__MACH__)
// Mac OS
#include <sys/ioctl.h>
#include <unistd.h>

#define VERSION "0.0.1 Mac OS"

#elif defined(unix) || defined(__unix__) || defined(__unix)
    // Unix like OS
    // ...
#else
    #error Unknown environment!
#endif

#endif //P2_MAIN_H
