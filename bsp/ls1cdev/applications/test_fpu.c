// 硬浮点测试用例的源文件

#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"


// 每个测试用例中for循环的最大值
#define TEST_FPU_MAX_COUNT          (1000)


// 使用硬浮点执行浮点数的加法
void test_fpu_add(void)
{
    unsigned int i = 0;
    float sum_f = 0.0;
    unsigned int *sum_p = (unsigned int *)&sum_f;

    rt_kprintf("\n\n----------------------%s-------------------\n", __FUNCTION__);
    for (i=0; i<TEST_FPU_MAX_COUNT; i++)
    {
        sum_f += 0.62113;
        rt_kprintf("[%s] *sum_p=0x%x\n", __FUNCTION__, *sum_p);
    }

    return ;
}


// 使用硬浮点执行浮点数的减法
void test_fpu_subtraction(void)
{
    unsigned int i = 0;
    float result_f = 252.731;
    unsigned int *result_p = (unsigned int *)&result_f;

    rt_kprintf("\n\n----------------------%s-------------------\n", __FUNCTION__);
    for (i=0; i<TEST_FPU_MAX_COUNT; i++)
    {
        result_f -= 0.62113;
        rt_kprintf("[%s] *result_p=0x%x\n", __FUNCTION__, *result_p);
    }

    return ;
}


// 使用硬浮点执行浮点数的乘法
void test_fpu_multiplication(void)
{
    unsigned int i = 0;
    float result_f = 9.016;
    unsigned int *result_p = (unsigned int *)&result_f;

    rt_kprintf("\n\n----------------------%s-------------------\n", __FUNCTION__);
    for (i=1; i<TEST_FPU_MAX_COUNT; i++)
    {
        result_f *= 1.00001;
        rt_kprintf("[%s] *result_p=0x%x\n", __FUNCTION__, *result_p);
    }

    return ;
}


// 使用硬浮点执行浮点数的除法
void test_fpu_division(void)
{
    unsigned int i = 0;
    float result_f = 723.801;
    unsigned int *result_p = (unsigned int *)&result_f;

    rt_kprintf("\n\n----------------------%s-------------------\n", __FUNCTION__);
    for (i=1; i<TEST_FPU_MAX_COUNT; i++)
    {
        result_f /= 1.00003;
        rt_kprintf("[%s] *result_p=0x%x\n", __FUNCTION__, *result_p);
    }

    return ;
}



// 测试使用硬浮点进行浮点数的加减乘除
void test_fpu(void)
{
    // 使用硬浮点执行浮点数的加法
    test_fpu_add();

    // 使用硬浮点执行浮点数的减法
    test_fpu_subtraction();

    // 使用硬浮点执行浮点数的乘法
    test_fpu_multiplication();

    // 使用硬浮点执行浮点数的除法
    test_fpu_division();

    return ;
}

#include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_fpu, test_fpu  e.g.test_fpu());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_fpu, test_fpu);




