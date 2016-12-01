#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"
#define MAX 300

struct table_ {
  char * path;
  int ncols;
  type_t *types;
};

void table_create(char* path, int ncols, type_t* types) {
  FILE *f;
  if(ncols<1 || path==NULL || types==NULL)
    return;

  strcat(path,".table");
  f=fopen(path,"w");
  if(f==NULL)
    return;
  fwrite(&ncols,sizeof(int),1,f);
  fwrite(types,sizeof(type_t),ncols,f);
  fclose(f);
  return;
}

table_t* table_open(char* path) {
  table_t *table;
  FILE *f;

  if(path==NULL)
    return NULL;
  table=(table_t *)malloc(sizeof(table_t));
  if(table==NULL)
    return NULL;
  table->path=strdup(path);
  if(table->path==NULL)
    return NULL;

  f=fopen(path,"r");
  if(f==NULL){
    free(table->path);
    free(table);
    return NULL;
  }

  fread(&table->ncols,sizeof(int),1,f);
  table->types=(type_t *)malloc(sizeof(type_t)*table->ncols);
  if(table->types==NULL){
    free(table->path);
    free(table);
    fclose(f);
    return NULL;
  }
  fread(table->types,sizeof(type_t),table->ncols,f);
  fclose(f);
  return table;
}

void table_close(table_t* table) {
  if(table==NULL)
    return;
  free(table->path);
  free(table->types);
  free(table);
  return;
}

int table_ncols(table_t* table) {
  if(table==NULL)
    return -1;
  return table->ncols;
}


char* table_data_path(table_t* table){
  if(table == NULL)
    return;
  return table->path;
}

type_t* table_types(table_t* table) {
  if(table==NULL)
    return;
  return table->types;
}

long table_first_pos(table_t* table) {
  if(table==NULL)
    return;
  return sizeof(int)+table->ncols*sizeof(types_t);
}

long table_last_pos(table_t* table) {
  int  i, pos,aux,posaux;
  char buff[MAX],*baux;
  FILE *f;

  if(table==NULL)
    return -1;

  f=fopen(table->path,"r");
  if(f==NULL)
    return -1;
  pos=table_first_pos(table);
  fseek(f,pos,SEEK_SET);/*we put the pointer in the first record*/
  posaux=pos;
  while(fread(buff,sizeof(char),MAX,f)>0){
    posaux=pos;
    baux=buff;
    for(i=0;i<table->ncols;i++){
      aux=*((int *)baux);/*tamanio de la primera columna*/
      pos=pos+sizeof(int)+aux;/*posicion de antes + int + columna*/
      baux=baux+sizeof(int)+aux;
    }
    fseek(f,pos,SEEK_SET);/*fseek para dejarlo al final de la tupla leida*/
  }
  fclose(f);
  return posaux;

}

record_t* table_read_record(table_t* table, long pos) {
  if(table==NULL)
    return;
}

void table_insert_record(table_t* table, void** values) {
  if(table==NULL)
    return;
}
