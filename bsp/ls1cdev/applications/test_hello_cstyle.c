/*test_hello_cstyle.c*/
#include <finsh.h>
int var;
int hello_rtt(int a)
{
    rt_kprintf("Hello, world! I am %d, this is c-tyle OK \n", a++);
    var  = a;
    return a;
}
/*使用finsh的函数导出宏,导出成c-style的命令*/
FINSH_FUNCTION_EXPORT(hello_rtt, say hello to rtt)
FINSH_VAR_EXPORT(var, finsh_type_int, just a var for test)
/*使用finsh的函数导出宏,函数重新命名为hr*/
FINSH_FUNCTION_EXPORT_ALIAS(hello_rtt, hr, say hello to rtt)
/*使用finsh的函数导出宏,导出成msh的命令*/
FINSH_FUNCTION_EXPORT_ALIAS(hello_rtt, __cmd_hello_rtt, say hello to rtt- List information about the FILEs)
