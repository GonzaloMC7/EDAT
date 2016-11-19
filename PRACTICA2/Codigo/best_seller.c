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
  int num;/*numero de best sellers*/
  int boolean=TRUE,i=0,j;
  int *isbns, *copias;
  char titulo[100],buff[1000];

  if(argc!=2){
    printf("ERROR, paso de argumentos incorrecto.\n");
    return 0;
  }
  num=atoi(argv[1]);
  isbns=(int *)malloc(sizeof(int)*num);
  copias=(int *)malloc(sizeof(int)*num);
  if(isbns==NULL || copias==NULL){
    printf("Memoria insuficiente\n");
    return 0;
  }

  /*CONNECTING TO DATABASE*/
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
  /*Hacemos la consulta que nos devuelve una tabla ordenada con los mas vendidos*/
  strcpy(buff,"select \"isbn\", \"num\" from (select \"ISBN\" as isbn,count(*) as num from public.\"Vendido\" as Vendido group by \"ISBN\" ) as cons order by \"num\" DESC");
  SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
  SQLExecute(stmt);
  while(boolean==TRUE && i<num){
    SQLBindCol(stmt,1,SQL_C_SLONG,isbns+i,sizeof(int),NULL);
    SQLBindCol(stmt,2,SQL_C_SLONG,copias+i,sizeof(int),NULL);
    ret=SQLFetch(stmt);
    if(!SQL_SUCCEEDED(ret)){
      boolean=FALSE;
    }
    else{
      i++;
    }
  }
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  /*Quedan los valores de isbns en las tablas isbns y copias*/
  /*desde el 0 hasta el i-1 (sera siempre igual a num-1, */
  /* excepto si num es mas grande que la cantidad de libros en la base*/


  /*Ahora buscamos los titulos y los imprimimos por pantalla*/
  strcpy(buff,"select \"Titulo\" from public.\"Edicion\" where \"ISBN\"=?");
  for(j=0;j<i;j++){
    /*Guardamos memoria para guardar la tabla en stmt*/
    ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if(!SQL_SUCCEEDED(ret)){
      printf("Error allocating statement\n");
      SQLDisconnect(dbc);
      SQLFreeHandle(SQL_HANDLE_DBC, dbc);
      SQLFreeHandle(SQL_HANDLE_ENV, env);
      return 0;
    }
    SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
    SQLBindParameter(stmt,1,SQL_PARAM_INPUT,SQL_C_SLONG,SQL_INTEGER,0,0,isbns+j,0,NULL);
    SQLBindCol(stmt,1,SQL_C_CHAR,titulo,sizeof(titulo),NULL);
    SQLExecute(stmt);
    SQLFetch(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    printf("Posicion:%d\t Titulo:%s\t\t Numero de copias vendidas:%d\n",j+1,titulo,copias[j]);
  }



  free(isbns);
  free(copias);
  SQLDisconnect(dbc);
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
  return 0;
}
