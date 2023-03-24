#pragma once

#include <stdint.h>
#ifdef __cplusplus
#include <cstring>
#else
#include <string.h>
#endif

#include "uORB.h"


#define MACHINE_TYPE_LEN        12

struct sys_state_s {
	volatile uint8_t state;
};
/* register this as object request broker structure */
ORB_DECLARE(sys_state);

struct sysinfo_s {
	volatile char machine_type[MACHINE_TYPE_LEN];
	volatile uint8_t machine_number;
	volatile uint8_t mbn; /* MASTER or BACKUP or NORMAL */
	volatile uint8_t net; /* 0:init(no wifi&eth) 1:eth 2:wifi */
};
/* register this as object request broker structure */
ORB_DECLARE(sysinfo);

struct price_info_s {
	volatile uint32_t price;
	volatile uint8_t mode; /* basic; incremental; multi; and so on */
	volatile uint8_t discount;
	/* other price info */
};
/* register this as object request broker structure */
ORB_DECLARE(price_info);

