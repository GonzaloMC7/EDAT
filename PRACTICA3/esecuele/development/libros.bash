#!/bin/bash

COMMAND="./esecuele"

rm -rf libros_db

#Crea la Base de Datos Libros
$COMMAND createdb libros_db

#Inserta las tablas de la base de datos
$COMMAND define libros_db << EOF
TABLE edicion 3 INT STR INT
TABLE usuarios 2 INT STR
TABLE ventas 2 INT INT
EOF

##Puebla tablas
$COMMAND insert libros_db << EOF
COPY edicion ./libros_files/edicion.txt
COPY usuarios ./libros_files/usuarios.txt
COPY ventas ./libros_files/ventas.txt
EOF

## Lista de libros comprados por "Jack"
$COMMAND query libros_db << EOF
usuarios SEQUENTIAL 1 STR Jack C_COLEQCTE SELECT INT 0 P_COL STR 1 P_COL PROJECT ventas SEQUENTIAL PRODUCT 1 1 C_COLEQCOL SELECT INT 0 P_COL PROJECT edicion SEQUENTIAL PRODUCT 0 3 C_COLEQCOL SELECT INT 0 P_COL STR 1 P_COL PROJECT
