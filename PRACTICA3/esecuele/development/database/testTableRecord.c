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
    table_t *tabla;
    type_t  id = INT, edad = INT, nombre = STR;
    type_t  *types, *aux;
    int     i;


    types = (type_t *) malloc(sizeof(type_t) * 3);
    if (types == NULL)
        return 0;

    types[0] = id;
    types[1] = nombre;
    types[2] = edad;

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


    /*NECESITAMOS INSERTAR VARIOS RECORDS*/
    /*LEERLOS Y COMPARAR QUE COINCIDEN CON LOS INSERTADOS*/
    /*PROBAR LAS FUNCIONS DE FIRST Y LAST POSITION*/

 ERR2:
    table_close(tabla); /*Se prueba el buen funcionamiento de esto con valgrind*/
 ERR1:
    free(types);
}