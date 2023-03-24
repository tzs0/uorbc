#include "mod_uorbc_ringbuffer.h"
#include "mod_uorbc_platform.h"



//------------------------------------------------------
static ring_buffer_size_t ring_buffer_num_items(ring_buffer_t* buffer);
static uint8_t ring_buffer_is_empty(ring_buffer_t* buffer);
//------------------------------------------------------

/**
 * @file
 * Implementation of ring buffer functions.
 */

/* 大于in_data且离in_data最近的2的n次方 */
uint32_t ring_buffer_to_the_power_of_2(uint32_t in_data)
{
	uint32_t tmp = in_data;
	while(1) {
		if(0 == (tmp & (tmp - 1))) {
			return tmp;
		}
		tmp++;
	}
}

ring_buffer_t* ring_buffer_init(size_t queue_size, size_t obj_size)
{
	uint32_t size_q = queue_size * obj_size;
	uint32_t size_new = ring_buffer_to_the_power_of_2(size_q);

	uorbc_platform_log("queue_size(%d)*obj_size(%d):%d size_new:%d", queue_size, obj_size, size_q, size_new);

	ring_buffer_t* p_ring = uorbc_platform_malloc(2 * sizeof(ring_buffer_size_t) + size_new * sizeof(size_t));
	if(NULL != p_ring) {
		memset(p_ring, 0x00, (2 * sizeof(ring_buffer_size_t) + size_new * sizeof(size_t)));
		p_ring->tail_index = 0;
		p_ring->head_index = 0;
		p_ring->buffer_size = size_new;
		p_ring->buffer_mask = p_ring->buffer_size - 1;
		uorbc_platform_log("buffer_size:%d buffer_mask:%d", p_ring->buffer_size, p_ring->buffer_mask);
		return p_ring;
	}

	return NULL;
}

/* 获取ring buffer的大小 */
size_t ring_buffer_get_size(ring_buffer_t* p)
{
	return p->buffer_size;
}


void ring_buffer_queue(ring_buffer_t* buffer, char data)
{
	/* Is buffer full? */
	if(ring_buffer_is_full(buffer)) {
		/* Is going to overwrite the oldest byte */
		/* Increase tail index */
//		uorbc_platform_log( "queue full" );
		buffer->tail_index = ((buffer->tail_index + 1) & buffer->buffer_mask);
	}

//	uorbc_platform_log( "data:%d", data );
//	uorbc_platform_log( "head_index:%d", buffer->head_index );

	/* Place data in buffer */
	buffer->buffer[buffer->head_index] = data;
	buffer->head_index = ((buffer->head_index + 1) & buffer->buffer_mask);

//	uorbc_platform_log( "buffer_mask:%d head_index:%d", buffer->buffer_mask, buffer->head_index );
}

void ring_buffer_queue_arr(ring_buffer_t* buffer, const char* data, ring_buffer_size_t size)
{
//	uorbc_platform_log( "in data:(%d)", size );
//	uorbc_platform_log_bin( data, size );

	/* Add bytes; one by one */
	ring_buffer_size_t i;
	for(i = 0; i < size; i++) {
		ring_buffer_queue(buffer, data[i]);
	}
}

uint8_t ring_buffer_dequeue(ring_buffer_t* buffer, char* data)
{
	if(ring_buffer_is_empty(buffer)) {
		/* No items */
//		uorbc_platform_log_err( "queue empty" );
		return 0;
	}

	*data = buffer->buffer[buffer->tail_index];
	buffer->tail_index = ((buffer->tail_index + 1) & buffer->buffer_mask);
	return 1;
}

ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t* buffer, char* data, ring_buffer_size_t len)
{
	if(ring_buffer_is_empty(buffer)) {
		/* No items */
//		uorbc_platform_log_err( "queue empty" );
		return 0;
	}

	char* data_ptr = data;
	ring_buffer_size_t cnt = 0;
	while((cnt < len) && ring_buffer_dequeue(buffer, data_ptr)) {
		cnt++;
		data_ptr++;
	}

//	uorbc_platform_log( "out data:(%d) cnt:%d", len, cnt );
//	uorbc_platform_log_bin( data, len );

	return cnt;
}

/* 从指定位置获取len长度的buffer内容 */
ring_buffer_size_t ring_buffer_just_peek(ring_buffer_t* buffer, char* data, ring_buffer_size_t pos, ring_buffer_size_t len)
{
	if(ring_buffer_is_empty(buffer)) {
		/* No items */
		uorbc_platform_log_err("queue empty");
		return 0;
	}

//	uorbc_platform_log("peek_pos:%d len:%d", pos, len);

	char* data_ptr = data;
	ring_buffer_size_t cnt = 0;
	ring_buffer_size_t peek_pos = pos;
	while(cnt < len) {
		*data_ptr = buffer->buffer[peek_pos];
		peek_pos = ((peek_pos + 1) & buffer->buffer_mask);

		cnt++;
		data_ptr++;
	}

//	uorbc_platform_log( "out data:(%d) cnt:%d", len, cnt );
//	uorbc_platform_log_bin( data, len );

	return cnt;
}

uint8_t* ring_buffer_pos_pointer(ring_buffer_t* buffer, ring_buffer_size_t pos)
{
	return (uint8_t*)&(buffer->buffer[pos]);
}

uint8_t ring_buffer_peek(ring_buffer_t* buffer, char* data, ring_buffer_size_t index)
{
	if(index >= ring_buffer_num_items(buffer)) {
		/* No items at index */
		return 0;
	}

	/* Add index to pointer */
	ring_buffer_size_t data_index = ((buffer->tail_index + index) & buffer->buffer_mask);
	*data = buffer->buffer[data_index];
	return 1;
}

/**
 * Returns whether a ring buffer is full.
 * @param buffer The buffer for which it should be returned whether it is full.
 * @return 1 if full; 0 otherwise.
 */
uint8_t ring_buffer_is_full(ring_buffer_t* buffer)
{
	return ((buffer->head_index - buffer->tail_index) & buffer->buffer_mask) == buffer->buffer_mask;
}

/* ringbuffer剩余大小 */
ring_buffer_size_t ring_buffer_left_space(ring_buffer_t* buffer)
{
//	uorbc_platform_log("buffer_mask:%d ring_buffer_num_items:%d", buffer->buffer_mask, ring_buffer_num_items( buffer ));

	return (buffer->buffer_mask - ring_buffer_num_items(buffer));
}


/**
 * Returns the number of items in a ring buffer.
 * @param buffer The buffer for which the number of items should be returned.
 * @return The number of items in the ring buffer.
 */
static ring_buffer_size_t ring_buffer_num_items(ring_buffer_t* buffer)
{
//	uorbc_platform_log("head_index:%d tail_index:%d buffer_mask:%d", buffer->head_index, buffer->tail_index, buffer->buffer_mask);
//	uorbc_platform_log("ring_buffer_num_items:%d", ( ( buffer->head_index - buffer->tail_index ) & buffer->buffer_mask ));
	return ((buffer->head_index - buffer->tail_index) & buffer->buffer_mask);
}

/**
 * Returns whether a ring buffer is empty.
 * @param buffer The buffer for which it should be returned whether it is empty.
 * @return 1 if empty; 0 otherwise.
 */
static uint8_t ring_buffer_is_empty(ring_buffer_t* buffer)
{
	return (buffer->head_index == buffer->tail_index);
}

