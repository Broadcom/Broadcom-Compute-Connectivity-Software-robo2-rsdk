/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * Filename:
 *    pktio_wrapper.c
 * Description:
 *    Robo2 Packet IO wrapper for user mode access
 */


#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/version.h>

#include <linux/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>

#include <soc/drv.h>
#include <soc/robo2/bcm53158/init.h>
#include <fsal/cosq.h>
#include <fsal/pktio.h>
#include <fsal_int/pkt.h>
#include <osal/sal_console.h>
#include <osal/sal_alloc.h>
#include <osal/sal_util.h>
#include <osal/sal_time.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

#define USER_PKTIO_DEFAULT_IFACE "eth0"

#define MAX_PKTIO_SOCKETS 1


/* PacketIO Socket  */
typedef struct {
    sal_thread_t   taskhdl;
    int            unit;
    int            sockfd;
    int            tx_cnt;
    int            tx_err;
    int            tx_drop;
    int            rx_cnt;
    int            rx_err;
    int            rx_drop;
    int            ifindex;
    char           macsa[6];
    volatile uint8 thread_stop;
    volatile uint8 thread_drop;
} pktio_cb_t;

pktio_cb_t _sys_sockets[MAX_PKTIO_SOCKETS];

 /*
  * Function:
 *     packetio_dump_stats
 * Description:
 *     print stats
 */
void packetio_dump_stats(void)
{
    int i;
    uint64 value;
    for (value = 0, i = 0; i < MAX_PKTIO_SOCKETS; i++)
        value += _sys_sockets[i].tx_cnt;
    sal_printf("Packetio Tx Ok   : %d \n", value);
    for (value = 0, i = 0; i < MAX_PKTIO_SOCKETS; i++)
        value += _sys_sockets[i].tx_drop;
    sal_printf("Packetio Tx Drop : %d \n", value);
    for (value = 0, i = 0; i < MAX_PKTIO_SOCKETS; i++)
        value += _sys_sockets[i].tx_err;
    sal_printf("Packetio Tx Error: %d \n", value);
    for (value = 0, i = 0; i < MAX_PKTIO_SOCKETS; i++)
        value += _sys_sockets[i].rx_cnt;
    sal_printf("Packetio Rx Ok   : %d \n", value);
    for (value = 0, i = 0; i < MAX_PKTIO_SOCKETS; i++)
        value += _sys_sockets[i].rx_drop;
    sal_printf("Packetio Rx Drop : %d \n", value);
    for (value = 0, i = 0; i < MAX_PKTIO_SOCKETS; i++)
        value += _sys_sockets[i].rx_err;
    sal_printf("Packetio Rx Error: %d \n", value);
}


/*
 * Function:
 *     soc_packetio_send
 * Description:
 *     Transmit a packet
 */
int soc_packetio_send(int unit, uint8_t *data, int len)
{
    extern int tx_dump_packet;
    int rv = 0, i;
    struct sockaddr_ll rawsock;

    if (unit >= MAX_PKTIO_SOCKETS) {
        unit = 0;
    }
    if (_sys_sockets[unit].taskhdl == 0) {
        _sys_sockets[unit].tx_drop++;
        return -1;
    }
    if (tx_dump_packet) {
        sal_printf("Pkt with CB header:\n");
        for(i=0; i < len; i++) {
            if ((i % 16) == 0) {
                sal_printf("\n%04x: ", i);
            }
            sal_printf("%02x ", *(data + i));
        }
        sal_printf("\n");
    }

    /* Index of the network device */
    rawsock.sll_ifindex = _sys_sockets[unit].ifindex;
    rawsock.sll_halen = ETH_ALEN;
    /* Destination MAC */
    sal_memcpy((void*)&rawsock.sll_addr[0], (void*)data, ETH_ALEN);

    /* Send packet */
    rv = sendto(_sys_sockets[unit].sockfd, data, len, 0,
                   (struct sockaddr*)&rawsock, sizeof(struct sockaddr_ll));
    if (rv < 0) {
        perror("PacketIO Send packet failed");
        _sys_sockets[unit].tx_err++;
        return -1;
    } else {
        /* Sendto retuns bytecount, return status ok instead */
        rv = 0;
    }

    _sys_sockets[unit].tx_cnt++;
    return rv;
}

/*
 * Function:
 *     soc_packetio_recv
 * Description:
 *     Task to Receive a packet and process it
 */
void soc_packetio_recv(void *params)
{
    int rlen = 0;
    int sockfd = 0;
    int pktlen = 0;
    uint8 pktbuffer[2048];
    uint8 *buffer;
    cbxi_egress_imp_header_t cb_header;
    cbxi_timestamp_header_t  ts_header;
    brcm_egress_imp_header_t bcm_header;
    int unit = PTR_TO_INT(params);

    if (unit > MAX_PKTIO_SOCKETS) {
        return;
    }

    sockfd = _sys_sockets[unit].sockfd;
    while (_sys_sockets[unit].thread_stop == 0) {
        rlen = recvfrom(sockfd, pktbuffer, sizeof(pktbuffer), 0, NULL, NULL);
        if (rlen < 0) {
            _sys_sockets[unit].rx_err++;
            continue;
        }
        _sys_sockets[unit].rx_cnt++;
        if (_sys_sockets[unit].thread_drop) {
            _sys_sockets[unit].rx_drop++;
            continue;
        }
        buffer = cbxi_rx_extract_header(pktbuffer, rlen, rlen, &pktlen,
                                        &cb_header, &ts_header, &bcm_header);
        if (!buffer) {
#if 0
            sal_printf("PacketIO: No IMP Header on packet, Dropping\n");
#endif
        } else {
            cbxi_rx_process_pkt(buffer, pktlen, &cb_header, &ts_header, &bcm_header);
        }
    }
    sal_thread_exit(0);
    return ;
}

/*
 * Function:
 *     soc_packetio_uninit
 * Description:
 *     Terminate the RAW socket interface
 */
int soc_packetio_uninit(int unit)
{
    if (unit > MAX_PKTIO_SOCKETS) {
        return -1;
    }
    _sys_sockets[unit].thread_stop = 1;
    sal_usleep(200);
    close(_sys_sockets[unit].sockfd);
    sal_memset(&_sys_sockets[unit], 0, sizeof(pktio_cb_t));
    return 0;
}

/*
 * Function:
 *     soc_packetio_init
 * Description:
 *     Initialize the Raw Socket Wrapper
 */
int soc_packetio_init(int unit)
{
    int option, sockfd;
    struct ifreq ifreq;
    sal_thread_t taskhdl;
    struct timeval tv;

    if (unit >= MAX_PKTIO_SOCKETS) {
        return 0;
    }

    /* Open RAW socket to send on */
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("PacketIO: Socket Open Failed");
        return -1;
    }

    _sys_sockets[unit].sockfd = sockfd;

    /* Get the index of the interface to send on */
    sal_memset(&ifreq, 0, sizeof(struct ifreq));
    sal_strncpy(ifreq.ifr_name, USER_PKTIO_DEFAULT_IFACE, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifreq) < 0) {
        perror("PacketIO: Get Ifindex Failed");
        return -1;
    }

    _sys_sockets[unit].ifindex = ifreq.ifr_ifindex;

    /* Get the MAC address of the interface to send on */
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifreq) < 0) {
        perror("PacketIO: Get MacAddress Failed");
        return -1;
    }

    sal_memcpy((void*)&_sys_sockets[unit].macsa, (void*)&ifreq.ifr_hwaddr.sa_data[0], ETH_ALEN);

    /* Set Promisc mode on this interface */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifreq) < 0) {
        perror("PacketIO: Get Interface Flags Failed");
        return -1;
    }
    ifreq.ifr_flags |= IFF_PROMISC;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifreq) < 0) {
        perror("PacketIO: Set Interface PROMISCUS mode Failed");
        return -1;
    }

    /* Set Reuse rules on and allow broadcast */
    option = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("PacketIO: Set Socket REUSEADDR Failed");
        close(sockfd);
        return -1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option)) == -1) {
        perror("PacketIO: Set Socket BROADCAST Failed");
        close(sockfd);
        return -1;
    }
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("PacketIO: Set Rx Timeout Failed");
        close(sockfd);
        return -1;
    }

    /* Start Thread for Rx Processing */
    taskhdl = sal_thread_create("packetio", SAL_THREAD_STKSZ, 1,
                                soc_packetio_recv, INT_TO_PTR(unit));
    if (taskhdl == SAL_THREAD_ERROR)
    {
        perror("PacketIO: Rx Thread Create Failed");
        return -1;
    }
    _sys_sockets[unit].taskhdl = taskhdl;
    return 0;
}
