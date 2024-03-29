#include <onix/interrupt.h>
#include <onix/debug.h>
#include <onix/onix.h>
#define LOGK(fmt,args...) DEBUGK(fmt,##args)

extern void memory_map_init();
extern void mapping_init();
extern void arena_init();
extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void task_init();
extern void syscall_init();
extern void hang();
extern void keyboard_init();
extern void tss_init();

void kernel_init()
{
    //tss 段初始化
    tss_init();
    //初始化内存映射表，标记已经被占用的内存页,还未开启虚拟内存
    memory_map_init(); 
    //初始化内存映射
    mapping_init();
    //
    arena_init();
    //中断初始化
    interrupt_init();
    // //时钟初始化
    clock_init();
    //键盘初始化
    keyboard_init();
    // //时间戳初始化
    // time_init();
    
    task_init();
    syscall_init();
    set_interrupt_state(true);
    
}