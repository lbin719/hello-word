#ifndef _MJ8000_H
#define _MJ8000_H

#define UART4_RXDMA_BUFFER_SIZE (255)

void mj8000_init(void);
void mj8000_task_handle(void);

#endif /* _MJ8000_H */

