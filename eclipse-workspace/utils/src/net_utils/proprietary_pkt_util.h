#ifndef NET_UTILS_PROPRIETARY_PKT_UTIL_H_
#define NET_UTILS_PROPRIETARY_PKT_UTIL_H_

#include "../project_cfg.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
bool check_proprietary_crc(const uint8_t* buf, size_t len);
uint32_t get_prop_pkt_crc(const uint8_t *data, size_t len);
#ifdef CHECKSUM_TEST
void test_checksum();
#endif

#endif /* NET_UTILS_PROPRIETARY_PKT_UTIL_H_ */
