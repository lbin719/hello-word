#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

typedef struct
{
     uint8_t *buf;    /*环形缓冲区        */
     uint32_t size;     /*环形缓冲区        */
     uint32_t front;    /*头指针            */
     uint32_t rear;     /*尾指针            */
}ring_buf_t;

uint32_t ring_buf_create(ring_buf_t *r, uint8_t *buf, uint32_t size);

void ring_buf_clr(ring_buf_t *r);

uint32_t ring_buf_len(ring_buf_t *r);

uint32_t ring_buf_put(ring_buf_t *r, uint8_t *buf, uint32_t len);

uint32_t ring_buf_get(ring_buf_t *r, uint8_t *buf, uint32_t len);

#endif
