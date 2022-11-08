#ifndef NET_UTILS_CHECKSUM_H_
#define NET_UTILS_CHECKSUM_H_

#include "project_cfg.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
bool check_ip_crc(uint8_t* buf, size_t len);
uint16_t get_ip_chksum(uint8_t *data, int size);
#ifdef CHECKSUM_TEST
void test_checksum();
#endif
#endif /* NET_UTILS_CHECKSUM_H_ */
