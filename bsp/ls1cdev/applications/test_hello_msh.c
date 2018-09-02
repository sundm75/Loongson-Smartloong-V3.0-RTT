#include <finsh.h>

int hello_rtt_msh(void)
{
    rt_kprintf("Hello, world!  this is msh\n");
    return 0;
}
MSH_CMD_EXPORT(hello_rtt_msh, my command test);


int mycmdarg(int argc, char** argv)
{
    rt_kprintf("argv[0]: %s\n", argv[0]);

    if (argc > 1)
        rt_kprintf("argv[1]: %s\n", argv[1]);

    return 0;
}
MSH_CMD_EXPORT(mycmdarg, my command with args);