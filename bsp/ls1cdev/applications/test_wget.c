/*
调用方法，在msh中运行以下命令
1. speak 开 rtt  //从网络得到 “开”的音频文件  rtt.mp3 存到SD卡上
*/
#include <rtthread.h>
#include  <finsh.h> 
#include <webclient.h>

int test_wget(int argc, char** argv)
{
    char *url = "http://183.193.243.90:9012/www/rt-thread.txt";
   char *fil = "/rtthread.txt";
    webclient_get_file(url, fil);
    return 0;
}
MSH_CMD_EXPORT(test_wget, my web download file);

int speak(int argc, char** argv)
{
    char url[512] = {'\0'};
    char fil[32]  = {'\0'};
    
    if (argc != 3)
    {
        rt_kprintf("speak words filename\n");
        return 0;
    }

    /* The first part of the voice link */
    char *urlhd = "http://tsn.baidu.com/text2audio?lan=zh&ctp=1&cuid=abcdxxx&tok=24.890e327245c548078b9c453d6d483dc3.2592000.1544565032.282335-14676469&tex=";//将***更换成自己的token,共70个
    
    /* The last part of the voice link */
    char *urltl = "&vol=9&per=0&spd=5&pit=5&aue=3";
    
    rt_kprintf("%s\n\n\n",argv[1]);
    
    /* Complete the link */
    strcat(url,urlhd);
    strcat(url,argv[1]);   /* What we want to speak */
    strcat(url,urltl);
    
    char *filhd = "/";     /* Path      */
    char *filtl = ".mp3";  /* File type */

    /* Complete the file name */
    strcat(fil,filhd);
    strcat(fil,argv[2]);
    strcat(fil,filtl);

    /* Get the voice back */
    webclient_get_file(url, fil);

    return 0;
}
MSH_CMD_EXPORT(speak, speak somthing  *** by rtt);
