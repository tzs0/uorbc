#ifndef __MOD_UORBC_UTILS_H__
#define __MOD_UORBC_UTILS_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "mod_uorbc_ringbuffer.h"
#include "mod_uorbc_platform.h"

//-------------------------------------------------------------
#define UORB_MAX_SUB_NUM   		            10
#define UORBC_QUEUE_MSG_NUM                 32
#define UORB_MULTI_MAX_INSTANCES	        7

#define uorbc_min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define uorbc_max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

typedef enum {
	UORBC_NONE = 0,
	ORBC_SYS_STATE = 1,
	ORBC_SYS_INFO = 2,
	ORBC_PRICE_INFO = 3,
	TOTAL_UORBC_NUM
} E_UORBC_ID;
//-------------------------------------------------------------
typedef struct _queue_payload_header {
	uint32_t                            timestamp; // required for logger
	uint8_t                             try_count; /* try send msg count */
	int32_t                             authority_list[UORB_MAX_SUB_NUM]; /* used to check if all subscriber got this msg */
	char                                buf[0];
} queue_payload_header_t, *p_queue_payload_header_t;

typedef struct _nonqueue_mode {
	bool                                published;
	void*                               data;
	int32_t                             authority_list[UORB_MAX_SUB_NUM];
} nonqueue_mode_t, *p_nonqueue_mode_t;

typedef struct _queue_mode {
	ring_buffer_t*			            queue;
	uint32_t							queue_lastmsg_pos[UORB_MAX_SUB_NUM];
	uint32_t                            queue_perpayload_len; /* lenth per queue payload */
	uint8_t*                            one_queue_msg;
	uint32_t							cur_msg_pos;
	uint 								queue_size; /* 队列深度 */
	size_t 								qbuf_size; /* 队列buffer大小 */
} queue_mode_t, *p_queue_mode_t;

typedef struct _orb_data {
	int32_t                             orb_id;
	uorbc_platform_sem_t                sem;
	uint32_t                            last_updated_time;
	int32_t                             registered_list[UORB_MAX_SUB_NUM]; /* subscriber list */
	bool                                queue_mode;

	union {
		nonqueue_mode_t                 nonq;
		queue_mode_t                    q;
	} meta;
} orb_data_t, *p_orb_data_t;
//-------------------------------------------------------------
bool mod_uorbc_utils_ismulti(const int orb_id);
int mod_uorbc_utils_get_orbid(const char* name);
E_UORBC_ID mod_uorbc_utils_multi_getpos(const int orb_id);
//-------------------------------------------------------------


#endif

