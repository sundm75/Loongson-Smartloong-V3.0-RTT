#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 10
#define RT_ALIGN_SIZE 8
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_DEBUG

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_SIGNALS

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_INTERRUPT_INFO
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart2"
#define RT_VER_NUM 0x40000

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 6
#define DFS_FILESYSTEM_TYPES_MAX 8
#define DFS_FD_MAX 32
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 936
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_0
#define RT_DFS_ELM_USE_LFN 0
#define RT_DFS_ELM_MAX_LFN 64
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_USING_CAN
#define RT_CAN_USING_HDR
#define RT_USING_I2C
#define RT_USING_PIN
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_SPI
#define RT_USING_SPI_MSD

/* Using WiFi */


/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_PTHREADS
#define RT_USING_POSIX

/* Network */

/* Socket abstraction layer */

#define RT_USING_SAL

/* protocol stack implement */

#define SAL_USING_LWIP
#define SAL_USING_POSIX
#define SAL_PROTO_FAMILIES_NUM 4

/* light weight TCP/IP stack */

#define RT_USING_LWIP
#define RT_USING_LWIP141
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_DNS

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "193.169.2.230"
#define RT_LWIP_GWADDR "193.169.2.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 4
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 3
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 4096
#define RT_LWIP_TCP_WND 2048
#define RT_LWIP_TCPTHREAD_PRIORITY 12
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 4096
#define RT_LWIP_ETHTHREAD_PRIORITY 14
#define RT_LWIP_ETHTHREAD_STACKSIZE 512
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
#define LWIP_NETIF_STATUS_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_NETIF_LOOPBACK 0
#define RT_LWIP_DEBUG
#define RT_LWIP_ICMP_DEBUG

/* Modbus master and slave stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread RTGUI */


/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_WEBCLIENT
#define WEBCLIENT_NOT_USE_TLS
#define PKG_USING_WEBCLIENT_LATEST_VERSION
#define PKG_USING_CJSON
#define PKG_USING_CJSON_LATEST_VERSION
#define PKG_USING_EZXML
#define PKG_USING_EZXML_LATEST_VERSION

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */

#define PKG_USING_NETUTILS
#define PKG_NETUTILS_PING
#define PKG_USING_NETUTILS_LATEST_VERSION

/* IoT Cloud */


/* security packages */


/* language packages */

#define PKG_USING_JERRYSCRIPT
#define PKG_USING_JERRYSCRIPT_LATEST_VERSION
#define PKG_JMEM_HEAP_SIZE 512
#define PKG_JERRY_ENABLE_ERROR_MESSAGES
#define PKG_JERRY_ENABLE_LOGGING
#define PKG_JMEM_STATS
#define PKG_CONFIG_DISABLE_ES2015
#define PKG_USING_MICROPYTHON

/* Hardware Module */


/* System Module */


/* Tools Module */


/* Network Module */

#define PKG_MICROPYTHON_HEAP_SIZE 8192
#define PKG_USING_MICROPYTHON_LATEST_VERSION

/* multimedia packages */


/* tools packages */


/* system packages */


/* peripheral libraries and drivers */


/* miscellaneous packages */


/* samples: kernel and components samples */

#define PKG_USING_NETWORK_SAMPLES
#define PKG_USING_NETWORK_SAMPLES_LATEST_VERSION
#define NETWORK_SAMPLES_USING_TCP_CLIENT
#define NETWORK_SAMPLES_USING_TCP_SERVER
#define NETWORK_SAMPLES_USING_UDP_CLIENT
#define NETWORK_SAMPLES_USING_UDP_SERVER
#define NETWORK_SAMPLES_USING_TCP_CLIENT_SELECT
#define RT_USING_UART2
#define RT_USING_UART1
#define RT_UART_RX_BUFFER_SIZE 64
#define RT_USING_GMAC_INT_MODE
#define RT_USING_FPU
#define RT_USING_SPI0
#define RT_USING_SPI1
#define RT_USING_I2C2
#define USING_BXCAN0
#define USING_BXCAN1
#define NO_TOUCH
#define RT_RTC_NAME "rtc"

#endif
