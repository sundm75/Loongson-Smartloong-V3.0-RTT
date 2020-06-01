/*
 * File      :cbus_dump.c
导出利用状态
*/
#include <rtthread.h>
#include <stdint.h>
#include <string.h>
#include "ls1c_public.h"
#include "ls1c_gpio.h"
#include "ls1c_delay.h"
#include "ls1c_regs.h"
#include "ls1c_pin.h"

int  cbus_dump(void)
{
    int i;
    unsigned int dat;
    unsigned long pll_rate = 0;

    for(int i1 = 0 ; i1 < 5 ; i1 ++) { //每个gpio5个功能的定义分别是bfd0110c0,bfd0110d0,bfd0110e0,bfd0110f0,bfd011200开始的16个字节，
		printf("\nfun%d[%08x]:",i1 + 1,LS1C_CBUS_FIRST0 + i1 * 0x10);
		for(int i = 0 ; i < 0x4 ; i ++) {
            dat = reg_read_32((volatile unsigned int *)((LS1C_CBUS_FIRST0 + i1 * 0x10) + i*4));
			printf(" %02x  %02x  %02x  %02x |",(unsigned char) dat&0xff,(unsigned char)(dat>>8)&0xff,(unsigned char)(dat>>16)&0xff,(unsigned char)(dat>>24)&0xff);
		}
	}
    printf("\r\n");
    
    return 0;
}


uint32_t hextol(char * str) {
	uint8_t i=0,l;
	uint32_t ret=0;
	l=strlen(str);
	for(i=0;i<l;i++) str[i]=str[i]|0x20; //'A-Z' to 'a-z'
	for(i=2;i<l;i++){
		ret=(ret<<4);
		if(str[i]>'9') ret|=(str[i]-'a'+10);
		else ret|=(str[i]&0xf);
	}
	return ret;
}

int reg_read(int argc, char *argv[])
{
	uint32_t addr_base;
	uint32_t dat;

	if(argc!=2) 
    {
		printf("regs_read 0xbfd011c0\r\n");
		return (-2);
	}
	addr_base=hextol(argv[1]);

	dat=reg_read_32((volatile unsigned int *)(addr_base));
	printf("%08x\r\n",dat);
	return (0);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(reg_read, reg_read 0xbfd011c0 );


int reg_write(int argc, char *argv[])
{
	uint32_t addr_base;
	uint32_t dat;

  	if(argc!=3) 
    {
		printf("regs_read 0xbfd011c0\r\n");
		return (-2);
	}
	addr_base=hextol(argv[1]);
    dat = hextol(argv[2]);

	reg_write_32(dat, (volatile unsigned int *)(addr_base));
	printf("* %08x = %08x\r\n",dat, addr_base);
	return (0);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(reg_write, reg_write 0xbfd011c0 0x03 );
MSH_CMD_EXPORT(cbus_dump, cbus_dump  );