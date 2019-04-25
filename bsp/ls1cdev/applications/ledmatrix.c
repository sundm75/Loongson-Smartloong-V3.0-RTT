/* 
* File      : ledmatric.c 
* 测试全彩屏例程
* HUB75接口：
* ------------------------
* |B1 NC G2 NC B D LAT NC|
* |R1 B1 R2 B2 A C CLK OE|
* ------------------------
* 对应连接智龙开发板引脚：
* -------------------------
* |74 *  59 *  63 65 67 * |
* |76 75 58 60 62 64 66 68|
* -------------------------
* 测试流程，在msh中运行
* 1. test_led_start 端口初始化 打开定时扫描
* 2. led + 对应参数，显示不同的图形
*    led clr
*    led rectr  显示红矩形框 
*    led rectg  显示绿矩形框 
*    led rtctb  显示蓝矩形框 
*    led ship  显示一条小船
*    led move  移动当前图形
*    led size
*    led c 0 设置当前颜色为0
*    led 1 2 3 4 显示4个数字
*/

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>  
#include <rthw.h>

#include "ls1c.h"  
#include "ls1c_timer.h"  
#include "ls1c_public.h"  
#include "ls1c_gpio.h"  
#include "mipsregs.h"  

#include "mytimer.h"
#include "ledmatrix.h"
#include "morphdigit.h"
#include "dot_matrix.h"

#define led1_gpio   52
#define led2_gpio   53
#define R1          76
#define G1          74
#define B1          75

#define R2          58
#define G2          59
#define B2          60

#define ROW_A       62
#define ROW_B       63
#define ROW_C       64
#define ROW_D       65
#define LED_CLK     66
#define LED_LOCK    67
#define LED_OE      68


#define ROW_0       {gpio_set(ROW_A, 0); gpio_set(ROW_B, 0);gpio_set(ROW_C, 0); gpio_set(ROW_D, 0);}
#define ROW_1       {gpio_set(ROW_A, 1); gpio_set(ROW_B, 0);gpio_set(ROW_C, 0); gpio_set(ROW_D, 0);}
#define ROW_2       {gpio_set(ROW_A, 0); gpio_set(ROW_B, 1);gpio_set(ROW_C, 0); gpio_set(ROW_D, 0);}
#define ROW_3       {gpio_set(ROW_A, 1); gpio_set(ROW_B, 1);gpio_set(ROW_C, 0); gpio_set(ROW_D, 0);}
#define ROW_4       {gpio_set(ROW_A, 0); gpio_set(ROW_B, 0);gpio_set(ROW_C, 1); gpio_set(ROW_D, 0);}
#define ROW_5       {gpio_set(ROW_A, 1); gpio_set(ROW_B, 0);gpio_set(ROW_C, 1); gpio_set(ROW_D, 0);}
#define ROW_6       {gpio_set(ROW_A, 0); gpio_set(ROW_B, 1);gpio_set(ROW_C, 1); gpio_set(ROW_D, 0);}
#define ROW_7       {gpio_set(ROW_A, 1); gpio_set(ROW_B, 1);gpio_set(ROW_C, 1); gpio_set(ROW_D, 0);}                    
#define ROW_8       {gpio_set(ROW_A, 0); gpio_set(ROW_B, 0);gpio_set(ROW_C, 0); gpio_set(ROW_D, 1);}                    
#define ROW_9       {gpio_set(ROW_A, 1); gpio_set(ROW_B, 0);gpio_set(ROW_C, 0); gpio_set(ROW_D, 1);}                  
#define ROW_10      {gpio_set(ROW_A, 0); gpio_set(ROW_B, 1);gpio_set(ROW_C, 0); gpio_set(ROW_D, 1);}                  
#define ROW_11      {gpio_set(ROW_A, 1); gpio_set(ROW_B, 1);gpio_set(ROW_C, 0); gpio_set(ROW_D, 1);}                    
#define ROW_12      {gpio_set(ROW_A, 0); gpio_set(ROW_B, 0);gpio_set(ROW_C, 1); gpio_set(ROW_D, 1);}                    
#define ROW_13      {gpio_set(ROW_A, 1); gpio_set(ROW_B, 0);gpio_set(ROW_C, 1); gpio_set(ROW_D, 1);}               
#define ROW_14      {gpio_set(ROW_A, 0); gpio_set(ROW_B, 1);gpio_set(ROW_C, 1); gpio_set(ROW_D, 1);}              
#define ROW_15      {gpio_set(ROW_A, 1); gpio_set(ROW_B, 1);gpio_set(ROW_C, 1); gpio_set(ROW_D, 1);}                 
                    
#define LAT_LE_H    gpio_set(LED_LOCK, 1);
#define LAT_LOCK    gpio_set(LED_LOCK, 0);

#define DAT_R1(x)    {gpio_set(R1, x);}
#define DAT_G1(x)    {gpio_set(G1, x);}
#define DAT_B1(x)    {gpio_set(B1, x);}

#define DAT_R2(x)    {gpio_set(R2, x);}
#define DAT_G2(x)    {gpio_set(G2, x);}
#define DAT_B2(x)    {gpio_set(B2, x);}

#define CTRL_CLK(x)  {gpio_set(LED_CLK, x);}

dat_rgb_t dat;

static rt_bool_t b_disp_reinit = 0;

static void led_disp_reinit(void)
{
    b_disp_reinit = 0;
    morph_reinit();
}

/*
x=0~63 y=0~31
*/
void draw_pixel(unsigned char x, unsigned char y, ColorDef c)
{
    switch(c)
    {
        case RED:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case GREEN:
            if(x < 32)
            {
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case BLUE:
            if(x < 32)
            {
                dat.b32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.b32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case YELLOW:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case PURPLE:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
                dat.b32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
                dat.b32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case CYAN:
            if(x < 32)
            {
                dat.b32[31 - y][0] |= (1 << x);
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.b32[31 - y][1] |= (1 << (x - 32));
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case WHITE:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
                dat.b32[31 - y][0] |= (1 << x);
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
                dat.b32[31 - y][1] |= (1 << (x - 32));
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case BLACK:
            if(x < 32)
            {
                dat.r32[31 - y][0] &= ~(1 << x);
                dat.b32[31 - y][0] &= ~(1 << x);
                dat.g32[31 - y][0] &= ~(1 << x);
            }
            else
            {
                dat.r32[31 - y][1] &= ~(1 << (x - 32));
                dat.b32[31 - y][1] &= ~(1 << (x - 32));
                dat.g32[31 - y][1] &= ~(1 << (x - 32));
            }
            break;

        default:
            break;
    }
}

/*
x2>x,y2>y
画线
*/
void draw_line(unsigned char x, unsigned char y,unsigned char deltax, unsigned char deltay, ColorDef c)
{
    unsigned char i;
    if(deltax == 0)
    {
        for(i=0; i<deltay; i++)
        {
            draw_pixel(x, y + i, c);
        }
    }
    else if(deltay == 0)
    {
        for(i=0; i<deltax; i++)
        {
            draw_pixel(x + i, y, c);
        }
    }
}

void move_effect(ColorDef color)
{
    unsigned char i,j;
    unsigned char temp_bit;

    for(i=0;i<16;i++)
    {
        temp_bit = dat.move_buf[i][0] & 0x1;
        for(j=0;j<MOVE_BUFF-1;j++)
        {
            dat.move_buf[i][j] >>= 1;
            if(dat.move_buf[i][j + 1] & 0x1)
            {
                dat.move_buf[i][j] |= 0x80;
            }
            //rt_kprintf("%d %d=0x%x\n",i,j,dat.move_buf[i][j]);
        }
        dat.move_buf[i][MOVE_BUFF-1] >>= 1;
        if(temp_bit)
        {
            dat.move_buf[i][MOVE_BUFF-1] |= 0x80;
        }
    }


    for(i=0;i<16;i++)
    {
        for(j=0;j<8;j++)
        {
            switch(color)
            {
                case RED:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case GREEN:
                    for(i=0; i<16; i++)
                    {
                        dat.g[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case BLUE:
                    for(i=0; i<16; i++)
                    {
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case YELLOW:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                        dat.g[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case PURPLE:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case CYAN:
                    for(i=0; i<16; i++)
                    {
                        dat.g[i][j] = dat.move_buf[i][j];
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case WHITE:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                        dat.g[i][j] = dat.move_buf[i][j];
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                default:
                    break;

            }
        }
    }
}

/* x=0~7 y=0~16 */
static void disp8x16(unsigned char x, unsigned char y, unsigned char *pVal,ColorDef color)
{
    unsigned char i;

    switch(color)
    {
        case RED:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
            }
            break;

        case GREEN:
            for(i=0; i<16; i++)
            {
                dat.g[y + i][x] = *(pVal + i);
            }
            break;

        case BLUE:
            for(i=0; i<16; i++)
            {
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        case YELLOW:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
                dat.g[y + i][x] = *(pVal + i);
            }
            break;

        case PURPLE:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        case CYAN:
            for(i=0; i<16; i++)
            {
                dat.g[y + i][x] = *(pVal + i);
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        case WHITE:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
                dat.g[y + i][x] = *(pVal + i);
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        default:
            break;

    }
}

static void shift_dat(unsigned char row)
{
    static unsigned char i,j;
    static unsigned long datr_shit1,datr_shit2;
    static unsigned long datg_shit1,datg_shit2;
    static unsigned long datb_shit1,datb_shit2;

    for(i=0; i<2; i++)
    {
        datr_shit1 = dat.r32[row][i];
        datr_shit2 = dat.r32[row + 16][i];
        datg_shit1 = dat.g32[row][i];
        datg_shit2 = dat.g32[row + 16][i];
        datb_shit1 = dat.b32[row][i];
        datb_shit2 = dat.b32[row + 16][i];
        for(j=0; j<32; j++)
        {
            CTRL_CLK(gpio_level_low);
            if(datr_shit1 & 0x1)
            {
                DAT_R1(gpio_level_high);
            }
            else
            {
                DAT_R1(gpio_level_low);
            }

            if(datg_shit1 & 0x1)
            {
                DAT_G1(gpio_level_high);
            }
            else
            {
                DAT_G1(gpio_level_low);
            }

            if(datb_shit1 & 0x1)
            {
                DAT_B1(gpio_level_high);
            }
            else
            {
                DAT_B1(gpio_level_low);
            }

            if(datr_shit2 & 0x1)
            {
                DAT_R2(gpio_level_high);
            }
            else
            {
                DAT_R2(gpio_level_low);
            }

            if(datg_shit2 & 0x1)
            {
                DAT_G2(gpio_level_high);
            }
            else
            {
                DAT_G2(gpio_level_low);
            }

            if(datb_shit2 & 0x1)
            {
                DAT_B2(gpio_level_high);
            }
            else
            {
                DAT_B2(gpio_level_low);
            }
            CTRL_CLK(gpio_level_high);
            datr_shit1 >>= 1;
            datg_shit1 >>= 1;
            datb_shit1 >>= 1;
            datr_shit2 >>= 1;
            datg_shit2 >>= 1;
            datb_shit2 >>= 1;
        }
    }
    CTRL_CLK(gpio_level_low);
}

/* TIM 800 ns */
void led_scan(void)
{
    static unsigned char index = 0;

    shift_dat(index);

    LAT_LE_H;
    switch(index)
    {
        case 0x0:
            ROW_0;
            break;

        case 0x1:
            ROW_1;
            break;

        case 0x2:
            ROW_2;
            break;

        case 0x3:
            ROW_3;
            break;

        case 0x4:
            ROW_4;
            break;

        case 0x5:
            ROW_5;
            break;

        case 0x6:
            ROW_6;
            break;

        case 0x7:
            ROW_7;
            break;

        case 0x8:
            ROW_8;
            break;

        case 0x9:
            ROW_9;
            break;

        case 0xa:
            ROW_10;
            break;

        case 0xb:
            ROW_11;
            break;

        case 0xc:
            ROW_12;
            break;

        case 0xd:
            ROW_13;
            break;

        case 0xe:
            ROW_14;
            break;

        case 0xf:
            ROW_15;
            break;

        default:
            break;
    }
    LAT_LOCK;

    index++;
    if(index > 15)
    {
        index = 0;
    }
}

void color_init(void)
{
    dat.color = RED;
}

void color_change(void)
{
    dat.color++;
    if(dat.color == BLACK)
    {
        dat.color = RED;
    }
}

ColorDef get_color(void)
{
    return dat.color;
}

void GPIO_Init(void)
{
    gpio_init(ROW_D   , gpio_mode_output);
    gpio_init(ROW_C   , gpio_mode_output);
    gpio_init(ROW_B   , gpio_mode_output);
    gpio_init(ROW_A   , gpio_mode_output);
    gpio_init(LED_OE  , gpio_mode_output);
    gpio_init(LED_LOCK, gpio_mode_output);
    gpio_init(LED_CLK , gpio_mode_output);
    gpio_init(R1      , gpio_mode_output);
    gpio_init(G1      , gpio_mode_output);
    gpio_init(B1      , gpio_mode_output);
    gpio_init(R2      , gpio_mode_output);
    gpio_init(G2      , gpio_mode_output);
    gpio_init(B2      , gpio_mode_output);
    gpio_init(led1_gpio     , gpio_mode_output);
    gpio_init(led2_gpio     , gpio_mode_output);
	
    gpio_set (ROW_D   , gpio_level_high); 
    gpio_set (ROW_C   , gpio_level_high); 
    gpio_set (ROW_B   , gpio_level_high); 
    gpio_set (ROW_A   , gpio_level_high); 
    gpio_set (LED_OE  , gpio_level_high); 
    gpio_set (LED_LOCK, gpio_level_high); 
    gpio_set (LED_CLK , gpio_level_high); 
    gpio_set (R1      , gpio_level_high); 
    gpio_set (G1      , gpio_level_high); 
    gpio_set (B1      , gpio_level_high); 
    gpio_set (R2      , gpio_level_high); 
    gpio_set (G2      , gpio_level_high); 
    gpio_set (B2      , gpio_level_high); 
    gpio_set (led1_gpio    , gpio_level_high); 
    gpio_set (led2_gpio    , gpio_level_high); 
}

void disp_background(unsigned char index,ColorDef c)
{
    unsigned char *pNum;
    if(index == 0)  //led clr
    {
        memset(dat.r32,0,LED_BUFF_SIZE);
    }
    else if(index == 1)  //disp symbol
    {
        pNum = &gap[0 << 4];
        disp8x16(2,LINE_1,pNum,c);
        disp8x16(5,LINE_1,pNum,c);
    }
    else if(index == 6) //disp zhufu
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
    else if(index == 7) //disp happy
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
    else if(index == 8) //disp happy
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
    else if(index == 9) //disp ship
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
}


/* *************************************** */
/* MSH CMD */
long led(unsigned char argc, char **argv)
{
    static unsigned char x,y,v,c;
    unsigned char *pNum;

    if(argc == 2)
    {
        const char *cx = argv[1];
        if (!strcmp(cx, "clr"))
        {
            rt_kprintf("clr\r\n");
            memset(dat.r32,0,sizeof(dat));
        }
        else if (!strcmp(cx, "rectr"))
        {
            rt_kprintf("rectr\r\n");
            memcpy(dat.r32,(const void *)&rect[0],sizeof(dat.r32));
        }
        else if (!strcmp(cx, "rectg"))
        {
            rt_kprintf("rectg\r\n");
            memcpy(dat.g32,(const void *)&rect[0],sizeof(dat.g32));
        }
        else if (!strcmp(cx, "rectb"))
        {
            rt_kprintf("rectb\r\n");
            memcpy(dat.b32,(const void *)&rect[0],sizeof(dat.b32));
        }
        else if (!strcmp(cx, "ship"))
        {
            rt_kprintf("ship\r\n");
            memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
        }
        else if (!strcmp(cx, "move"))
        {
            rt_kprintf("move\r\n");
            move_effect(RED);
        }
        else if (!strcmp(cx, "size"))
        {
            rt_kprintf("dat=%d\n",sizeof(dat));
            rt_kprintf("dat.r=%d\n",sizeof(dat.r));
            rt_kprintf("dat.r32=%d\n",sizeof(dat.r32));
            rt_kprintf("dat.move=%d\n",sizeof(dat.move_buf));
            rt_kprintf("dat.color=%d\n",sizeof(dat.color));
        }
    }

    if(argc == 3)
    {
        const char *cx = argv[1];
        const char *cc = argv[2];

        c = atoi((const char *)cc);

        if(!strcmp(cx, "c"))
        {
            rt_kprintf("color=%d\r\n",c);
            dat.color = (ColorDef)c;
            led_disp_reinit();
        }
        return 0;
    }

    if(argc == 5)
    {
        const char *cx = argv[1];
        const char *cy = argv[2];
        const char *cc = argv[3];
        const char *cv = argv[4];

        x = atoi((const char *)cx);
        y = atoi((const char *)cy);
        c = atoi((const char *)cc);
        v = atoi((const char *)cv);

        rt_kprintf("%d,%d,%d,%d ",x,y,c,v);

        if(v <= 10)
        {
            pNum = &num[v << 4];
            disp8x16(x,y,pNum,(ColorDef)c);
            rt_kprintf("num\r\n");
        }
        return 0;
    }
    return 0;
}
FINSH_FUNCTION_EXPORT(led, led test);
MSH_CMD_EXPORT(led, led test);

//////////////////////////////以下测试/////////////////////////////////
static rt_mutex_t mutex_move = RT_NULL;
static rt_event_t event_time = RT_NULL;

static void led_thread_entry(void *parameter)
{
    while(1)
    {
        rt_mutex_take(mutex_move, RT_WAITING_FOREVER);
        move_effect((ColorDef)get_color());
        rt_mutex_release(mutex_move);
        rt_thread_delay(RT_TICK_PER_SECOND/100);

        if(0 == gpio_get(led1_gpio))
        {
            // led
            gpio_set (led1_gpio    , gpio_level_high); 
        }
        else if(1 == gpio_get(led1_gpio))
        {
            // led
            gpio_set (led1_gpio    , gpio_level_low); ;
        }

    }
}

void test_led_start(void)
{
    rt_thread_t tid_shift;
    int i;
    GPIO_Init();
    color_init();
    Timer_Init(1, 200, 190); 
    /* create thread */
    tid_shift = rt_thread_create("ledmatrix", led_thread_entry, RT_NULL,
    
                                    1024, 6, 20);
    for(i=0;i<7;i++)
    {
        disp_background(0,i);
        rt_thread_delay(400);
    }
    disp_background(0,0);

    mutex_move = rt_mutex_create("mutex_m", RT_IPC_FLAG_FIFO);
    RT_ASSERT(mutex_move != RT_NULL);

    /* creat event */
    event_time = rt_event_create("event_time", RT_IPC_FLAG_FIFO);
    RT_ASSERT(event_time != RT_NULL);

    rt_thread_startup(tid_shift) ;                              
    RT_ASSERT(tid_shift != RT_NULL);

}
MSH_CMD_EXPORT(test_led_start, gpio init and led open);
