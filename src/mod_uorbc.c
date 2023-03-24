/********************************************************************************

 **** Copyright (C), 2021, xx xx xx xx info&tech Co., Ltd.                ****

 ********************************************************************************
 * File Name     : mod_uorbc.c
 * Author        : vincent
 * Date          : 2021-06-02
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 *
 * Record        :
 * 1.Date        : 2021-06-02
 *   Author      : vincent
 *   Modification: Created file

*************************************************************************************************************/

#include "mod_uorbc.h"
#include "uORB.h"
#include "mod_uorbc_utils.h"
#include <time.h>
#include "mod_uorbc_platform.h"
#include "mod_uorbc_qos.h"

/* uORB: micro object request broker */


//----------------------------------------------------------------
static void orb_init_multi(int i);
static void orb_init_nomulti(int i);
static int get_subscriber_id(void);
static int get_subscriber_id_pos(p_orb_data_t p, int subscriber_id);
static p_orb_data_t* uorbc_instance(void);
static int orb_check(int handle);
//----------------------------------------------------------------
static p_orb_data_t* uorbc_instance(void)
{
	static p_orb_data_t* g_orb_buf = NULL;

	if(NULL==g_orb_buf) {
		g_orb_buf = (p_orb_data_t*)uorbc_platform_malloc(sizeof(p_orb_data_t) * TOTAL_UORBC_NUM);
		uorbc_platform_assert(g_orb_buf);
		memset(g_orb_buf, 0x00, (sizeof(p_orb_data_t) * TOTAL_UORBC_NUM));
	}
//    uorbc_platform_log("g_orb_buf:%p", g_orb_buf);

	return g_orb_buf;
}
//----------------------------------------------------------------
static void orb_init_multi(int i)
{
	int ret = -1;
	int j = 0;
	int k = 0;

	uorbc_platform_log("orb_init_multi");

	p_orb_data_t* orb = uorbc_instance();
	p_orb_data_t p = NULL;

	orb[i] = (p_orb_data_t) uorbc_platform_malloc(sizeof(orb_data_t) * UORB_MULTI_MAX_INSTANCES);
	uorbc_platform_assert(orb[i]);
	memset(orb[i], 0x00, (sizeof(orb_data_t) * UORB_MULTI_MAX_INSTANCES));

	for(j = 0; j < UORB_MULTI_MAX_INSTANCES; ++j) {
		p = &(orb[i][j]);

		p->orb_id = -1;
		p->queue_mode = false;
		ret = uorbc_platform_sem_init(& (p->sem), 0, 1);
		if(0 != ret) {
			uorbc_platform_perror("uorbc_platform_sem_init");
		}
		p->last_updated_time = 0;
		for(k = 0; k < UORB_MAX_SUB_NUM; ++k) {
			p->registered_list[k] = -1;
		}
	}
}
//----------------------------------------------------------------
static void orb_init_nomulti(int i)
{
	int ret = -1;
	int k = 0;

	uorbc_platform_log("orb_init_nomulti");

	p_orb_data_t* orb = uorbc_instance();

	orb[i] = (p_orb_data_t)uorbc_platform_malloc(sizeof(orb_data_t));
	uorbc_platform_assert(orb[i]);
	memset(orb[i], 0x00, (sizeof(orb_data_t)));

	orb[i]->orb_id = -1;
	orb[i]->queue_mode = false;

	ret = uorbc_platform_sem_init(& (orb[i]->sem), 0, 1);
	if(0 != ret) {
		uorbc_platform_perror("uorbc_platform_sem_init");
	}
	orb[i]->last_updated_time = 0;
	for(k = 0; k < UORB_MAX_SUB_NUM; ++k) {
		orb[i]->registered_list[k] = -1;
	}
}
//----------------------------------------------------------------
int orb_exists(const struct orb_metadata* meta, int instance)
{
	int orb_id = mod_uorbc_utils_get_orbid(meta->o_name);
	if(orb_id < 0 || orb_id >= TOTAL_UORBC_NUM) {
		return -1;
	}
	if(!mod_uorbc_utils_ismulti(orb_id) && instance > 0) {
		return -1;
	}

	p_orb_data_t* orb = uorbc_instance();
	p_orb_data_t p = &(orb[orb_id][instance]);

	if((p->orb_id) != -1) {
		return 0;
	}

	return -1;
}
//----------------------------------------------------------------
orb_advert_t orb_advertise(const struct orb_metadata* meta, const void* data)
{
//	uorbc_platform_log( "orb_advertise" );
	if(UORBC_QOS_1==meta->qos) {
		uorbc_platform_log("QOS1 topic need use queue mode");
		return orb_advertise_queue(meta, data, UORBC_QUEUE_MSG_NUM);
	}

	return orb_advertise_multi(meta, data, NULL);
}
//----------------------------------------------------------------
orb_advert_t orb_advertise_queue(const struct orb_metadata* meta, const void* data, unsigned int queue_size)
{
	return orb_advertise_multi_queue(meta, data, NULL, queue_size);
}
//----------------------------------------------------------------
static int get_subscriber_id(void)
{
	/* now we just use different thread as reciever */
	return (int)uorbc_platfrom_get_thread_id();
}
//----------------------------------------------------------------
//Unqueued version
orb_advert_t orb_advertise_multi(const struct orb_metadata* meta, const void* data, int* instance)
{
	if(UORBC_QOS_1==meta->qos) {
		uorbc_platform_log("QOS1 topic need use queue mode");
		return orb_advertise_multi_queue(meta, data, instance, UORBC_QUEUE_MSG_NUM);
	}

	int inst = -1;
	int orb_id = -1;
	int task_id = -1;
	int i = 0;
	p_orb_data_t p = NULL;
//	uorbc_platform_log( "orb_advertise_multi" );

	orb_id = mod_uorbc_utils_get_orbid(meta->o_name);

	uorbc_platform_log("orb name:%s o_size：%d orb_id:%d", meta->o_name, meta->o_size, orb_id);

	if(orb_id < 0 || orb_id >= TOTAL_UORBC_NUM) {
		return NULL;
	}
	else {
		if(NULL == instance) {
			inst = 0;
		}
		else {
			inst = *instance;
		}

		uorbc_platform_log("inst:%d", inst);
		p_orb_data_t* orb = uorbc_instance();
		p = &(orb[orb_id][inst]);

		uorbc_platform_log("set queue_mode as false, this is nonqueue mode");
		p->queue_mode = false;
		p_nonqueue_mode_t nonq = &(p->meta.nonq);

		if(/* ( mod_uorbc_utils_ismulti( orb_id ) ) &&*/ (inst >= 0) && (inst < UORB_MULTI_MAX_INSTANCES)) {
			uorbc_platform_sem_wait(&(p->sem));
			nonq->published = false;
//            int atomic_state = orb_lock();
			if(NULL==nonq->data) {
				nonq->data = (uint8_t*)uorbc_platform_malloc(sizeof(uint8_t) * meta->o_size);
				uorbc_platform_assert(nonq->data);
				memset(nonq->data, 0x00, (sizeof(uint8_t) * meta->o_size));
			}
			uorbc_platform_memcpy(nonq->data, data, meta->o_size);
//            orb_unlock(atomic_state);
			p->orb_id = orb_id;
			p->last_updated_time = time(NULL);
			nonq->published = true;
			task_id = get_subscriber_id();
			for(i = 0; i < UORB_MAX_SUB_NUM; ++i) {
				nonq->authority_list[i] = -1;
			}
			uorbc_platform_sem_post(&(p->sem));
		}
	}

	if(NULL == p) {
		uorbc_platform_log("advert fail");
	}
	else {
		uorbc_platform_log("advert succ");
	}

	return (orb_advert_t)p;
}
//----------------------------------------------------------------
orb_advert_t orb_advertise_multi_queue(const struct orb_metadata* meta, const void* data, int* instance,
                                       unsigned int queue_size)
{
	p_orb_data_t p = NULL;
	int inst = -1;
	int orb_id = -1;
	int task_id = -1;
	int i = 0;

	orb_id = mod_uorbc_utils_get_orbid(meta->o_name);

	uorbc_platform_log("orb name:%s o_size:%d orb_id:%d queue_size:%d", meta->o_name, meta->o_size, orb_id, queue_size);

	if(orb_id < 0 || orb_id >= TOTAL_UORBC_NUM) {
		return NULL;
	}
	else {
		if(NULL == instance) {
			inst = 0;
		}
		else {
			inst = *instance;
		}

		uorbc_platform_log("inst:%d", inst);

		p_orb_data_t* orb = uorbc_instance();
		p = &(orb[orb_id][inst]);
		p->queue_mode = true;
		p_queue_mode_t q = &(p->meta.q);

		if(/* ( mod_uorbc_utils_ismulti( orb_id ) ) && */ (inst >= 0) && (inst < UORB_MULTI_MAX_INSTANCES)) {
			uorbc_platform_sem_wait(&(p->sem));

			uint32_t payload_len = sizeof(queue_payload_header_t)+meta->o_size;
			q->queue_perpayload_len = ring_buffer_to_the_power_of_2(payload_len);
			q->one_queue_msg = (uint8_t*)uorbc_platform_malloc(sizeof(uint8_t) * (q->queue_perpayload_len));
			uorbc_platform_assert(q->one_queue_msg);
			memset(q->one_queue_msg, 0x00, (sizeof(uint8_t) * (q->queue_perpayload_len)));
			q->queue = ring_buffer_init(queue_size, q->queue_perpayload_len);
			uorbc_platform_assert(q->queue);
			q->qbuf_size = ring_buffer_get_size(q->queue);
			uorbc_platform_log("meta->o_name:%s qbuf_size:%d", meta->o_name, q->qbuf_size);
			uorbc_platform_log("queue_perpayload_len:%d", q->queue_perpayload_len);

			/* if no space, dequeue a payload msg to p->data */
			if(ring_buffer_left_space(q->queue) < (q->queue_perpayload_len)) {
				ring_buffer_dequeue_arr(q->queue, q->one_queue_msg, q->queue_perpayload_len);
			}

			/* publish a initial msg */
			p_queue_payload_header_t queue_payload = (p_queue_payload_header_t)(q->one_queue_msg);
			queue_payload->timestamp = time(NULL);
			queue_payload->try_count = 1;
			memset(queue_payload->authority_list, -1, sizeof(queue_payload->authority_list));
			if(NULL!=data) {
				memcpy(queue_payload->buf, data, meta->o_size);
			}
			ring_buffer_queue_arr(q->queue, (char*)queue_payload, q->queue_perpayload_len);

			p->orb_id = orb_id;
			p->last_updated_time = time(NULL);

			q->cur_msg_pos++;
			q->queue_size = queue_size;
			uorbc_platform_log("cur_msg_pos:%d, last_gen is 0, queue_size:%d", q->cur_msg_pos, queue_size);

			task_id = get_subscriber_id();

			for(i = 0; i < UORB_MAX_SUB_NUM; ++i) {
				q->queue_lastmsg_pos[i] = 0;
			}

			uorbc_platform_sem_post(&(p->sem));
		}
	}

	if(NULL == p) {
		uorbc_platform_log("advert fail");
	}
	else {
		uorbc_platform_log("advert succ");
	}

	return (orb_advert_t)p;
}
//----------------------------------------------------------------
int orb_unadvertise(orb_advert_t* handle)
{
	*handle = NULL;
	return 0;
}
//----------------------------------------------------------------
int orb_publish_auto(const struct orb_metadata* meta, orb_advert_t* handle, const void* data, int* instance)
{
	int orb_id = -1;

	if(*handle == NULL) {
		int orb_id = mod_uorbc_utils_get_orbid(meta->o_name);
		if(mod_uorbc_utils_ismulti(orb_id)) {
			*handle = orb_advertise_multi(meta, data, instance);
		}
		else {
			*handle = orb_advertise(meta, data);
			if(instance) {
				*instance = 0;
			}
		}

		if(*handle != NULL) {
			return 0;
		}
	}
	else {
		return orb_publish(meta, UORB_PUB_NORMAL, data);
	}

	return -1;
}
//----------------------------------------------------------------
int  orb_publish(const struct orb_metadata* meta, uint8_t retry, const void* data)
{
	int orb_id = -1;
	int multi_pos = 0;
	size_t buffer_space = 0;

	orb_id = mod_uorbc_utils_get_orbid(meta->o_name);

	uorbc_platform_log("orb name:%s orb_id:%d", meta->o_name, orb_id);

	if(orb_id < 0 || orb_id >= TOTAL_UORBC_NUM) {
		uorbc_platform_log_err("invalid orb_id:%d", orb_id);
		return -1;
	}
	else {
		if(mod_uorbc_utils_ismulti(orb_id)) {
			multi_pos = mod_uorbc_utils_multi_getpos(orb_id);
			if(UORBC_NONE==multi_pos) {
				uorbc_platform_log_err("invalid orb_id:%d", orb_id);
				return -1;
			}
		}

		p_orb_data_t* orb = uorbc_instance();
		p_orb_data_t p = &(orb[orb_id][multi_pos]);

		if(false==p->queue_mode) { /* nonqueue mode */
			p_nonqueue_mode_t nonq = &(p->meta.nonq);
			uorbc_platform_sem_wait(&(p->sem));
			nonq->published = false;
//            int atomic_state = orb_lock();
			uorbc_platform_memcpy(nonq->data, data, meta->o_size);
//            orb_unlock(atomic_state);
			p->orb_id = orb_id;
			p->last_updated_time = time(NULL);
			nonq->published = true;
			for(int i = 0; i < UORB_MAX_SUB_NUM; ++i) {
				nonq->authority_list[i] = -1; /* this is a brand new msg */
			}
			uorbc_platform_sem_post(&(p->sem));
		}
		else { /* queue mode */
			p_queue_mode_t q = &(p->meta.q);
			uorbc_platform_sem_wait(&(p->sem));

			/* if no space in ringbuffer */
			buffer_space = ring_buffer_left_space(q->queue);
			if(buffer_space < (q->queue_perpayload_len)) {
				ring_buffer_dequeue_arr(q->queue, q->one_queue_msg, (q->queue_perpayload_len));
			}

			p_queue_payload_header_t one_msg = NULL;
			if(UORB_PUB_RETRY==retry) {
				one_msg = (p_queue_payload_header_t)data;
				one_msg->try_count++;
			}
			else { /* this is a brand new msg */
				one_msg = (p_queue_payload_header_t)q->one_queue_msg;
				memset(one_msg, 0x00, q->queue_perpayload_len);
				one_msg->timestamp = time(NULL);
				one_msg->try_count = 1;
				memset(one_msg->authority_list, -1, sizeof(one_msg->authority_list));
				memcpy(one_msg->buf, data, meta->o_size);
			}

			/* DEBUGURLVGL TODO: 如果qos是1,将消息推入qos队列,qos队列重发后将消息出队列 */
			if(UORBC_QOS_1==meta->qos) {
				//TODO: mod_uorbc_qos_enqueue
			}

			ring_buffer_queue_arr(q->queue, one_msg, q->queue_perpayload_len);

			p->orb_id = orb_id;
			p->last_updated_time = time(NULL);
			q->cur_msg_pos++;

			uorbc_platform_sem_post(&(p->sem));
		}
	}

	return 0;
}
//----------------------------------------------------------------
int  orb_subscribe(const struct orb_metadata* meta)
{
	int ret = -1;
	int orb_id = -1;
	int instance = 0;
	int task_id = -1;
	uint32_t cur_msg_pos = 0;
	uint queue_size = 0;

	if(meta == NULL || meta->o_name == NULL) {
		uorbc_platform_log_err("meta == NULL || meta->o_name == NULL");
		return -1;
	}

	orb_id = mod_uorbc_utils_get_orbid(meta->o_name);

	uorbc_platform_log("orb name:%s orb_id:%d", meta->o_name, orb_id);

	if(orb_id > 0 && orb_id < TOTAL_UORBC_NUM) {
		if(mod_uorbc_utils_ismulti(orb_id)) {     /* 使用orb_subscribe_multi订阅multi topic */
			return ret;
		}

		task_id = get_subscriber_id();

//        int atomic_state = orb_lock();
		p_orb_data_t* orb = uorbc_instance();
		p_orb_data_t p = &(orb[orb_id][instance]);

		uorbc_platform_sem_wait(&(p->sem));
		for(int i = 0; i < UORB_MAX_SUB_NUM; ++i) {
			if(p->registered_list[i] == task_id) {
				ret = (int)((orb_id << 4) | (instance));
				break;
			}

			if(p->registered_list[i] == -1) {
				p->registered_list[i] = task_id;
				uorbc_platform_log("p->registered_list[%d]:%d task_id:%d,p:%p",
				                   i, p->registered_list[i], task_id,p);

				// If there were any previous publications allow the subscriber to read them
				if(true==p->queue_mode) {
					p_queue_mode_t q = &(p->meta.q);
					cur_msg_pos = q->cur_msg_pos;
					queue_size = q->queue_size;
					q->queue_lastmsg_pos[i] = cur_msg_pos - uorbc_min(cur_msg_pos, queue_size);
					uorbc_platform_log("cur_msg_pos:%d", q->cur_msg_pos);
				}

				ret = (int)((orb_id << 4) | (instance));
				break;
			}
		}
		uorbc_platform_sem_post(&(p->sem));
//        orb_unlock(atomic_state);
	}
	else {
		uorbc_platform_log_err("error orb_id:%d", orb_id);
		return -1;
	}

	uorbc_platform_log("sub handle:%d", ret);

	return ret;
}
//----------------------------------------------------------------
int  orb_subscribe_multi(const struct orb_metadata* meta, unsigned instance)
{
	int ret = -1;
	int orb_id = -1;
	int task_id = -1;
	int i = 0;

	if(meta == NULL || meta->o_name == NULL) {
		uorbc_platform_log_err("meta == NULL || meta->o_name == NULL");
		return -1;
	}

	orb_id = mod_uorbc_utils_get_orbid(meta->o_name);

	uorbc_platform_log("orb name:%s orb_id:%d", meta->o_name, orb_id);

	if(orb_id >= 0 && orb_id < TOTAL_UORBC_NUM && mod_uorbc_utils_ismulti(orb_id)) {
		task_id = get_subscriber_id();
//        int atomic_state = orb_lock();
		p_orb_data_t* orb = uorbc_instance();
		p_orb_data_t p = &(orb[orb_id][instance]);
		uorbc_platform_sem_wait(&(p->sem));
		for(i = 0; i < UORB_MAX_SUB_NUM; ++i) {
			if(p->registered_list[i] == task_id) {
				ret = (int)((orb_id << 4) | (instance));
				break;
			}
			if(p->registered_list[i] == -1) {
				p->registered_list[i] = task_id;
				ret = (int)((orb_id << 4) | (instance));
				break;
			}
		}
		uorbc_platform_sem_post(&(p->sem));
//        orb_unlock(atomic_state);
	}

	uorbc_platform_log("sub:%d", ret);

	return ret;
}
//----------------------------------------------------------------
int  orb_unsubscribe(int handle)
{
	int task_id = -1;
	int orb_id = -1;
	int instance = -1;
	int i = 0;

	if(handle < 0) {
		return -1;
	}

	orb_id = (handle >> 4);
	instance = handle - ((handle >> 4) << 4);

	p_orb_data_t* orb = uorbc_instance();
	p_orb_data_t p = &(orb[orb_id][instance]);

	if(orb_id >= 0 && orb_id < TOTAL_UORBC_NUM) {
		task_id = get_subscriber_id();
//        int atomic_state = orb_lock();
		uorbc_platform_sem_wait(&(p->sem));
		for(i = 0; i < UORB_MAX_SUB_NUM; ++i) {
			if(p->registered_list[i] == task_id) {
				p->registered_list[i] = -1;
				uorbc_platform_sem_post(&(p->sem));
//                orb_unlock(atomic_state);
				return 0;
			}
		}
		uorbc_platform_sem_post(&(p->sem));
//        orb_unlock(atomic_state);
	}

	return -1;
}

static int get_subscriber_id_pos(p_orb_data_t p, int subscriber_id)
{
//	uorbc_platform_log("p:%p subscribe_id:%d", p, subscriber_id);

	for(int i=0; i<UORB_MAX_SUB_NUM; i++) {
//        uorbc_platform_log("p->registered_list[%d]:%d task_id:%d p:%p",
//                    i, p->registered_list[i], subscriber_id, p);
		if(subscriber_id==p->registered_list[i]) {
//			uorbc_platform_log("subscribe_id:%d pos:%d", subscriber_id, i);
			return i;
		}
	}

	return -1;
}

static bool subscriber_id_hit(p_queue_payload_header_t queue_msg, int subscriber_id)
{
	for(int i=0; i<sizeof(queue_msg->authority_list); i++) {
		if(subscriber_id==queue_msg->authority_list[i]) {
//			uorbc_platform_log("subscriber_id(%d) is already in queue_msg->authority_list", subscriber_id);
			return true;
		}
	}

	return false;
}
//----------------------------------------------------------------
int orb_copy(const struct orb_metadata* meta, int handle, void* buffer)
{
	int multi_pos = -1;
	int task_id = -1;
	int i = 0;
	int registered_len = 0, authority_len = 0;
	uint32_t cur_msg_pos = 0;
	uint queue_size = 0;
	int sub_val = 0;
	ring_buffer_t* p_ring = NULL;
	size_t peek_pos = 0;

	int orb_id = mod_uorbc_utils_get_orbid(meta->o_name);

	uorbc_platform_log("orb_id:%d o_name:%s o_size:%d handle:%d", orb_id, meta->o_name, meta->o_size, handle);

	if(orb_id != -1 && orb_id == (handle >> 4)) {
		multi_pos = handle - ((handle >> 4) << 4);
		p_orb_data_t* orb = uorbc_instance();
		p_orb_data_t p = &(orb[orb_id][multi_pos]);
		uorbc_platform_sem_wait(&(p->sem));

		if(orb_check(handle)<=0) {
			uorbc_platform_log("error or no msg");
			uorbc_platform_sem_post(& (p->sem));
			return -1;
		}

		if(multi_pos > 0 && !mod_uorbc_utils_ismulti(orb_id)) {
			uorbc_platform_sem_post(& (p->sem));
			uorbc_platform_log_err("error multi_pos:%d not multi", multi_pos);
			return -1;
		}
		else if(multi_pos >= 0 && multi_pos < UORB_MULTI_MAX_INSTANCES) {
			task_id = get_subscriber_id();

//				int atomic_state = orb_lock();
			if(true == p->queue_mode) {
				p_queue_mode_t q = &(p->meta.q);
				cur_msg_pos = q->cur_msg_pos;
				int idpos = get_subscriber_id_pos(p, task_id);
				if(-1==idpos) {
					uorbc_platform_sem_post(& (p->sem));
					uorbc_platform_log_err("error idpos(%d)", idpos);
					return -1;
				}

				queue_size = q->queue_size;
				if(cur_msg_pos > (q->queue_lastmsg_pos[idpos] + queue_size)) {
					/* Reader is too far behind: some messages are lost */
					q->queue_lastmsg_pos[idpos] = cur_msg_pos - queue_size;
				}

				uint32_t msg_pan = q->cur_msg_pos-q->queue_lastmsg_pos[idpos];
				uint32_t* que_buf = uorbc_platform_malloc(sizeof(uint32_t)+msg_pan*(meta->o_size));
				uorbc_platform_assert(que_buf);
				memset(que_buf, 0x00, (sizeof(uint32_t)+msg_pan*(meta->o_size)));
				que_buf[0] = msg_pan;
				uint8_t* _que_buf = (uint8_t*)(&que_buf[1]);

				do {
					p_ring = q->queue;
					peek_pos = (q->queue_lastmsg_pos[idpos])*(q->queue_perpayload_len);
					peek_pos %= (q->qbuf_size);
					p_queue_payload_header_t queue_msg = (p_queue_payload_header_t)ring_buffer_pos_pointer(p_ring, peek_pos);
					if(queue_msg->try_count>1) { /* this is a retry msg */
						if(true==subscriber_id_hit(queue_msg, task_id)) { /* this receiver has received this msg */
							/* noneed get msg again */
							if(que_buf) {
								free(que_buf);
								que_buf = NULL;
								_que_buf = NULL;
							}
						}
						else {
							ring_buffer_just_peek(p_ring, _que_buf, peek_pos+sizeof(queue_payload_header_t), meta->o_size);
							queue_msg->authority_list[idpos] = task_id; /* this msg is received by this subscriber */
							_que_buf += meta->o_size;
						}
					}
					else {
						ring_buffer_just_peek(p_ring, _que_buf, peek_pos+sizeof(queue_payload_header_t), meta->o_size);
						queue_msg->authority_list[idpos] = task_id; /* this msg is received by this subscriber */
						_que_buf += meta->o_size;
					}

					if(q->queue_lastmsg_pos[idpos] < q->cur_msg_pos) {
						q->queue_lastmsg_pos[idpos]++;
					}
					else {
						q->queue_lastmsg_pos[idpos] = q->cur_msg_pos;
					}
				}
				while(q->queue_lastmsg_pos[idpos] != q->cur_msg_pos);
				*((uint32_t*)buffer) = que_buf;
			}
			else { /* nonqueue mode */
				p_nonqueue_mode_t nonq = &(p->meta.nonq);
				uorbc_platform_memcpy(buffer, nonq->data, meta->o_size);

				for(int i = 0; i < UORB_MAX_SUB_NUM; ++i) {
					if(p->registered_list[i] != -1) {
						++registered_len;
					}
					if(nonq->authority_list[i] != -1) {
						++authority_len;
					}
				}

				if(authority_len >= registered_len) { /* TODO: this judgement is not strict */
					nonq->published = false;
				}
			}
//				orb_unlock ( atomic_state );
		}

		uorbc_platform_sem_post(& (p->sem));
	}

	return 0;
}
//----------------------------------------------------------------
static int orb_check(int handle)
{
	if(handle < 0) {
		uorbc_platform_log_err("handle < 0");
		return -1;
	}

	int orb_id = (handle >> 4);
	int multi_pos = handle - ((handle >> 4) << 4);

	if(orb_id < 0 || orb_id >= TOTAL_UORBC_NUM ||
	   multi_pos < 0 || multi_pos >= UORB_MULTI_MAX_INSTANCES) {
		uorbc_platform_log_err("error orb_id:%d multi_pos:%d", orb_id, multi_pos);
		return -1;
	}

	p_orb_data_t* orb = uorbc_instance();
	p_orb_data_t p = &(orb[orb_id][multi_pos]);

	int task_id = get_subscriber_id();
	int hit=0;

	hit = get_subscriber_id_pos(p, task_id);
	if(-1==hit) {
		uorbc_platform_log_err("this subcriber(%d) orb_id:%d multi_pos:%d dont subscribe this topic",
		                       task_id, orb_id, multi_pos);
		return -1;
	}

	bool authorised = false;
	if(true == p->queue_mode) { /* queue mode */
		p_queue_mode_t q = &(p->meta.q);
		int msg_span = 0;
		if(q->cur_msg_pos != q->queue_lastmsg_pos[hit]) {
			msg_span = q->cur_msg_pos - q->queue_lastmsg_pos[hit];
		}
		else {
		}

		return msg_span;
	}
	else { /* nonqueue mode */
		p_nonqueue_mode_t nonq = &(p->meta.nonq);
		for(int i = 0; i < UORB_MAX_SUB_NUM; ++i) {
			if(nonq->authority_list[i] == task_id) {
				authorised = true;
				break;
			}
		}

		if(nonq->published && !authorised) {
			return 1;
		}
	}

	return 0;
}
//----------------------------------------------------------------
/* 获取multi   orb个数   */
int  orb_group_count(const struct orb_metadata* meta)
{
	unsigned instance = 0;

	for(int i = 0; i < UORB_MULTI_MAX_INSTANCES; ++i) {
		if(orb_exists(meta, i) == 0) {
			++instance;
		}
	}

	return instance;
}
//----------------------------------------------------------------
void orb_init(void)
{
	uorbc_platform_log("orb_init");

	for(int i = ORBC_SYS_STATE; i < TOTAL_UORBC_NUM; ++i) {
		if(mod_uorbc_utils_ismulti(i)) {
			orb_init_multi(i);
		}
		else {
			orb_init_nomulti(i);
		}
	}
}
//----------------------------------------------------------------


