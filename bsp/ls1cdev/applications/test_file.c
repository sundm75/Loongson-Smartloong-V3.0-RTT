#include <dfs_posix.h> /* 当需要使用文件操作时，需要包含这个头文件*/
/* 假设文件操作是在一个函数中完成 */
void test_file()
{
	int fd;
	char s[] = "RT-Thread Programmer!", buffer[80];
	/* 打开 /text.txt 作写入，如果该文件不存在则建立该文件 */
	fd = open("/text.txt", O_WRONLY | O_CREAT,0);
	if (fd >= 0)
	{
		write(fd, s, sizeof(s));
		close(fd);
	}
	/* 打开 /text.txt 准备作读取动作 */
	fd = open("/text.txt", O_RDONLY,0);
	if (fd >= 0)
	{
		read(fd, buffer, sizeof(buffer));
		close(fd);
	}
	rt_kprintf("%s", buffer);
}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_file, file test);