#include "bitmap.h"            //函数定义
#include "global.h"            //global.h 不清楚
#include "string.h"            //memset函数要用
#include "interrupt.h"         
#include "print.h"             
#include "debug.h"

#define BITMAP_MASK 1

//位图的初始化
void bitmap_init(struct bitmap* btmp)
{
    memset(btmp->bits,0,btmp->btmp_bytes_len);
    return;
}

/*判断bit idx位是否为1,若为1,则返回true,否则返回 false*/
int bitmap_scan_test(struct bitmap* btmp,uint32_t bit_idx)  //一个8位的数 bit_idx/8 找数组下标 %得索引下的具体位置
{
    uint32_t byte_idx = bit_idx/8;
    uint32_t byte_pos = bit_idx%8;
    return (btmp->bits[byte_idx] & (BITMAP_MASK << byte_pos));
}

/*在位图中申请连续cnt 个位，成功，则返回其起始位下标，失败，返回-1*/
int bitmap_scan(struct bitmap* btmp,uint32_t cnt)
{
    ASSERT(cnt >= 1);
    uint32_t idx_byte = 0;
    
    //逐字节的比较
    while(idx_byte < btmp->btmp_bytes_len && btmp->bits[idx_byte] == 0xff)
    	++idx_byte;
    
    //找到末尾了
    if(idx_byte == btmp->btmp_bytes_len)return -1;
    

    //逐位比较
    uint32_t idx_bit = 0;
    while((btmp->bits[idx_byte] & (uint8_t)(BITMAP_MASK << idx_bit)))
    	++idx_bit;


    uint32_t idx_bit_start = idx_bit + 8*idx_byte;
    if(cnt == 1)return idx_bit_start;
    
    
    uint32_t bit_left = btmp->btmp_bytes_len*8 - idx_bit_start;//还有多少位
    uint32_t next_bit = idx_bit_start + 1;//下一个位置
    uint32_t count = 1;//找到多少空闲的位

    idx_bit_start = -1;//找不到连续的位就直接返回
    while(bit_left--)
    {
        if(!(bitmap_scan_test(btmp,next_bit)))count++;
        else count = 0;

        if(count==cnt)
        {
            idx_bit_start = next_bit - cnt + 1;
            break;
        }

        next_bit++;
    }

    return idx_bit_start;
}

void bitmap_set(struct bitmap* btmp,uint32_t bit_idx,int8_t value)
{
    ASSERT(value == 1 || value == 0);
    uint32_t byte_idx = bit_idx/8;
    uint32_t byte_pos = bit_idx%8;
    if(value)btmp->bits[byte_idx] |=  (BITMAP_MASK << byte_pos);//1
    else btmp->bits[byte_idx] &= ~(BITMAP_MASK << byte_pos);//0
    return;
}
