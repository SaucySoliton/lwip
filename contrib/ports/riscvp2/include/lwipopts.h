#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

// #define SYS_ARCH_PROTECT            0
// #define SSIZE_MAX                   1
#define MEMP_MEM_MALLOC      1  

//#define LCP_CILEN_PATCH 1
//#define PATCH_IPHDR_IN 1
#define NO_SYS 1
// #define PPP_FCS_TABLE 1
//#define MEM_LIBC_MALLOC             1
//#define MEMP_MEM_MALLOC             1
// https://www.avrfreaks.net/forum/lwip-build-problem-when-defining-memusepools
//  #define MEM_USE_POOLS 1
//  #define MEMP_USE_CUSTOM_POOLS 1
#define PPP_SUPPORT 1
#define PPPOS_SUPPORT 1
#define PPP_IPV4_SUPPORT 1
#define LWIP_IPV4 1
#define LWIP_CALLBACK_API 1
//#define PPP_AUTH_SUPPORT 1
#define PPP_SERVER 1
// #define VJ_SUPPORT 0  
#define MEM_ALIGNMENT               4
// #define LWIP_SKIP_PACKING_CHECK 1
#define MEM_SIZE                    (32 * 1024)
// #define MEMP_NUM_PBUF               20
// #define MEMP_NUM_UDP_PCB            20
// #define MEMP_NUM_TCP_PCB            20
// #define MEMP_NUM_TCP_PCB_LISTEN     16
// #define MEMP_NUM_TCP_SEG            128
// #define MEMP_NUM_REASSDATA          32
// #define MEMP_NUM_ARP_QUEUE          10
// #define MEMP_NUM_TCPIP_MSG_API          8  //KAW
// #define PBUF_POOL_SIZE              512
// #define LWIP_ARP                    1
// #define IP_REASS_MAX_PBUFS          64
// #define IP_FRAG_USES_STATIC_BUF     0
// #define IP_DEFAULT_TTL              255
// #define IP_SOF_BROADCAST            1
// #define IP_SOF_BROADCAST_RECV       1
#define LWIP_ICMP                   1
#define CHECKSUM_CHECK_ICMP    1
//#define LWIP_BROADCAST_PING         1
//#define LWIP_MULTICAST_PING         1
#define LWIP_RAW                    1
#define LWIP_UDP                    1
#define LWIP_DNS                    1
#define LWIP_TCP                    1
// #define TCP_WND                     (4 * TCP_MSS)
// #define TCP_MSS                     1460
// #define TCP_SND_BUF                 (8 * TCP_MSS)
/// #define TCP_LISTEN_BACKLOG          1
// #define LWIP_NETIF_STATUS_CALLBACK  1
// #define LWIP_NETIF_LINK_CALLBACK    1
// #define LWIP_NETIF_HWADDRHINT       1
#define LWIP_NETCONN                0
#define LWIP_SOCKET                 0
// #define LWIP_STATS_DISPLAY          1
#define MEM_STATS                   0
#define SYS_STATS                   0
#define MEMP_STATS                  0
#define LINK_STATS                  0
#define ETHARP_TRUST_IP_MAC         0
// #define ETH_PAD_SIZE                2
// #define LWIP_CHKSUM_ALGORITHM       2

// #define LWIP_TCP_KEEPALIVE          1

// Keepalive values, compliant with RFC 1122. Don't change this unless you know what you're doing
#define TCP_KEEPIDLE_DEFAULT        10000UL // Default KEEPALIVE timer in milliseconds
#define TCP_KEEPINTVL_DEFAULT       2000UL  // Default Time between KEEPALIVE probes in milliseconds
#define TCP_KEEPCNT_DEFAULT         9U      // Default Counter for KEEPALIVE probes

//#include "mch.h"
#include "arch/sys_arch.h"
// void mem_init(void)
// {
// 	return;
// }
//#define mem_init()
// #define mem_free                    free
// #define mem_malloc                  malloc
// #define mem_calloc(c, n)            calloc(c , n)
// #define mem_realloc(p, sz)          realloc(p,sz)

#define LWIP_DEBUG                  1
//#define PRINTPKT_SUPPORT 1
#define ETHARP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                  LWIP_DBG_OFF
#define API_LIB_DEBUG               LWIP_DBG_OFF
#define API_MSG_DEBUG               LWIP_DBG_OFF
#define SOCKETS_DEBUG               LWIP_DBG_OFF
#define ICMP_DEBUG                  LWIP_DBG_OFF
#define INET_DEBUG                  LWIP_DBG_OFF
#define IP_DEBUG                    LWIP_DBG_OFF
#define IP_REASS_DEBUG              LWIP_DBG_OFF
#define RAW_DEBUG                   LWIP_DBG_OFF
#define MEM_DEBUG                   LWIP_DBG_ON
#define MEMP_DEBUG                  LWIP_DBG_ON
#define SYS_DEBUG                   LWIP_DBG_OFF
#define TCP_DEBUG                   LWIP_DBG_OFF
#define TCP_INPUT_DEBUG             LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
#define TCP_RTO_DEBUG               LWIP_DBG_OFF
#define TCP_CWND_DEBUG              LWIP_DBG_OFF
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#define UDP_DEBUG                   LWIP_DBG_OFF
#define TCPIP_DEBUG                 LWIP_DBG_OFF
#define PPP_DEBUG                   LWIP_DBG_ON
#define SLIP_DEBUG                  LWIP_DBG_OFF
#define DHCP_DEBUG                  LWIP_DBG_OFF

#endif /* __LWIPOPTS_H__ */
