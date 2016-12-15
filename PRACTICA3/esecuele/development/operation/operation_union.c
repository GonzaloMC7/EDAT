
#include "operation.h"

/* UNION OPERATION: concatenation of the results of two operations with the same columns */

typedef struct
{
    operation_t* operation1;
    operation_t* operation2;
} operation_union_args_t;

void
operation_union_reset(void* vargs)
{
    operation_union_args_t* args = vargs;

    operation_reset(args->operation1);
    operation_reset(args->operation2);
}

int operation_union_next(void* vargs)
{
    operation_union_args_t *args = vargs;
    int                    ret;
    ret = operation_next(args->operation1);
    /*if there is something in the first operation return it */
    if (ret != 0)
        return ret;
    /*if theres nothing we check the second operation*/
    return operation_next(args->operation2);
}

void* operation_union_get(int col, void* vargs)
{
    operation_union_args_t* args       = vargs;
    operation_t           * operation1 = args->operation1;
    operation_t           * operation2 = args->operation2;
}

void operation_union_close(void* vargs)
{
    operation_union_args_t* args = vargs;

    operation_close(args->operation1);
    operation_close(args->operation2);
    free(args);
}

operation_t*
operation_union_create(operation_t* operation1, operation_t* operation2)
{
    operation_t           * operation;
    operation_union_args_t* args;

    args             = malloc(sizeof(operation_union_args_t));
    args->first_next = 1;
    args->operation1 = operation1;
    args->operation2 = operation2;

    operation        = malloc(sizeof(operation_t));
    operation->args  = args;
    operation->reset = operation_union_reset;
    operation->next  = operation_union_next;
    operation->get   = operation_union_get;
    operation->close = operation_union_close;
    operation->ncols = operation_ncols(operation1);
    operation->types = malloc(operation->ncols * sizeof(type_t));
    memcpy(operation->types, operation1->types, operation1->ncols * sizeof(type_t));
}

