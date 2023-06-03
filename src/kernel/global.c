#include <onix/global.h>
#include <onix/string.h>
#include <onix/debug.h>


descriptor_t gdt[GDT_SIZE]; //内核全局描述符表
pointer_t gdt_ptr;          //内核全局描述符表指针

void descriptor_init(descriptor_t *desc, u32 base, u32 limit)
{
    desc->base_low = base & 0xffffff;
    desc->base_high = (base >> 24) & 0xff;
    desc->limit_low = limit & 0xffff;
    desc->limit_high = (limit >> 16) & 0xf;
}

//初始化内核全局描述符表
//GDT用于管理内存分段
/*
1.sgdt指令用于将GDT（全局描述符表）的基地址和限制值存储在指定的内存位置中。
  具体来说，它会将GDT寄存器的值（即GDTR）加载到指定的内存位置中。
2.lgdt指令与sgdt指令相反，它的作用是将指定内存地址中的数据加载到GDT寄存器中。
*/
void gdt_init()
{
    // DEBUGK("init gdt!!!!\n");
    // //将GDTR寄存器的值读入到 gdt_ptr中
    // asm volatile("sgdt gdt_ptr");
    // //拷贝loader中加载的GDT到内核定义的gdt中
    // memcpy(&gdt, (void *)gdt_ptr.base, gdt_ptr.limit + 1);
    // //设置新的gdtr的值，更改起始地址和长度界限
    // gdt_ptr.base = (u32)&gdt;
    // gdt_ptr.limit = sizeof(gdt) - 1;
    // //加载将gdt_ptr内存中的值加载到GDTR寄存器中
    // asm volatile("lgdt gdt_ptr\n");

    DEBUGK("init gdt!!!\n");

    memset(gdt, 0, sizeof(gdt));

    descriptor_t *desc;
    desc = gdt + KERNEL_CODE_IDX;
    descriptor_init(desc, 0, 0xFFFFF);
    desc->segment = 1;     // 代码段
    desc->granularity = 1; // 4K
    desc->big = 1;         // 32 位
    desc->long_mode = 0;   // 不是 64 位
    desc->present = 1;     // 在内存中
    desc->DPL = 0;         // 内核特权级
    desc->type = 0b1010;   // 代码 / 非依从 / 可读 / 没有被访问过

    desc = gdt + KERNEL_DATA_IDX;
    descriptor_init(desc, 0, 0xFFFFF);
    desc->segment = 1;     // 数据段
    desc->granularity = 1; // 4K
    desc->big = 1;         // 32 位
    desc->long_mode = 0;   // 不是 64 位
    desc->present = 1;     // 在内存中
    desc->DPL = 0;         // 内核特权级
    desc->type = 0b0010;   // 数据 / 向上增长 / 可写 / 没有被访问过

    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;
}