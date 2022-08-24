#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h> // for backtrace related functions

// TODO: set these elsewhere?
#define DEBUG 1
#define TRACE 1


// Debugging printf macro.
// Works like printf but includes a "DEBUG: file:line:function(): " prefix.
// Controlled by the DEBUG define.
// Can be stripped out of the build by setting DEBUG to 0.
// See: https://stackoverflow.com/a/1644898
#define debug_printf(fmt, ...) do { if (DEBUG) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while(0)


// Debugging trace logger (useful for debugging execution flow on crashes, etc).
// Prints "TRACE: file:line:function()" based on the call site.
// Controlled by the TRACE define.
#define debug_trace() do { if (TRACE) fprintf(stderr, "TRACE: %s:%d:%s()\n", __FILE__, __LINE__, __func__); } while(0)


// Print a stack backtrace.
// The plan is for this to be called as part of the assert(condition) macro below.
void print_backtrace() {
    const int buffer_size = 10;
    void *buffer[buffer_size];
    const int size = backtrace(buffer, buffer_size);
    char **strings = backtrace_symbols(buffer, size);

    if (strings == NULL) {
        fprintf(stderr, "backtrace found %d frames:\n", size);
        backtrace_symbols_fd(buffer, size, fileno(stderr));
    } else {
        fprintf(stderr, "backtrace found %d frames:\n", size - 1); // don't report the print_backtrace() frame itself
        for (int i = 1; i < size; i += 1) {
            fprintf(stderr, "\t%s\n", strings[i]);
        }
        free(strings);
    }
}

// Assert some condition is true (evalutates to 0), otherwise log the failure location, print the strack trace, and exit the program.
#define assert(condition) do { if (!(condition)) { fprintf(stderr, "ASSERT: %s:%d:%s()\n", __FILE__, __LINE__, __func__); print_backtrace(); exit(condition); } } while(0)

#ifdef __cplusplus
}
#endif

#endif