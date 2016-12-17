#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"
#define MAX    300

struct table_
{
    char   * path;
    int    ncols;
    type_t *types;
};

void table_create(char* path, int ncols, type_t* types)
{
    FILE *f;
    if (ncols < 1 || path == NULL || types == NULL)
        return;

    f = fopen(path, "w");
    if (f == NULL)
        return;
    /*Escribimos cabecera de fichero con la estructura
       de la tabla*/
    fwrite(&ncols, sizeof(int), 1, f);
    fwrite(types, sizeof(type_t), ncols, f);
    fclose(f);
    return;
}

table_t* table_open(char* path)
{
    table_t *table;
    FILE    *f;

    if (path == NULL)
        return NULL;
    table = (table_t *) malloc(sizeof(table_t));
    if (table == NULL)
        return NULL;
    table->path = (char *) malloc(sizeof(char) * (1 + strlen(path)));
    strcpy(table->path, path);
    if (table->path == NULL)
        return NULL;
    f = fopen(path, "r");
    if (f == NULL)
    {
        free(table->path);
        free(table);
        return NULL;
    }
    fread(&table->ncols, sizeof(int), 1, f);
    table->types = (type_t *) malloc(sizeof(type_t) * table->ncols);
    if (table->types == NULL)
    {
        free(table->path);
        free(table);
        fclose(f);
        return NULL;
    }
    fread(table->types, sizeof(type_t), table->ncols, f);
    fclose(f);
    return table;
}

void table_close(table_t* table)
{
    if (table == NULL)
        return;
    free(table->path);
    free(table->types);
    free(table);
    return;
}

int table_ncols(table_t* table)
{
    if (table == NULL)
        return -1;
    return table->ncols;
}


char* table_data_path(table_t* table)
{
    if (table == NULL)
        return NULL;
    return table->path;
}

type_t* table_types(table_t* table)
{
    if (table == NULL)
        return NULL;
    return table->types;
}

long table_first_pos(table_t* table)
{
    if (table == NULL)
        return -1;
    return sizeof(int) + table->ncols * sizeof(type_t);
}

long table_last_pos(table_t* table)
{
    FILE *f;
    long aux;
    if (table == NULL)
        return -1;

    f = fopen(table->path, "r");
    if (f == NULL)
    {
        return -1;
    }
    fseek(f, 0, SEEK_END); /*fseek para dejar el puntero al final del                           fichero*/
    aux = ftell(f);        /*guardamos la posicion del fichero*/
    fclose(f);
    return aux;
}

record_t* table_read_record(table_t* table, long pos)
{
    record_t *rec = NULL; /*record a guardar*/
    FILE     *f;          /*direccion al fichero de la tabla*/
    int      i, j;
    size_t   tam;
    long     auxpos;
    type_t   type;
    void     **buff;

    if (table == NULL)
        return NULL;
    /*Abrimos el fichero*/
    f = fopen(table->path, "r");
    if (f == NULL)
        return NULL;

    /*Leemos la informacion de la tupla*/

    /*Guardamos el espacio para el numero de columnas*/
    buff = (void * *) malloc(sizeof(void *) * table->ncols);
    if (buff == NULL)
        goto ERR1;
    /*Movemos el puntero a la posicion dada*/
    fseek(f, pos, SEEK_SET);
    /*Para cada columna*/
    for (i = 0; i < table->ncols; i++)
    {
        /*Tamaño a leer*/
        fread(&tam, sizeof(size_t), 1, f);
        /*Guardamos en la primera con el espacio necesario*/
        buff[i] = (void *) malloc(tam);
        if (buff[i] == NULL)
        {
            for (j = 0; j < i; j++)
            {
                free(buff[j]);
            }
            goto ERR2;
        }
        fread(buff[i], tam, 1, f);
    }
    /*Creamos el record*/
    /*Buscamos la posicion del siguiente*/
    auxpos = ftell(f);
    rec    = record_create(buff, table->ncols, auxpos);

    /*cerramos fichero y liberamos el buff generico*/
    /*No liberamos cada columna ya que el record apunta*/
    /*directamente a estas columnas*/
 ERR2:
    free(buff);
 ERR1:
    fclose(f);
    return rec;
}

void table_insert_record(table_t* table, void** values)
{
    int    i;
    size_t tam;
    FILE   *f;
    if (table == NULL || values == NULL)
        return;
    /*Abrimos el fichero apuntando al final*/
    f = fopen(table->path, "a");
    if (f == NULL)
        return;
    /*Escribimos cols una a una*/
    for (i = 0; i < table->ncols; i++)
    {
        /*Escribimos su tamano*/
        tam = value_length(table->types[i], values[i]);

        fwrite(&tam, sizeof(long), 1, f);
        /*Escribimos el dato*/
        fwrite(values[i], tam, 1, f);
    }
    fclose(f);
    return;
}















