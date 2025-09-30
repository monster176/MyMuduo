#pragma once

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
    // __thread关键字是告诉每个线程都单独有一个t_cachedTid,而不共享进程的t_cachedTid
    extern __thread int t_cachedTid;

    // 用于缓存当前线程的id
    void CacheId();

    inline int tid()
    {
        //  告诉编译器你期望t_cachedTid==0这种情况发生几率小一点 ，编译器会自动优化，返回值就是第一个参数的值
        if(__builtin_expect(t_cachedTid==0,0))
        {
            CacheId();
        }
        return t_cachedTid;
    }
}