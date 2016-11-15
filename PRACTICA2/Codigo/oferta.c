#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#define TRUE 1
#define FALSE 0

int main(int argc, char **argv){
  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  SQLHSTMT stmt;
  int num;/*numero de isbns*/
  int boolean, descuento;

  /*REVISAMOS LOS DATOS DE ENTRADA*/
  if(argc<4){
    printf("ERROR, faltan argumentos.\n")
    return 0;
  }

  descuento=argv[1];
  num=argc-4;/*NUmero de isbsns sera el numero de argumentos menos los 4 primeros argumentos*/
  /*FALTAN POR REVISAR que la fecha tiene sentido*/



  /*LETS CONNECT TO THE DATABASE*/
  /* Allocate an environment handle */
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
  /* We want ODBC 3 support */
  SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
  /* Allocate a connection handle */
  SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
  /* Connect to the DSN mydsn */
  ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*) "DRIVER=PostgreSQL ANSI;DATABASE=PRACT_2;SERVER=localhost;PORT=5432;UID=alumnodb;PWD=alumnodb;", SQL_NTS,
                         outstr, sizeof(outstr), &outstrlen,
                         SQL_DRIVER_NOPROMPT);

  /*Probamos que se haya conectado con exito*/
  if(SQL_SUCCEEDED(ret)){
    printf("Conectado a la base de datos\n\n");
  }
  else{
    fprintf(stderr, "Error al conectarse a la base de datos\n");
    odbc_extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
    /* disconnect from database and free up allocated handles */
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
    return 0;
  }

  /*Guardamos memoria para guardar la tabla en stmt*/
  ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  if(!SQL_SUCCEEDED(ret)){
    printf("Error allocating statement\n");
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
    return 0;
  }


  /*REVISEMOS QUE TODOS LOS ISBNS ESTAN EN NUESTRA BASE DE DATOS*/





  /*Desconectamos y liberamos*/
  SQLDisconnect(dbc);
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
  return 0;
}
