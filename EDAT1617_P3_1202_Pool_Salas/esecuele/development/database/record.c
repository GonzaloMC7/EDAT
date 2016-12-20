#include "record.h"


struct record_
{
    int  ncols;
    void **values;
    long next;
};


record_t* record_create(void** values, int ncols, long next)
{
    int      i, j;
    record_t *record;
    if (values == NULL || ncols < 1 || next < 0)
        return NULL;

    record = (record_t *) malloc(sizeof(record_t));
    if (record == NULL)
        return NULL;
    record->next   = next;
    record->ncols  = ncols;
    record->values = values;

    return record;
}

void* record_get(record_t* record, int n)
{
    if (record == NULL || n > record->ncols)
        return NULL;
    return record->values[n];
}

long record_next(record_t* record)
{
    if (record == NULL)
        return -1;
    return record->next;
}

void record_free(record_t* record)
{
    int i;
    if (record == NULL)
        return;
    for (i = 0; i < record->ncols; i++)
    {
        free(record->values[i]);
    }
    free(record->values);
    free(record);
    return;
}
