#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_
#include "board.h"

#define LED_OE      68

#define OE_LED_DIS       gpio_set (LED_OE  , gpio_level_high); ;
#define OE_LED_EN        gpio_set (LED_OE  , gpio_level_low); ;

#define LINE_1 16
#define LINE_2 0

#define MOVE_BUFF 22

#define LED_BUFF_SIZE   768

typedef enum
{
    RED = 0,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    CYAN,
    WHITE,
    BLACK
}ColorDef;

#pragma anon_unions

typedef struct _dat_rgb_t
{
    union
    {
        unsigned char r[32][8];
        unsigned long r32[32][2];
    };

    union
    {
        unsigned char g[32][8];
        unsigned long g32[32][2];
    };

    union
    {
        unsigned char b[32][8];
        unsigned long b32[32][2];
    };
    
    unsigned char move_buf[16][MOVE_BUFF];  //display buff for move
    ColorDef color;    // 0~6 color
}dat_rgb_t;
extern dat_rgb_t dat;

typedef struct _disp_time_t
{
    unsigned char year;
    unsigned char mon;
    unsigned char day;
    unsigned char week;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
}disp_time_t;
extern disp_time_t disp_t;

void led_scan(void);
void GPIO_Init(void);
void disp_background(unsigned char index,ColorDef c);
void draw_pixel(unsigned char x, unsigned char y,ColorDef c);
void draw_line(unsigned char x, unsigned char y,unsigned char deltax, unsigned char deltay, ColorDef c);
void color_init(void);
void color_change(void);
ColorDef get_color(void);
void move_effect(ColorDef color);

#endif

