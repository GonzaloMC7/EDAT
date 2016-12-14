#include "operation.h"

/* COUNT OPERATION: operation whose result is the number of results of the source operation */

typedef struct {
    operation_t* suboperation;
    int count;
} operation_count_args_t;

void
operation_count_reset(void* vargs) {
    operation_count_args_t* args = vargs;

    operation_reset(args->suboperation);
}

int operation_count_next(void* vargs) {
    /* to be implemented */
}

void* operation_count_get(int col, void* vargs) {
    operation_count_args_t* args = vargs;

    return operation_get(col, args->suboperation);
}

void operation_count_close(void* vargs) {
    operation_count_args_t* args = vargs;

    operation_close(args->suboperation);
    free(args);
}

operation_t*
operation_count_create(operation_t* suboperation) {
    operation_t* operation;
    operation_count_args_t* args;

    args = malloc(sizeof(operation_count_args_t));
    args->suboperation = suboperation;
    args->count = 0 /*tampoco estoy seguro*/

    operation = malloc(sizeof(operation_t));
    operation->args = args;
    operation->reset = operation_count_reset;
    operation->next = operation_count_next;
    operation->get = operation_count_get;
    operation->close = operation_count_close;
    operation->ncols = suboperation->ncols;
    operation->types = malloc(operation->ncols * sizeof(type_t));
    memcpy(operation->types, suboperation->types, operation->ncols * sizeof(type_t));

    return operation;
}

