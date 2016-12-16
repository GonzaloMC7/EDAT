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
    record_t *record1, *record2;
    void     **values;

    types = (type_t *) malloc(sizeof(type_t) * 3);
    if (types == NULL)
        return 0;

    types[0] = id;
    types[1] = nombre;
    types[2] = edad;

    /*Probemos primero algunas funciones del table*/
    /*Si no funciona esto, no funciona nada del resto*/
    table_create("Personas", 3, types);

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



    /*Probemos que funciona el record y sus funciones*/

    /*Guardemos espacio para nuestro buffer con el cual rellenaremos los records*/
    values = (void * *) malloc(sizeof(void *) * 3);
    /*Para cada campo ahora*/
    naux      = (int *) malloc(sizeof(int));
    *naux     = 23;
    values[0] = aux;
    values[1] = (char *) malloc(sizeof(char) * (strlen("Pepe") + 1));
    strcpy(values[1], "Pepe");
    naux      = (int *) malloc(sizeof(int));
    *naux     = 2;
    values[2] = naux;

    /*Inicialicemos nuestro primer record*/
    pos     = table_last_pos(tabla);
    pos     = pos + sizeof(int) * 2 + sizeof(char) * (strlen(values[1]) + 1);
    record1 = record_create(values, 3, pos);
    if (record1 != NULL)
    {
        printf("Creado el record1 con exito\n");
        printf("Correcto funcionamiento de record_create y table_last_pos\n");
    }
    else
    {
        printf("Error en el creado de record1\n");
        printf("Funcionamiento errado de record_create o table_last_pos\n");
        goto ERR3;
    }

    /*Reservamos nueva memoria e inicializamos el record2*/
    naux      = (int *) malloc(sizeof(int));
    *naux     = 15;
    values[0] = naux;
    values[1] = (char *) malloc(sizeof(char) * (strlen("Simone") + 1));
    strcpy(values[1], "Simone");
    naux      = (int *) malloc(sizeof(int));
    *naux     = 8;
    values[2] = naux;

    pos     = table_last_pos(tabla);
    pos     = pos + sizeof(int) * 2 + sizeof(char) * (strlen(values[1]) + 1);
    record2 = record_create(values, 3, pos);
    if (record2 != NULL)
    {
        printf("Creado el record2 con exito\n");
        printf("Correcto funcionamiento de record_create y table_last_pos\n");
    }
    else
    {
        printf("Error en el creado de record2\n");
        printf("Funcionamiento errado de record_create o table_last_pos\n");
        goto ERR4;
    }

    /*NECESITAMOS INSERTAR VARIOS RECORDS*/
    /*LEERLOS Y COMPARAR QUE COINCIDEN CON LOS INSERTADOS*/
    /*PROBAR LAS FUNCIONS DE FIRST Y LAST POSITION*/

    record_free(record2);
 ERR4:
    record_free(record1);
 ERR3:
    free(values);
 ERR2:
    table_close(tabla); /*Se prueba el buen funcionamiento de esto con valgrind*/
 ERR1:
    free(types);
}