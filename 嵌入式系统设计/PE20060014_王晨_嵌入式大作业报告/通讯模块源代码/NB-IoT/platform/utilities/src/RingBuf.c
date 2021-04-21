/*************************************************************************
  \file				RingBuf.c
  \brief
	Source file of the ring buffer module.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	Dec. 13, 2017		Johnason - Created

**************************************************************************/

#include "RingBuf.h"
#include "fsl_interrupt_manager.h"

/*
 *  ======== RingBuf_construct ========
 */
void RingBuf_construct(RingBuf_Handle object, unsigned char *bufPtr,
    size_t bufSize)
{
    object->buffer = bufPtr;
    object->length = bufSize;
    object->count = 0;
    object->head = bufSize - 1;
    object->tail = 0;
    object->maxCount = 0;
}

/*
 *  ======== RingBuf_get ========
 */
int RingBuf_get(RingBuf_Handle object, unsigned char *data)
{
	INT_SYS_DisableIRQGlobal();

    if (!object->count) {
    	INT_SYS_EnableIRQGlobal();
        return -1;
    }

    *data = object->buffer[object->tail];
    object->tail = (object->tail + 1) % object->length;
    object->count--;

    INT_SYS_EnableIRQGlobal();

    return (object->count);
}

/*
 *  ======== RingBuf_getCount ========
 */
int RingBuf_getCount(RingBuf_Handle object)
{
    return (object->count);
}

/*
 *  ======== RingBuf_isFull ========
 */
bool RingBuf_isFull(RingBuf_Handle object)
{
    return (object->count == object->length);
}

/*
 *  ======== RingBuf_getMaxCount ========
 */
int RingBuf_getMaxCount(RingBuf_Handle object)
{
    return (object->maxCount);
}

/*
 *  ======== RingBuf_peek ========
 */
int RingBuf_peek(RingBuf_Handle object, unsigned char *data)
{
    int          retCount;

    INT_SYS_DisableIRQGlobal();

    *data = object->buffer[object->tail];
    retCount = object->count;

    INT_SYS_EnableIRQGlobal();

    return (retCount);
}

/*
 *  ======== RingBuf_put ========
 */
int RingBuf_put(RingBuf_Handle object, unsigned char data)
{
    unsigned int next;

    INT_SYS_DisableIRQGlobal();

    if (object->count != object->length) {
        next = (object->head + 1) % object->length;
        object->buffer[next] = data;
        object->head = next;
        object->count++;
        object->maxCount = (object->count > object->maxCount) ?
                            object->count :
                            object->maxCount;
    }
    else {

    	INT_SYS_EnableIRQGlobal();
        return (-1);
    }

    INT_SYS_EnableIRQGlobal();

    return (object->count);
}
