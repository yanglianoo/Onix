#ifndef ONIX_BITMAP_H
#define ONIX_BITMAP_H

#include <onix/types.h>

typedef struct bitmap_t
{
    u8 *bits; 
    u32 length;
    u32 offset;
}bitmap_t;

// 初始化位图
void bitmap_init(bitmap_t *map, char *bits, u32 length, u32 offest);

// 构造位图
void bitmap_make(bitmap_t *map, char *bits, u32 length, u32 offest);

// 测试位图的某一位是否为 1
bool bitmap_test(bitmap_t *map, u32 index);

// 设置位图某位的值
void bitmap_set(bitmap_t *map, u32 index, bool value);

// 从位图中得到连续的 count 位
int bitmap_scan(bitmap_t *map, u32 count);

#endif