#include <inttypes.h>
#include <stddef.h>
/**
 * @file
 * Prototypes and structures for the ring buffer module.
 */

#ifndef __MOD_UORBC_RINGBUFFER_H_
#define __MOD_UORBC_RINGBUFFER_H_


typedef size_t ring_buffer_size_t;

typedef struct {
	ring_buffer_size_t tail_index;
	ring_buffer_size_t head_index;
	ring_buffer_size_t buffer_size;
	ring_buffer_size_t buffer_mask;
	char buffer[0];
} ring_buffer_t;

/**
 * Initializes the ring buffer pointed to by <em>buffer</em>.
 * This function can also be used to empty/reset the buffer.
 * @param buffer The ring buffer to initialize.
 */
ring_buffer_t* ring_buffer_init(size_t queue_size, size_t obj_size);

/**
 * Adds a byte to a ring buffer.
 * @param buffer The buffer in which the data should be placed.
 * @param data The byte to place.
 */
void ring_buffer_queue(ring_buffer_t* buffer, char data);

/**
 * Adds an array of bytes to a ring buffer.
 * @param buffer The buffer in which the data should be placed.
 * @param data A pointer to the array of bytes to place in the queue.
 * @param size The size of the array.
 */
void ring_buffer_queue_arr(ring_buffer_t* buffer, const char* data, ring_buffer_size_t size);

/**
 * Returns the oldest byte in a ring buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @return 1 if data was returned; 0 otherwise.
 */
uint8_t ring_buffer_dequeue(ring_buffer_t* buffer, char* data);

/**
 * Returns the <em>len</em> oldest bytes in a ring buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of bytes to return.
 * @return The number of bytes returned.
 */
ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t* buffer, char* data, ring_buffer_size_t len);
/**
 * Peeks a ring buffer, i.e. returns an element without removing it.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @param index The index to peek.
 * @return 1 if data was returned; 0 otherwise.
 */
uint8_t ring_buffer_peek(ring_buffer_t* buffer, char* data, ring_buffer_size_t index);

/**
 * Returns whether a ring buffer is full.
 * @param buffer The buffer for which it should be returned whether it is full.
 * @return 1 if full; 0 otherwise.
 */
uint8_t ring_buffer_is_full(ring_buffer_t* buffer);

ring_buffer_size_t ring_buffer_left_space(ring_buffer_t* buffer);
size_t ring_buffer_get_size(ring_buffer_t* p);
ring_buffer_size_t ring_buffer_just_peek(ring_buffer_t* buffer, char* data, ring_buffer_size_t pos, ring_buffer_size_t len);
uint8_t* ring_buffer_pos_pointer(ring_buffer_t* buffer, ring_buffer_size_t pos);
uint32_t ring_buffer_to_the_power_of_2(uint32_t in_data);


#endif /* RINGBUFFER_H */

