#include "operation.h"

/* COUNT OPERATION: operation whose result is the number of results of the source operation */

typedef struct
{
    operation_t* suboperation;
    int        num;
    int        flag;
} operation_count_args_t;

void
operation_count_reset(void* vargs)
{
    operation_count_args_t* args = vargs;
    args->num  = 0;
    args->flag = -1;
    operation_reset(args->suboperation);
}

int operation_count_next(void* vargs)
{
    operation_count_args_t *args = vargs;
    /*We just have one row so we implement a flag*/
    /*if its the second time we call this func, returns 0*/
    if (args->flag == 1)
    {
        return 0;
    }

    args->flag = 1;
    /*we count the number of rows in suboperation*/
    if (args->num == 0)
    {
        while (operation_next(args->suboperation) != 0)
        {
            args->num++;
        }
    }
    /*we succesfully counted the rows*/
    return 1;
}

void* operation_count_get(int col, void* vargs)
{
    operation_count_args_t* args = vargs;

    return &args->num;
}

void operation_count_close(void* vargs)
{
    operation_count_args_t* args = vargs;

    operation_close(args->suboperation);
    free(args);
}

operation_t*
operation_count_create(operation_t* suboperation)
{
    operation_t           * operation;
    operation_count_args_t* args;

    args               = malloc(sizeof(operation_count_args_t));
    args->suboperation = suboperation;
    args->num          = 0;
    args->flag         = -1;
    operation          = malloc(sizeof(operation_t));
    operation->args    = args;
    operation->reset   = operation_count_reset;
    operation->next    = operation_count_next;
    operation->get     = operation_count_get;
    operation->close   = operation_count_close;
    operation->ncols   = suboperation->ncols;
    operation->types   = malloc(operation->ncols * sizeof(type_t));
    memcpy(operation->types, suboperation->types, operation->ncols * sizeof(type_t));

    return operation;
}

