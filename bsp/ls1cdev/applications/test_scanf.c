#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sscanf_test(void)
{
    int ret;
    char *string;
    int  digit;
    char buf1[255];
    char buf2[255];
    char buf3[255];
    char buf4[255];

    /*1.最简单的用法*/
    string = "china beijing 123";
    ret = sscanf(string, "%s %s %d", buf1, buf2, &digit);
    printf("1.string=%s\n", string);
    printf("1.ret=%d, buf1=%s, buf2=%s, digit=%d\n\n", ret, buf1, buf2, digit);
    /*
    **执行结果:
    **1.ret=3, buf1=china, buf2=beijing, digit=123
    **可以看出,sscanf的返回值是读取的参数个数
    */

    /*2.取指定长度的字符串*/
    string = "123456789";
    sscanf(string, "%5s", buf1);
    printf("2.string=%s\n", string);
    printf("2.buf1=%s\n\n", buf1);
    /*
    **执行结果:
    **2.buf1=12345
    */

    /*3.取到指定字符为止的字符串*/
    string = "123/456";
    sscanf(string, "%[^/]", buf1);
    printf("3.string=%s\n", string);
    printf("3.buf1=%s\n\n", buf1);
    /*
    **执行结果:
    **3.buf1=123
    */

    /*4.取到指定字符集为止的字符串*/
    string = "123abcABC";
    sscanf(string, "%[^A-Z]", buf1);
    printf("4.string=%s\n", string);
    printf("4.buf1=%s\n\n", buf1);
    /*
    **执行结果:
    **4.buf1=123abc
    */

    /*5.取仅包含指定字符集的字符串*/
    string = "0123abcABC";
    sscanf(string, "%[0-9]%[a-z]%[A-Z]", buf1, buf2, buf3);
    printf("5.string=%s\n", string);
    printf("5.buf1=%s, buf2=%s, buf3=%s\n\n", buf1, buf2, buf3);
    /*
    **执行结果:
    **5.buf1=123, buf2=abc, buf3=ABC
    */

    /*6.获取指定字符中间的字符串*/
    string = "ios<android>wp7";
    sscanf(string, "%*[^<]<%[^>]", buf1);
    printf("6.string=%s\n", string);
    printf("6.buf1=%s\n\n", buf1);
    /*
    **执行结果:
    **6.buf1=android
    */

    /*7.指定要跳过的字符串*/
    string = "iosVSandroid";
    sscanf(string, "%[a-z]VS%[a-z]", buf1, buf2);
    printf("7.string=%s\n", string);
    printf("7.buf1=%s, buf2=%s\n\n", buf1, buf2);
    /*
    **执行结果:
    **7.buf1=ios, buf2=android
    */

    /*8.分割以某字符隔开的字符串*/
    string = "android-iphone-wp7";
    /*
    **字符串取道'-'为止,后面还需要跟着分隔符'-',
    **起到过滤作用,有点类似于第7点
    */
    sscanf(string, "%[^-]-%[^-]-%[^-]", buf1, buf2, buf3);
    printf("8.string=%s\n", string);
    printf("8.buf1=%s, buf2=%s, buf3=%s\n\n", buf1, buf2, buf3);
    /*
    **执行结果:
    **8.buf1=android, buf2=iphone, buf3=wp7
    */

    /*9.提取邮箱地址*/
    string = "Email:beijing@sina.com.cn";
    sscanf(string, "%[^:]:%[^@]@%[^.].%s", buf1, buf2, buf3, buf4);
    printf("9.string=%s\n", string);
    printf("9.buf1=%s, buf2=%s, buf3=%s, buf4=%s\n\n", buf1, buf2, buf3, buf4);
    /*
    **执行结果:
    **9.buf1=Email, buf2=beijing, buf3=sina, buf4=com.cn
    */

    /*10.过滤掉不想截取或不需要的字符串--补充,
    **在%号后面加一*号,代表过滤这个字符串,不读取
    */
    string = "android iphone wp7";
    sscanf(string, "%s %*s %s", buf1, buf2);
    printf("10.string=%s\n", string);
    printf("10.buf1=%s, buf2=%s\n\n", buf1, buf2);
    /*
    **执行结果:
    **10.android wp7
    */

    /*11.重新组合,
    **
    */
    rt_uint8_t mac_addr[6];
    string = "a0:20:a6:0d:64:2a";
    sscanf(string, "%x:%x:%x:%x:%x:%x", (rt_uint32_t *)&mac_addr[0], (rt_uint32_t *)&mac_addr[1], (rt_uint32_t *)&mac_addr[2], (rt_uint32_t *)&mac_addr[3], (rt_uint32_t *)&mac_addr[4], (rt_uint32_t *)&mac_addr[5]);
    for(int i = 0; i<6; i++)
    {
        printf("mac_addr[%d] = %x", i, mac_addr[i]);
    }
    printf("11.float = %f\n", 3.145926);

    /*
    **执行结果:
    **11.android wp7
    */
}

#ifdef RT_USING_FINSH
MSH_CMD_EXPORT(sscanf_test, sscanf_test);
#endif