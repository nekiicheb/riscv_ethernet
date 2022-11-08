#ifndef NET_UTILS_NETINET_IN_H_
#define NET_UTILS_NETINET_IN_H_

#include <stdint.h>

#undef htons
#undef htonl
#undef ntohs
#undef ntohl

uint16_t htons(uint16_t v);
uint32_t htonl(uint32_t v);
uint16_t ntohs(uint16_t v);
uint32_t ntohl(uint32_t v);

#endif /* NET_UTILS_NETINET_IN_H_ */
