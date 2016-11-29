#include "record.h"


struct par_{
  int size;
  void *value;
}par;

struct record_ {
  int ncols;
  par *pares;
  long next;
};


record_t* record_create(void** values, int ncols, long next) {
  int i,j;
  record_t *record;
  if(values==NULL || ncols<1 || next < 0)
    return NULL;

  record=(record *)malloc(sizeof(record_t));
  if(record==NULL)
    return NULL;
  record->next=next;
  record->ncols=ncols;
  record->pares=(par *)malloc(sizeof(par)*ncols);
  if(record->pares==NULL){
    free(record);
    return NULL;
  }
  for(i=0;i<ncols;i++){
    record->pares[i].value=values[i];
    record->pares[i].size=sizeof(values[i]);
  }
  return record;
}

void* record_get(record_t* record, int n) {
  if(record==NULL || n>record->ncols)
    return NULL;
  return record->pares[i].value;
}

long record_next(record_t* record) {
  if(record==NULL)
    return -1;
  return record->next;
}

void record_free(record_t* record) {
  int i;
  if(record==NULL)
    return;
  for(i=0;i<record->ncols;i++){
    free(record->pares[i].value);
  }
  free(record->pares);
  free(record);
  return;
}
