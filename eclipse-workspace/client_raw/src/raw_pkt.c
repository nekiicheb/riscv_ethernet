#include "./net_utils/proprietary_pkt_util.h"
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

#define MY_DEST_MAC0	0x00
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x00
#define MY_DEST_MAC3	0x00
#define MY_DEST_MAC4	0x00
#define MY_DEST_MAC5	0x00

#define DEFAULT_IF	"lo"
#define BUF_SIZE		1024
#define SRC_ADDR  "127.0.0.1"
#define DST_ADDR  "127.0.0.1"
#define SRC_PORT  "7777"
#define DST_PORT  "8888"
#define CRC32_SIZE 4
#define L2_SIZE 14

int main(int argc, char *argv[])
{
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[BUF_SIZE];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	struct udphdr *udp = (struct udphdr *) (sendbuf + sizeof(struct ether_header) + sizeof(struct iphdr));
	struct sockaddr_ll socket_address;
	char ifName[IFNAMSIZ];

	/* Get interface name */
	strcpy(ifName, DEFAULT_IF);

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZE);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETH_P_IP);
	tx_len += sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr) + CRC32_SIZE;

	/* fabricate the IP header */
	iph->ihl      = 5;
	iph->version  = 4;
	iph->tos      = 16; // low delay
	iph->tot_len  = htons(sizeof(struct iphdr) + sizeof(struct udphdr));
	iph->id       = htons(54321);
	iph->ttl      = 64; // hops
	iph->protocol = 17; // UDP
	iph->check = 0;
	iph->saddr = inet_addr(SRC_ADDR);
	iph->daddr = inet_addr(DST_ADDR);
	iph->check = 0;
	// fabricate the UDP header
	udp->source = htons(atoi(SRC_PORT));
	// destination port number
	udp->dest = htons(atoi(DST_PORT));
	udp->len = htons(sizeof(struct udphdr));

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = MY_DEST_MAC0;
	socket_address.sll_addr[1] = MY_DEST_MAC1;
	socket_address.sll_addr[2] = MY_DEST_MAC2;
	socket_address.sll_addr[3] = MY_DEST_MAC3;
	socket_address.sll_addr[4] = MY_DEST_MAC4;
	socket_address.sll_addr[5] = MY_DEST_MAC5;
	*(uint32_t*)(sendbuf + tx_len - CRC32_SIZE) = htonl(get_prop_pkt_crc((uint8_t*)iph, tx_len - L2_SIZE - CRC32_SIZE));

	/* Send correct pkt */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    printf("Send correct pkt failed\n");

	/* Send incorrect pkt */
	*(uint32_t*)(sendbuf + tx_len - CRC32_SIZE) = htonl(1);
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    printf("Send incorrect pkt failed\n");
	return 0;
}
