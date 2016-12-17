#include <stdio.h>
#include <stdlib.h>
#include "record.h"
#include "table.h"
/*
    Crearemos una tabla con la siguiente estructura

    ID    NOMBRE     EDAD
    int   str        int

    En ella guardaremos varias tuplas y probaremos que todo funciona
    como es esperado al extraerlas
 */
int main()
{
    table_t  *tabla;
    type_t   id = INT, edad = INT, nombre = STR;
    type_t   *types, *aux;
    int      i, *naux;
    long     pos;
    record_t *record;
    void     **values;

    types = (type_t *) malloc(sizeof(type_t) * 3);
    if (types == NULL)
        return 0;

    types[0] = id;
    types[1] = nombre;
    types[2] = edad;

    /*Probemos primero algunas funciones del table*/
    /*Si no funciona esto, no funciona nada del resto*/
    table_create("Personas.table", 3, types);

    tabla = table_open("Personas.table");
    if (tabla == NULL)
    {
        printf("ERROR: table_create o table_open devuelve error\n");
        printf("Puede deberse a memoria insuficiente\n");
        goto ERR1;
    }

    /*Probemos las funciones basicas*/
    if (table_ncols(tabla) == 3)
    {
        printf("table_ncols salida correcta\n");
    }
    else
    {
        printf("ERROR: table_ncols salida incorrect\n");
        goto ERR2;
    }
    if (strcmp(table_data_path(tabla), "Personas.table") == 0)
    {
        printf("table_data_path salida correcta\n");
    }
    else
    {
        printf("ERROR: table_data_path salida incorrecta\n");
        goto ERR2;
    }
    /*Probemos que nos devuelven los types correctos*/
    aux = table_types(tabla);
    for (i = 0; i < 3 && types[i] == aux[i]; i++)
        ;
    if (i == 3)
    {
        printf("table_types salida correcta\n");
    }
    else
    {
        printf("ERROR: tabla_types salida incorrecta\n");
        goto ERR2;
    }
    printf("table_create y table_open ejecutadas con exito\n");


    /*Insertemos records en table y probamos records desde table*/
    values = (void * *) malloc(sizeof(void *) * 3);
    /*Para cada campo ahora*/
    naux      = (int *) malloc(sizeof(int));
    *naux     = 23;
    values[0] = naux;
    values[1] = (char *) malloc(sizeof(char) * (strlen("Pepe") + 1));
    strcpy(values[1], "Pepe");
    naux      = (int *) malloc(sizeof(int));
    *naux     = 2;
    values[2] = naux;
    /*insertamos un record*/
    table_insert_record(tabla, values);

    /*cambiemos los valores de values*/
    *naux = 12;
    strcpy(values[1], "Juan");
    *naux = 8;
    /*Insertamos otro record*/
    table_insert_record(tabla, values);

    /*Liberamos los values utilizados*/
    for (i = 0; i < 3; i++)
        free(values[i]);
    free(values);

    printf("\nSalida esperada:\n");
    printf("\nTabla:\n23\tPepe\t2\n12\tJuan\t8\n");
    printf("\nSalida:\n");
    /*POSIBLES FALLOS  EN INSERT O EN FUNCS DE RECORD*/
    /*Extraigamos records e imprimamoslo por pantalla*/
    pos    = table_first_pos(tabla);
    record = table_read_record(tabla, pos);
    printf("\nTabla:\n");
    for (i = 1; i <= 3; i++)
    {
        print_value(stdout, aux[i - 1], record_get(record, i));
        printf("\t");
    }
    printf("\n");
    pos = record_next(record);
    record_free(record);
    /*Abrimos el siguiente record*/
    record = table_read_record(tabla, pos);
    for (i = 1; i <= 3; i++)
    {
        print_value(stdout, aux[i - 1], record_get(record, i));
        printf("\t");
    }
    printf("\n");
    record_free(record);
    printf("\nSi la salida coincide con la esperada todo funciona correctamente\n");

 ERR2:
    table_close(tabla); /*Se prueba el buen funcionamiento de esto con valgrind*/
 ERR1:
    free(types);
}