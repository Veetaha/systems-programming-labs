#pragma once

#include <errno.h>
#include <string.h>
#include <stdio.h>

#define loop while(true)
#define let __auto_type
#define println(str, params...) printf(str "\n", ##params)
#define panic(fmt...) ({ println(fmt); exit(1); })

#define Option(T) Option_##T
#define define_option(T) \
typedef struct {         \
    bool is_some;        \
    T val;               \
} Option_##T             \

#define Some(T, val) ((Option(T)) { true, (val) })
#define None(T) ((Option(T)) { false })

#define expect(option, panic_args...) \
({                                    \
    if (!option.is_some) {            \
        panic(panic_args);            \
    }                                 \
    option.val;                       \
})


#define unwrap(option) \
expect(option, "Failed to unwrap() option at %s:%u. Errno: %s", __FILE__, __LINE__, strerror(errno))


#define lambda(return_type, function_body) \
({                                         \
    return_type __fn__ function_body       \
        __fn__;                            \
})
