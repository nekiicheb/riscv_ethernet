#ifndef APPLICATION_HART0_NET_NET_H_
#define APPLICATION_HART0_NET_NET_H_
#include <stdio.h>
#include <stdbool.h>

#define PACKET_MAX   16384U
struct eth_pkt
{
  size_t length;
  uint8_t data[PACKET_MAX];
} eth_pkt;


volatile bool is_available_rx_pkt;
void eth_low_level_init(void);
struct eth_pkt* get_rx_pkt(void);
int32_t eth_send_pkt(const uint8_t* buf, size_t length);

#endif /* APPLICATION_HART0_NET_NET_H_ */
