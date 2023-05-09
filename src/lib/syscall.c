#include <onix/syscall.h>

/**
 * @brief  单个参数的syscall
 * @param  nr: 系统调用号
 * @return _inline: 
 */
static _inline u32 _syscall0(u32 nr)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr));
    return ret;
}

/**
 * @brief  两个参数的 syscall
 * @param  nr: 系统调用号
 * @param  arg: 系统调用函数的参数
 * @return _inline: 
 */
static _inline u32 _syscall1(u32 nr, u32 arg)
{
    u32 ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg));
    return ret;
}


u32 test()
{
    return _syscall0(SYS_NR_TEST);
}

/**
 * @brief  系统调用：任务调度
 */
void yield()
{
    _syscall0(SYS_NR_YIELD);
}

/**
 * @brief  slepp 系统调用
 * @param  ms: 睡眠实践 ms
 */
void sleep(u32 ms)
{
    _syscall1(SYS_NR_SLEEP, ms);
}