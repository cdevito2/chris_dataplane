#include "dp_buffer.h"

dp_buf_t *dp_buf_alloc(size_t capacity)
{
    //allocate struct + data buffer
    dp_buf_t *buf = malloc( sizeof(dp_buf_t));
    if (buf == NULL)
    {
        return NULL;
    }
    //allocate databuffer
    buf->data = malloc(capacity);
    if (buf->data == NULL)
    {
        free(buf);
        return NULL;
    }
    
    memset(buf->data, 0 , capacity);
    //set fields
    buf->capacity = capacity;
    buf->len = 0;

    return buf;
}

void dp_buf_free(dp_buf_t *buf)
{
    //free data and struct
    if (buf != NULL)
    {
        if (buf->data != NULL)
        {
            free(buf->data);
            buf->data = NULL;
        }
        //free(buf);
    }
}

void db_buf_reset(dp_buf_t *buf)
{
    if (buf == NULL){
        return;
    }
    
    //set length to 0
    buf->len = 0;

    if (buf->data == NULL)
    {
        return;
    }
    //clear buffer
    memset(buf->data,0,buf->capacity);
}