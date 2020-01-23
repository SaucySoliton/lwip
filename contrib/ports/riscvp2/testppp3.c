#include <stdio.h>
#include <stdlib.h>

#include <ping.h>
#include <lwip/apps/http_client.h>


#include "include/lwipopts.h"

#include "lwip/netif.h"
#include <netif/ppp/ppp.h>
#include <lwip/arch.h>


#include "sys_p2.h"

unsigned int max_rx_len=0;
unsigned int total_rx_len=0;

sys_prot_t sys_arch_protect(void) { return 0; }
void sys_arch_unprotect(sys_prot_t val)	{return;  }

// lwIP includes 
#include "lwip/dns.h"

#ifndef PPPOS_SUPPORT
#define PPPOS_SUPPORT 0
#endif /* PPPOS_SUPPORT */

#if PPPOS_SUPPORT
#include "netif/ppp/pppos.h"
#include "lwip/sio.h"
#define PPP_PTY_TEST 1
#endif /* PPPOS_SUPPORT */


#if PPPOS_SUPPORT
static sio_fd_t ppp_sio;
static ppp_pcb *ppp;
static struct netif pppos_netif;


int rx_escape=0;
int rx_count=0;

#define PPP_FCS_POLYNOMIAL 0x8408
static u16_t
ppp_get_fcs(u8_t byte)
{
  unsigned int octet;
  int bit;
  octet = byte;
  for (bit = 8; bit-- > 0; ) {
    octet = (octet & 0x01) ? ((octet >> 1) ^ PPP_FCS_POLYNOMIAL) : (octet >> 1);
  }
  return octet & 0xffff;
}
#define PPP_FCS(fcs, c) (((fcs) >> 8) ^ ppp_get_fcs(((fcs) ^ (c)) & 0xff))


unsigned int rx_fcs=0;

static void
pppos_rx_task(void)
{
  u32_t len;
  u8_t buffer[1280];
  u8_t rx_char;
  //LWIP_UNUSED_ARG(arg);
  int r;

#if 1
#if 1
  len=0;
  while( (r=sys_serial_getch() )!=-1 && len<1280 )
  {
	  buffer[len]=r&0xff;
//	  printf("%x.",buffer[len]);
	  len++;
  }
#else
  len=sys_serial_getbuf( buffer );
#endif
  for(r=0;r<len;r++)
  {
     if( 1 )
     {
    	     if (buffer[r]==0x7e)
	     {
		     if( rx_count==0 )
             {
			     //printf("PPP flag\n");
             }
		     else{
	        printf("## PPP RX %d bytes",rx_count);
		if( rx_fcs==0xf0b8 )
			printf("\n");
		else
			printf(" FCS bad\n");
		     }
		rx_count=0;
		rx_fcs=0xffff;
	     }
	     else
	     {
		
		     rx_count++;
	     
		  if( buffer[r]==0x7d)
		  {
			  rx_escape=1;
			  //printf("^");
		  }
		  else
		  {
			  if( rx_escape )
		  		rx_char=buffer[r]^0x20;
			  else
		  		rx_char=buffer[r];
			  rx_escape=0;
			  rx_fcs = PPP_FCS(rx_fcs,rx_char);
		  	  //printf("%02x.%04x\n",rx_char,rx_fcs);
		  }
            }
     }  }
  /* Please read the "PPPoS input path" chapter in the PPP documentation. */
      /* Pass received raw characters from PPPoS to be decoded through lwIP
       * TCPIP thread using the TCPIP API. This is thread safe in all cases
       * but you should avoid passing data byte after byte. */
  if(len>0)
  {
     pppos_input(ppp, buffer, len);  
     //printf("\n");
  }
  if(len>max_rx_len)
	  max_rx_len=len;
#else
  r=sys_serial_getch();
  buffer[0]=r;
  if( r!=-1 )
  {
     len=1;
     pppos_input(ppp, buffer, len);  
     if( 0 )
     {
    	     if (r==0x7e)
	     {
	        printf("\n");
	     }
		  if( buffer[0]==0x7d)
		  {
			  rx_escape=1;
		  }
		  else
		  {
			  if( rx_escape )
		  		printf("%02x.",buffer[0]-0x20);
			  else
		  		printf("%02x.",buffer[0]);
			  rx_escape=0;
		  }
     }
  }
  
  else
     len=0;
#endif
  total_rx_len+=len;
}

static void
ppp_link_status_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);

    switch(err_code) {
    case PPPERR_NONE:               /* No error. */
        {
#if LWIP_DNS
        const ip_addr_t *ns;
#endif /* LWIP_DNS */
        fprintf(stderr, "ppp_link_status_cb: PPPERR_NONE\n\r");
#if LWIP_IPV4
        fprintf(stderr, "   our_ip4addr = %s\n\r", ip4addr_ntoa(netif_ip4_addr(pppif)));
        fprintf(stderr, "   his_ipaddr  = %s\n\r", ip4addr_ntoa(netif_ip4_gw(pppif)));
        fprintf(stderr, "   netmask     = %s\n\r", ip4addr_ntoa(netif_ip4_netmask(pppif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
        fprintf(stderr, "   our_ip6addr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* LWIP_IPV6 */

#if LWIP_DNS
        ns = dns_getserver(0);
        fprintf(stderr, "   dns1        = %s\n\r", ipaddr_ntoa(ns));
        ns = dns_getserver(1);
        fprintf(stderr, "   dns2        = %s\n\r", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#if PPP_IPV6_SUPPORT
        fprintf(stderr, "   our6_ipaddr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
        }
        break;

    case PPPERR_PARAM:             /* Invalid parameter. */
        printf("ppp_link_status_cb: PPPERR_PARAM\n");
        break;

    case PPPERR_OPEN:              /* Unable to open PPP session. */
        printf("ppp_link_status_cb: PPPERR_OPEN\n");
        break;

    case PPPERR_DEVICE:            /* Invalid I/O device for PPP. */
        printf("ppp_link_status_cb: PPPERR_DEVICE\n");
        break;

    case PPPERR_ALLOC:             /* Unable to allocate resources. */
        printf("ppp_link_status_cb: PPPERR_ALLOC\n");
        break;

    case PPPERR_USER:              /* User interrupt. */
        printf("ppp_link_status_cb: PPPERR_USER\n");
        break;

    case PPPERR_CONNECT:           /* Connection lost. */
        printf("ppp_link_status_cb: PPPERR_CONNECT\n");
        break;

    case PPPERR_AUTHFAIL:          /* Failed authentication challenge. */
        printf("ppp_link_status_cb: PPPERR_AUTHFAIL\n");
        break;

    case PPPERR_PROTOCOL:          /* Failed to meet protocol. */
        printf("ppp_link_status_cb: PPPERR_PROTOCOL\n");
        break;

    case PPPERR_PEERDEAD:          /* Connection timeout. */
        printf("ppp_link_status_cb: PPPERR_PEERDEAD\n");
        break;

    case PPPERR_IDLETIMEOUT:       /* Idle Timeout. */
        printf("ppp_link_status_cb: PPPERR_IDLETIMEOUT\n");
        break;

    case PPPERR_CONNECTTIME:       /* PPPERR_CONNECTTIME. */
        printf("ppp_link_status_cb: PPPERR_CONNECTTIME\n");
        break;

    case PPPERR_LOOPBACK:          /* Connection timeout. */
        printf("ppp_link_status_cb: PPPERR_LOOPBACK\n");
        break;

    default:
        printf("ppp_link_status_cb: unknown errCode %d\n", err_code);
        break;
    }
}

static u32_t
ppp_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
  int i;
  int escape=0;
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(ctx);
  
  printf("\n");  printf("ppp send %dB ", len);
  for( i=0;i<len;i++)
  {
    // FullDuplexSerial2_tx(fds,data[i],0);
     sys_serial_putch(data[i]);
  }
  if (0) // (len<160 && len>0)
  {
	   for( i=0;i<len;i++)
	  {
		  if( data[i]==0x7d)
		  {
			  escape=1;
		  }
		  else
		  {
			  if( escape )
		  		printf("%02x_",(data[i]-0x20)&0xff);
			  else
		  		printf("%02x_",data[i]&0xff);
			  escape=0;
		  }
		  //printf("%x_",data[i]);
	  }
  }
  if (0) // (len<160 && len>0)
  {
	   for( i=0;i<len;i++)
	  {
		  printf("%02x_",data[i]&0xff);

	  }
  }
  printf("\n");
  return len;
  //return sio_write(ppp_sio, data, len);
}

#if LWIP_NETIF_STATUS_CALLBACK
static void
netif_status_callback(struct netif *nif)
{
  printf("PPPNETIF: %c%c%d is %s\n", nif->name[0], nif->name[1], nif->num,
         netif_is_up(nif) ? "UP" : "DOWN");
#if LWIP_IPV4
  printf("IPV4: Host at %s ", ip4addr_ntoa(netif_ip4_addr(nif)));
  printf("mask %s ", ip4addr_ntoa(netif_ip4_netmask(nif)));
  printf("gateway %s\n", ip4addr_ntoa(netif_ip4_gw(nif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
  printf("IPV6: Host at %s\n", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_HOSTNAME
  printf("FQDN: %s\n", netif_get_hostname(nif));
#endif /* LWIP_NETIF_HOSTNAME */
}
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#endif


void
pppos_example_init(void)
{

#if 1 //NO_SYS
  lwip_init();
  //test_init(NULL);
#else /* NO_SYS */
  err = sys_sem_new(&init_sem, 0);
  LWIP_ASSERT("failed to create init_sem", err == ERR_OK);
  LWIP_UNUSED_ARG(err);
  tcpip_init(test_init, &init_sem);
  /* we have to wait for initialization to finish before
   * calling update_adapter()! */
  sys_sem_wait(&init_sem);
  sys_sem_free(&init_sem);
#endif /* NO_SYS */

#if PPPOS_SUPPORT
#if PPP_PTY_TEST
//  ppp_sio = sio_open(2);
#else
//  ppp_sio = sio_open(0);
#endif
  //if(!ppp_sio)
  //{
  //    perror("PPPOS example: Error opening device");
  //    return;
  //}

  ppp = pppos_create(&pppos_netif, ppp_output_cb, ppp_link_status_cb, NULL);
  if (!ppp)
  {
      printf("PPPOS example: Could not create PPP control interface");
      return;
  }

#ifdef LWIP_PPP_CHAP_TEST
  ppp_set_auth(ppp, PPPAUTHTYPE_CHAP, "lwip", "mysecret");
#endif

/*
 * Initiate PPP client connection
 * ==============================
 */

/* Set this interface as default route */
ppp_set_default(ppp);

/*
 * Basic PPP client configuration. Can only be set if PPP session is in the
 * dead state (i.e. disconnected). We don't need to provide thread-safe
 * equivalents through PPPAPI because those helpers are only changing
 * structure members while session is inactive for lwIP core. Configuration
 * only need to be done once.
 */

/* Ask the peer for up to 2 DNS server addresses. */
// ppp_set_usepeerdns(ppp, 1);

/* Auth configuration, this is pretty self-explanatory */
//ppp_set_auth(ppp, PPPAUTHTYPE_ANY, "login", "password");

/*
 * Initiate PPP negotiation, without waiting (holdoff=0), can only be called
 * if PPP session is in the dead state (i.e. disconnected).
 */
u16_t holdoff = 0;


/* Set our address */
//IP4_ADDR(&addr, 192,168,20,1);
//ppp_set_ipcp_ouraddr(ppp, &addr);

/* Set peer(his) address */
//IP4_ADDR(&addr, 192,168,20,2);
//ppp_set_ipcp_hisaddr(ppp, &addr);

ppp_set_usepeerdns(ppp, 1);
// "Assertion "cilen == p->len - HEADERLEN - PPP_HDRLEN" failed at line 759 in ../../../lwip/src/netif/ppp/fsm.c"
// mean that peer is not supplying DNS

//ppp_set_silent(ppp, 1);

  ppp_connect(ppp, holdoff);
  //ppp_listen(ppp);

#if LWIP_NETIF_STATUS_CALLBACK
  netif_set_status_callback(&pppos_netif, netif_status_callback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */

  //sys_thread_new("pppos_rx_thread", pppos_rx_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
#endif /* PPPOS_SUPPORT */

}


#if LWIP_TCP && LWIP_CALLBACK_API
static err_t
netio_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  printf("netio_rx\n");
  if (err == ERR_OK && p != NULL) {
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
  } else {
    pbuf_free(p);
  }

  if (err == ERR_OK && p == NULL) {
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_close(pcb);
  }

  return ERR_OK;
}

static err_t
netio_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  if (pcb != NULL) {
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, netio_recv);
  }
  return ERR_OK;
}

void
netio_init(void)
{
  struct tcp_pcb *pcb;

  pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
  tcp_bind(pcb, IP_ANY_TYPE, 18767);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, netio_accept);
}
#endif /* LWIP_TCP && LWIP_CALLBACK_API */


int gen_callback( void )
{
	printf("callback\n");
	return 0;
}
int httpc_r( void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err )
{
	printf("result %d\n", httpc_result);
	return 0;
}
int httpc_h( void )
{
	printf("headers\n");
	return 0;
}

struct tcp_pcb *testpcb;


uint32_t tcp_send_packet(void)
{
	int error;
    char *string = "GET / HTTP/1.0\r\n\r\n ";
    uint32_t len = strlen(string);

    /* push to buffer */
        error = tcp_write(testpcb, string, strlen(string), TCP_WRITE_FLAG_COPY);

    if (error) {
        printf("ERROR: Code: %d (tcp_send_packet :: tcp_write)\n", error);
        return 1;
    }

    /* now send */
    error = tcp_output(testpcb);
    if (error) {
        printf("ERROR: Code: %d (tcp_send_packet :: tcp_output)\n", error);
        return 1;
    }
    return 0;
}


/* connection established callback, err is unused and only return 0 */
err_t connectCallback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    printf("Connection Established.\n");
    printf("Now sending a packet\n");
    tcp_send_packet();
    return 0;
}

err_t tcpRecvCallback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    printf("Data recieved.\n");
    if (p == NULL) {
        printf("The remote host closed the connection.\n");
        printf("Now I'm closing the connection.\n");
        //tcp_close_con();
        return ERR_ABRT;
    } else {
        printf("Number of pbufs %d\n", pbuf_clen(p));
        printf("Contents of pbuf %s\n", (char *)p->payload);
    }

    return 0;
}

void tcp_setup(void)
{
    uint32_t data = 0xdeadbeef;

    /* create an ip */
    ip4_addr_t ip;
    IP4_ADDR(&ip, 192,168,20,1);    //IP of my PHP server

    /* create the control block */
    testpcb = tcp_new();    //testpcb is a global struct tcp_pcb
                            // as defined by lwIP


    /* dummy data to pass to callbacks*/

    tcp_arg(testpcb, &data);

    /* register callbacks with the pcb */

    //tcp_err(testpcb, tcpErrorHandler);
    tcp_recv(testpcb, tcpRecvCallback);
    //tcp_sent(testpcb, tcpSendCallback);

    /* now connect */
    tcp_connect(testpcb, &ip, 80, connectCallback);

}
void main()
{

    char b[16];

    sys_init();
    unsigned int triggertime,currenttime;
    int i = 0;
    int r;
    printf("starting pppos\n");
    pppos_example_init();
    printf("back to main\n");

/* Set our address */

    currenttime=sys_now();
    triggertime=currenttime;
    //FullDuplexSerial2_str(fds,"Hello World\n",0);
    //
    httpc_connection_t hct;
    httpc_state_t *hst;
    //hst=&hsts;
    hct.use_proxy = 0;
    hct.result_fn = httpc_r;
    hct.headers_done_fn = httpc_h;

#if 1 
    ip4_addr_t pingaddr;
    IP4_ADDR(&pingaddr, 192,168,20,1);
    //ping_init(&pingaddr);
#endif
    httpd_init();
#if 1
    tcpecho_raw_init();
    netio_init();
#endif
    int once=1;
    for(;;)
    {
#if NO_SYS
       /* handle timers (already done in tcpip.c when NO_SYS=0) */
       sys_check_timeouts();
       pppos_rx_task();
       //rand();

#endif /* NO_SYS */
       currenttime=sys_now();
       if( currenttime > triggertime+10000 )
       {
	       triggertime=currenttime;
	       printf("ppp rx %d, max chunk %d\n",total_rx_len,max_rx_len);
	       max_rx_len=0;
	       total_rx_len=0;
	       if (once)
	       {
		       printf("starting httpc_get_file\n");
		//printf("%d\n",httpc_get_file(&pingaddr,80,"/",&hct,gen_callback,NULL,&hst) );
		once=0;
		tcp_setup();
	       }

       }
       else if( currenttime < triggertime ) // try to handle rollover here
       {
	       triggertime=currenttime;
	       printf("time rollover\n");
       }
    }// main loop

}
