#include <onix/interrupt.h>
#include <onix/global.h>
#include <onix/debug.h>
#include <onix/printk.h>
#include <onix/stdlib.h>
#define LOGK(fmt,args...) DEBUGK(fmt,##args)

#define ENTRY_SIZE 0x30

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1
#define PIC_EOI 0x20

gate_t idt[IDT_SIZE];
pointer_t idt_ptr;
//中断向量表
handler_t handler_table[IDT_SIZE];
//中断函数入口地址
extern handler_t handler_entry_table[ENTRY_SIZE];



static char *messages[] = {
    "#DE Divide Error\0",
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};
void exception_handler(int vector)
{
    char *message = NULL;
    if(vector < 22)
    {
        message = messages[vector];
    }
    else
    {
        message = messages[15];
    }
    printk("Exception : [0x%02X] %s \n",vector,messages[vector]);
    // 阻塞
    hang();
}
// 通知中断控制器，中断处理结束
void send_eoi(int vector)
{
    if (vector >= 0x20 && vector < 0x28)
    {
        outb(PIC_M_CTRL, PIC_EOI);
    }
    if (vector >= 0x28 && vector < 0x30)
    {
        outb(PIC_M_CTRL, PIC_EOI);
        outb(PIC_S_CTRL, PIC_EOI);
    }
}

u32 counter = 0;
void default_handler(int vector)
{
    send_eoi(vector);
    LOGK(" [%d] default interrupt called %d...\n",vector,counter++);
}

// 初始化外中断控制器
void pic_init()
{
    outb(PIC_M_CTRL, 0b00010001); // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_M_DATA, 0x20);       // ICW2: 起始中断向量号 0x20
    outb(PIC_M_DATA, 0b00000100); // ICW3: IR2接从片.
    outb(PIC_M_DATA, 0b00000001); // ICW4: 8086模式, 正常EOI

    outb(PIC_S_CTRL, 0b00010001); // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_S_DATA, 0x28);       // ICW2: 起始中断向量号 0x28
    outb(PIC_S_DATA, 2);          // ICW3: 设置从片连接到主片的 IR2 引脚
    outb(PIC_S_DATA, 0b00000001); // ICW4: 8086模式, 正常EOI

    // outb(PIC_M_DATA, 0b11111111); // 关闭所有中断
    // outb(PIC_S_DATA, 0b11111111); // 关闭所有中断
}
void idt_init()
{
    //初始化中断描述符表
    for (size_t i = 0; i < IDT_SIZE; i++)
    {
        gate_t *gate = &idt[i];
        handler_t handler = handler_entry_table[i];
        gate->offset0 = (u32)handler & 0xffff;         //段内偏移 0 ~ 15位
        gate->offset1 = ((u32)handler >> 16) & 0xffff; //段内偏移 16 ~ 31位
        gate->selector = 1 << 3;    //代码段选择子
        gate->reserved = 0;         //保留不用
        gate->type = 0b1110;        
        gate->DPL = 0;              // 使用 int 指令访问的最低权限
        gate->present = 1;          // 是否有效
    }
    for(size_t i = 0; i < 0x20; i++)
    {
        handler_table[i] = exception_handler;
    }
    for (size_t i = 0x20; i < ENTRY_SIZE; i++)
    {
        handler_table[i] = default_handler;
    }
    //加载中断描述符表
    idt_ptr.base = (u32)idt;
    idt_ptr.limit = sizeof(idt) - 1;
    BMB;
    asm volatile("lidt idt_ptr\n");
}


void interrupt_init()
{
    pic_init();
    idt_init();
}
