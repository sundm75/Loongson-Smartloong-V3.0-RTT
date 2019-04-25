#include <rtthread.h>
#include <rtdef.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

#include "morphdigit.h"
#include "ledmatrix.h"

#define SHIFT_TIME 5
#define COLUMN_1 0
#define COLUMN_2 24
#define COLUMN_3 47

#define sA  0
#define sB  1
#define sC  2
#define sD  3
#define sE  4
#define sF  5
#define sG  6
static const int segHeight = 5;
static const int segWidth = 5;
static unsigned char xOffset,yOffset;

static ColorDef m_color;
static rt_bool_t b_morph_reinit = 0;

static void draw_pixel_offset(unsigned char x, unsigned char y, ColorDef c)
{
    draw_pixel(xOffset + x, yOffset + y, c);
}

static void draw_line_offset(unsigned char x, unsigned char y,unsigned char deltax, unsigned char deltay, ColorDef c)
{
    draw_line(xOffset + x, yOffset + y, deltax, deltay, c);
}

//æ— offset
static void draw_seg(unsigned char seg)
{
    switch(seg)
    {
        case 0:
            draw_line_offset(2, segHeight * 2 + 3, segWidth, 0, m_color);
            break;
        case sB:
            draw_line_offset(segWidth + 2, segHeight + 3, 0, segHeight, m_color);
            break;
        case sC:
            draw_line_offset(segWidth + 2, 2, 0, segHeight, m_color);
            break;
        case sD:
            draw_line_offset(2, 1, segWidth, 0, m_color);
            break;
        case sE:
            draw_line_offset(1, 2, 0, segHeight, m_color);
            break;
        case sF:
            draw_line_offset(1, segHeight + 3, 0, segHeight, m_color);
            break;
        case sG:
            draw_line_offset(2, segHeight + 2, segWidth, 0, m_color);
            break;
        default:
            break;
    }
}

static void draw_num(unsigned char x, unsigned char y,unsigned char num)
{
    xOffset = x;
    yOffset = y;
    switch(num)
    {
        case 0: // ZERO
            draw_seg(sA);
            draw_seg(sB);
            draw_seg(sC);
            draw_seg(sD);
            draw_seg(sE);
            draw_seg(sF);
            break;

        case 1: // ONE
            draw_seg(sB);
            draw_seg(sC);
            break;

        case 2: // TWO
            draw_seg(sA);
            draw_seg(sB);
            draw_seg(sD);
            draw_seg(sE);
            draw_seg(sG);
            break;

        case 3: // THREE
            draw_seg(sA);
            draw_seg(sB);
            draw_seg(sC);
            draw_seg(sD);
            draw_seg(sG);
            break;

        case 4: // FOUR
            draw_seg(sB);
            draw_seg(sC);
            draw_seg(sF);
            draw_seg(sG);
            break;

        case 5: // FIVE
            draw_seg(sA);
            draw_seg(sC);
            draw_seg(sD);
            draw_seg(sF);
            draw_seg(sG);
            break;

        case 6: // SIX
            draw_seg(sA);
            draw_seg(sC);
            draw_seg(sD);
            draw_seg(sE);
            draw_seg(sF);
            draw_seg(sG);
            break;

        case 7: // SEVEN
            draw_seg(sA);
            draw_seg(sB);
            draw_seg(sC);
            break;

        case 8: // EIGHT
            draw_seg(sA);
            draw_seg(sB);
            draw_seg(sC);
            draw_seg(sD);
            draw_seg(sE);
            draw_seg(sF);
            draw_seg(sG);
            break;

        case 9: // NINE
            draw_seg(sA);
            draw_seg(sB);
            draw_seg(sC);
            draw_seg(sD);
            draw_seg(sF);
            draw_seg(sG);
            break;

        default:
            break;
    }
}

/*
9 -> 0
5 -> 0
*/
static void morph0(void)
{
    // ZERO
    int i;
    for ( i = 0; i < segWidth; i++)
    {
      // Flow G into E & B
      draw_pixel_offset(segWidth + 1 - i, segHeight + 2, BLACK);    //G
      draw_pixel_offset(1, segHeight + 1 - i, m_color);   //E
      draw_pixel_offset(segWidth + 2, segHeight * 2 + 2 - i, m_color);    //B
      rt_thread_delay(SHIFT_TIME);
    }
}

static void morph1(void)
{
    // Zero to One
    int i;
    for (i = 0; i <= segWidth; i++)
    {
        // Move F left to right
        //(1,            segHeight + 3, 0, segHeight, m_color)
        //(segWidth + 2, segHeight + 3, 0, segHeight, m_color)
        draw_line_offset(1 + i, segHeight + 3, 0, segHeight, BLACK);
        draw_line_offset(2 + i, segHeight + 3, 0, segHeight, m_color);

        // Move E left to right
        //(1,            2, 0, segHeight, m_color)
        //(segWidth + 2, 2, 0, segHeight, m_color)
        draw_line_offset(1 + i, 2, 0, segHeight, BLACK);
        draw_line_offset(2 + i, 2, 0, segHeight, m_color);

        // Gradually Erase A, G, D
        //(2, segHeight * 2 + 3, segWidth, 0, m_color)
        //(2, segHeight + 2,     segWidth, 0, m_color)
        //(2, 1,                 segWidth, 0, m_color)
        draw_pixel_offset(2 + i, 1,                 BLACK); // D
        draw_pixel_offset(2 + i, segHeight + 2,     BLACK); // G
        draw_pixel_offset(2 + i, segHeight * 2 + 3, BLACK); // A

        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph2(void)
{
    // TWO
    int i;
    for (i = 0; i <= segWidth; i++)
    {
        if (i < segWidth)
        {
            //gradually A D G
            //(2, segHeight * 2 + 3, segWidth, 0, m_color)
            //(2, segHeight + 2,     segWidth, 0, m_color)
            //(2, 1,                 segWidth, 0, m_color)
            draw_pixel_offset(segWidth + 1 - i, segHeight * 2 + 3, m_color);
            draw_pixel_offset(segWidth + 1 - i, segHeight + 2, m_color);
            draw_pixel_offset(segWidth + 1 - i, 1, m_color);
        }

    // Move C right to left
    //(segWidth + 2, 2, 0, segHeight, m_color)
    //(1, 2, 0, segHeight, m_color)
    draw_line_offset(segWidth + 2 - i, 2, 0, segHeight, BLACK);
    draw_line_offset(segWidth + 1 - i, 2, 0, segHeight, m_color);
    rt_thread_delay(SHIFT_TIME);
    }
}

static void morph3(void)
{
    // THREE
    int i;
    for (i = 0; i <= segWidth; i++)
    {
        // Move E left to right
        //(1,            2, 0, segHeight, m_color)
        //(segWidth + 2, 2, 0, segHeight, m_color)
        draw_line_offset(1 + i, 2, 0, segHeight, BLACK);
        draw_line_offset(2 + i, 2, 0, segHeight, m_color);
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph4(void)
{
    // FOUR
    int i;
    for (i = 0;  i < segWidth; i++)
    {
        //A (2, segHeight * 2 + 3, segWidth, 0, m_color)
        //F (1, segHeight + 3, 0, segHeight, m_color)
        //D (2, 1, segWidth, 0, m_color)
        draw_pixel_offset(segWidth + 1 - i, segHeight * 2 + 3, BLACK);  //erase A
        draw_pixel_offset(1, segHeight * 2 + 2 - i, m_color); // Draw as F
        draw_pixel_offset(2 + i, 1, BLACK);  //erase D
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph5(void)
{
    // FIVE
    int i;
    for (i = 0;  i < segWidth; i++)
    {
        //(segWidth + 2, segHeight + 3, 0, segHeight, m_color)
        //(2, 1, segWidth, 0, m_color)
        //(2, segHeight * 2 + 3, segWidth, 0, m_color)
        draw_pixel_offset(segWidth + 2, segHeight + 3 + i, BLACK);  // Erase B
        draw_pixel_offset(segWidth + 1 - i, 1, m_color);              // Draw as D
        draw_pixel_offset(segWidth + 1 - i, segHeight * 2 + 3, m_color);  // Draw A
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph6(void)
{
    // SIX
    int i;
    for (i = 0;  i <= segWidth; i++)
    {
        //Move C right to left
        //draw_line_offset(segWidth + 2 - i, 2, 0, segHeight, BLACK);
        //draw_line_offset(segWidth + 1 - i, 2, 0, segHeight, m_color);
        draw_line_offset(segWidth + 1 - i, 2, 0, segHeight, m_color);
        if (i > 0)
            draw_line_offset(segWidth + 2 - i, 2, 0, segHeight, BLACK);
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph7(void)
{
    // SEVEN
    int i;
    for (i = 0;  i <= segWidth; i++)
    {
        // Move F left to right
        //(1,            segHeight + 3, 0, segHeight, m_color)
        //(segWidth + 2, segHeight + 3, 0, segHeight, m_color)
        draw_line_offset(1 + i, segHeight + 3, 0, segHeight, BLACK);
        draw_line_offset(2 + i, segHeight + 3, 0, segHeight, m_color);

        // Move E left to right
        //(1,            2, 0, segHeight, m_color)
        //(segWidth + 2, 2, 0, segHeight, m_color)
        draw_line_offset(1 + i, 2, 0, segHeight, BLACK);
        draw_line_offset(2 + i, 2, 0, segHeight, m_color);

        // Erase D and G gradually
        draw_pixel_offset(2 + i, 1,                 BLACK); // D
        draw_pixel_offset(2 + i, segHeight + 2,     BLACK); // G
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph8(void)
{
    // EIGHT
    int i;
    for (i = 0;  i <= segWidth; i++)
    {
        //Move C right to left
        //draw_line_offset(segWidth + 2 - i, 2, 0, segHeight, BLACK);
        //draw_line_offset(segWidth + 1 - i, 2, 0, segHeight, m_color);
        draw_line_offset(segWidth + 1 - i, 2, 0, segHeight, m_color);
        if (i > 0)
            draw_line_offset(segWidth + 2 - i, 2, 0, segHeight, BLACK);

        // Move B right to left
        //B (segWidth + 2, segHeight + 3, 0, segHeight, m_color)
        //F (1,            segHeight + 3, 0, segHeight, m_color)
        draw_line_offset(segWidth + 1 - i, segHeight + 3, 0, segHeight, m_color);
        if (i > 0)
            draw_line_offset(segWidth + 2 - i, segHeight + 3, 0, segHeight, BLACK);

        // Gradually draw D and G
        if (i < segWidth)
        {
            //D (2, 1, segWidth, 0, m_color)
            //G (2, segHeight + 2, segWidth, 0, m_color)
            draw_pixel_offset(segWidth + 1 - i, segHeight + 2, m_color); // G
            draw_pixel_offset(segWidth + 1 - i, 1, m_color);  //gradually  D
        }
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph9(void)
{
    // NINE
    int i;
    for (i = 0; i <= segWidth; i++)
    {
        // Move E left to right
        //(1,            2, 0, segHeight, m_color)
        //(segWidth + 2, 2, 0, segHeight, m_color)
        draw_line_offset(1 + i, 2, 0, segHeight, BLACK);
        draw_line_offset(2 + i, 2, 0, segHeight, m_color);
        rt_thread_delay(SHIFT_TIME);
    }
}

static void morph(unsigned char x,unsigned char y,unsigned char newValue) {
    xOffset = x;
    yOffset = y;
    switch (newValue) {
        case 0:
            morph0();
            break;
        case 1:
            morph1();
            break;
        case 2:
            morph2();
            break;
        case 3:
            morph3();
            break;
        case 4:
            morph4();
            break;
        case 5:
            morph5();
            break;
        case 6:
            morph6();
            break;
        case 7:
            morph7();
            break;
        case 8:
            morph8();
            break;
        case 9:
            morph9();
            break;
        default:
            break;
    }
}

static void morph_init(disp_time_t *pdt)
{
    unsigned char tmp;

    tmp = pdt->hour / 10;
    draw_num(COLUMN_1, LINE_2, tmp);
    tmp = pdt->hour % 10;
    draw_num(COLUMN_1 + 8,LINE_2,tmp);

    tmp = pdt->min / 10;
    draw_num(COLUMN_2, LINE_2, tmp);
    tmp = pdt->min % 10;
    draw_num(COLUMN_2 + 8,LINE_2,tmp);

    tmp = pdt->sec / 10;
    draw_num(COLUMN_3, LINE_2, tmp);
    tmp = pdt->sec % 10;
    draw_num(COLUMN_3 + 8,LINE_2,tmp);
}

void morph_reinit(void)
{
    b_morph_reinit = 0;
}

void morph_update(disp_time_t *pdt,ColorDef c)
{
    static unsigned char dat_array[6];
    m_color = c;
    if(!b_morph_reinit)
    {
        b_morph_reinit = 1;

        disp_background(0,m_color);
        disp_background(1,m_color);

        morph_init(pdt);

        dat_array[0] = pdt->hour / 10;
        dat_array[1] = pdt->hour % 10;
        dat_array[2] = pdt->min / 10;
        dat_array[3] = pdt->min % 10;
        dat_array[4] = pdt->sec / 10;
        dat_array[5] = pdt->sec % 10;
        return;
    }

    if(dat_array[5] != (pdt->sec % 10))
    {
        dat_array[5] = pdt->sec % 10;
        morph(COLUMN_3 + 8,LINE_2,dat_array[5]);
    }
    if(dat_array[4] != (pdt->sec / 10))
    {
        dat_array[4] = pdt->sec / 10;
        morph(COLUMN_3,LINE_2,dat_array[4]);
    }

    if(dat_array[3] != (pdt->min % 10))
    {
        dat_array[3] = pdt->min % 10;
        morph(COLUMN_2 + 8,LINE_2,dat_array[3]);
    }
    if(dat_array[2] != (pdt->min / 10))
    {
        dat_array[2] = pdt->min / 10;
        morph(COLUMN_2,LINE_2,dat_array[2]);
    }

    if(dat_array[1] != (pdt->hour % 10))
    {
        dat_array[1] = pdt->hour % 10;
        morph(COLUMN_1 + 8, LINE_2,dat_array[1]);
    }
    if(dat_array[0] != (pdt->hour / 10))
    {
        dat_array[0] = pdt->hour / 10;
        morph(COLUMN_1,LINE_2,dat_array[0]);
    }
}

