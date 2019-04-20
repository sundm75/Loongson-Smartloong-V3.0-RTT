import os

# CPU options
ARCH='mips'
CPU ='loongson_1c'
# toolchains options
CROSS_TOOL  = 'gcc'

# 使用硬浮点必须在 .config 中定义 RT_USING_DFP;
# 普通工程(不用C++)用  mipsel-none-elf- 可用 软浮点 + devfs ;可用 硬浮点 + devfs; 只可用 gcc 链接 ;不可定义 RT_USING_CPLUSPLUS ; 显示gui demo 不可用RT_USING_DFP
#PLATFORM_SEL = 'cc' 
# 柿饼工程(用C++)用  mips-sde-elf- 可用 软浮点 + devfs ; 不可用硬浮点+ devfs (如果用了就卡死); 只能用 g++ 链接;
PLATFORM_SEL = 'c++'    

if os.getenv('RTT_CC'):
	CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
else:
    print('================ERROR===========================')
    print('Not support %s yet!' % CROSS_TOOL)
    print('=================================================')
    exit(0)

if PLATFORM_SEL == 'cc':
    EXEC_PATH   = r'C:\mipsel-none-elf-win\bin'
    BUILD  = 'debug'
    PREFIX  = 'mipsel-none-elf-'
    LINK    = PREFIX + 'gcc'
	
if PLATFORM_SEL == 'c++':
    EXEC_PATH   = r'C:\mips-2016.05\bin'
    BUILD  = 'releases'
    PREFIX  = 'mips-sde-elf-'
    LINK    = PREFIX + 'g++'

#if os.getenv('RTT_EXEC_PATH'):
#    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

CC      = PREFIX + 'gcc'
CXX     = PREFIX + 'g++'
AS      = PREFIX + 'gcc'
AR      = PREFIX + 'ar'
RANLIB = PREFIX + 'ranlib'

TARGET_EXT = 'elf'
SIZE    = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY  = PREFIX + 'objcopy'
STRIP   = PREFIX + 'strip'
READELF = PREFIX + 'readelf'
DEVICE = ' -mips32r2 -msoft-float -mfp32'
CFLAGS  = DEVICE + ' -EL -G0 -mno-abicalls -fno-pic -fno-builtin -fno-exceptions -ffunction-sections -fomit-frame-pointer'
AFLAGS  = ' -c' + DEVICE + ' -EL -fno-pic -fno-builtin -mno-abicalls -x assembler-with-cpp  -DSYSTEM_STACK=0x80003fe8'
LFLAGS  = DEVICE + ' -nostartfiles -EL -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,Reset_Handler -T ls1c_ram.lds'
CPATH   = ''
LPATH   = ''


if BUILD == 'debug':
    CFLAGS += ' -O0 -gdwarf-2'
    AFLAGS += ' -gdwarf-2'
else:
    CFLAGS += ' -O2'

CXXFLAGS = CFLAGS
DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtt.asm\n'
READELF_ACTION = READELF + ' -a $TARGET > rtt.map\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
