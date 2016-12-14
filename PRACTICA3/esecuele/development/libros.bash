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
edicion SEQUENTIAL 1 STR 
