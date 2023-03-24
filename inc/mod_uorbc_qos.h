#ifndef __MOD_UORBC_QOS_H_
#define __MOD_UORBC_QOS_H_

#include "list.h"


/* 0:just send msg, dont care received or not */
#define UORBC_QOS_0             0
/* 1:send and guarantee msg is received by all receiver */
#define UORBC_QOS_1             1


int mod_uorbc_qos_init(void);
int mod_uorbc_qos_enqueue(struct list_head* l);


#endif

