#ifndef APPLICATION_HART0_NET_NET_H_
#define APPLICATION_HART0_NET_NET_H_
#include <stdio.h>
#include <stdbool.h>

volatile bool is_available_rx_pkt;
void eth_low_level_init(void);
int32_t eth_send_pkt(uint8_t* buf, size_t length);

#endif /* APPLICATION_HART0_NET_NET_H_ */
