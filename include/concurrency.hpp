#ifndef CONCURRENCY_HPP
#define CONCURRENCY_HPP

#include <cstdlib>
#include <iostream>
#include <gctypes.h>
#include <ogc/lwp.h>
#include <ogc/mutex.h>

// Inline functions to add automatic stderr message and exit(1) call if any LWP_* call fails.

namespace advhb {

    inline s32 LWP_MutexInit_with_sysexit(mutex_t *mutex, bool use_recursive) {
        s32 r = LWP_MutexInit(mutex, use_recursive);
        if (r != 0) {
            std::cerr << "LWP_MutexInit call failed" << std::endl;
            std::exit(1);
        }
        return r;
    }

    inline s32 LWP_MutexDestroy_with_sysexit(mutex_t mutex) {
        s32 r = LWP_MutexDestroy(mutex);
        if (r != 0) {
            std::cerr << "LWP_MutexDestroy call failed" << std::endl;
            std::exit(1);
        }
        return r;
    }

    inline s32 LWP_MutexLock_with_sysexit(mutex_t mutex) {
        s32 r = LWP_MutexLock(mutex);
        if (r != 0) {
            std::cerr << "LWP_MutexLock call failed" << std::endl;
            std::exit(1);
        }
        return r;
    }

    inline s32 LWP_MutexUnlock_with_sysexit(mutex_t mutex) {
        s32 r = LWP_MutexUnlock(mutex);
        if (r != 0) {
            std::cerr << "LWP_MutexUnlock call failed" << std::endl;
            std::exit(1);
        }
        return r;
    }

    inline s32 LWP_CreateThread_with_sysexit(
        lwp_t *thethread,
        void* (*entry)(void *),
        void *arg,
        void *stackbase,
        u32 stack_size,
        u8 prio) {
        s32 r = LWP_CreateThread(thethread, entry, arg, stackbase, stack_size, prio);
        if (r != 0) {
            std::cerr << "LWP_CreateThread call failed" << std::endl;
            std::exit(1);
        }
        return r;
    }

    inline s32 LWP_JoinThread_with_sysexit(lwp_t thethread, void** value_ptr) {
        s32 r = LWP_JoinThread(thethread, value_ptr);
        if (r != 0) {
            std::cerr << "LWP_JoinThread call failed" << std::endl;
            std::exit(1);
        }
        return r;
    }

}

#endif /*CONCURRENCY_HPP*/
