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
  int boolean, descuento,i,aux;
  char inicio[100],fin[100];
  char buff[1000];

  /*REVISAMOS LOS DATOS DE ENTRADA*/
  if(argc<4){
    printf("ERROR, faltan argumentos.\n");
    return 0;
  }

  descuento=atoi(argv[1]);
  /*Comprobamos que las fechas son coherentes, asumimos que nos las pasan bien*/
  /*con el formato anyo-mes-dia*/
  if(strcmp(argv[2],argv[3])>0){
    printf("ERROR, las fechas introducidas son incorrectas.\n");
    return 0;
  }



  /*LETS CONNECT TO THE DATABASE*/
  /* Allocate an environment handle */
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
  /* We want ODBC 3 support */
  SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
  /* Allocate a connection handle */
  SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
  /* Connect to the DSN mydsn */
  ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*) "DRIVER=PostgreSQL ANSI;DATABASE=practica2;SERVER=localhost;PORT=5432;UID=andressp05;PWD=salsita;", SQL_NTS,
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




  /*REVISEMOS QUE TODOS LOS ISBNS ESTAN EN NUESTRA BASE DE DATOS*/
  strcpy(buff,"select \"ISBN\" from public.\"Edicion\" where \"ISBN\"=?");
  boolean=TRUE;
  for(i=4;i<argc && boolean==TRUE;i++){
    /*asignamos el isbn a una variable auxiliar*/
    aux=atoi(argv[i]);

    /*Guardamos memoria para guardar la tabla en stmt*/
    ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if(!SQL_SUCCEEDED(ret)){
      printf("Error allocating statement\n");
      SQLDisconnect(dbc);
      SQLFreeHandle(SQL_HANDLE_DBC, dbc);
      SQLFreeHandle(SQL_HANDLE_ENV, env);
      return 0;
    }

    /*preparamos la consulta*/
    SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
    SQLBindParameter(stmt,1,SQL_PARAM_INPUT,SQL_C_SLONG,SQL_INTEGER,0,0,&aux,0,NULL);
    SQLExecute(stmt);

    /*hacemos un fetch*/
    ret=SQLFetch(stmt);
    /*si el fetch falla, el isbn no esta en la tabla*/
    if(!SQL_SUCCEEDED(ret)){
      boolean=FALSE;
    }
    /*liberamos la tabla para utilizar despues*/
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  }

  /*si algun isbn no esta en la tabla, sal del programa*/
  if(boolean==FALSE){
    /*Desconectamos y liberamos*/
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
    printf("ERROR, algun isbn no esta en nuestra base de datos.\n");
    return 0;
  }

 	/*Creamos la consulta*/
 	strcpy(buff, "insert into public.\"Ofertas\" (\"Inicio\", \"Fin\",\"descuento\",\"isbn\") values (?,?,?,?)");
 	SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
    for(i=4;i<argc; i++){
    	/*Guardamos memoria para guardar la tabla en stmt*/
    	ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    	if(!SQL_SUCCEEDED(ret)){
      		printf("Error allocating statement\n");
      		SQLDisconnect(dbc);
      		SQLFreeHandle(SQL_HANDLE_DBC, dbc);
      		SQLFreeHandle(SQL_HANDLE_ENV, env);
    		return 0;
    	}
        
        aux=atoi(argv[i]);
        SQLBindParameter(stmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,0,0,inicio,0,NULL);
      	SQLBindParameter(stmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,0,0,fin,0,NULL);
       	SQLBindParameter(stmt,3,SQL_PARAM_INPUT,SQL_C_SLONG,SQL_INTEGER,0,0,&descuento,0,NULL);        	
       	SQLBindParameter(stmt,i,SQL_PARAM_INPUT,SQL_C_SLONG,SQL_INTEGER,0,0,&aux,0,NULL);
		
		/*Realizamos la consulta y la guardamos en stmt*/
        ret=SQLExecute(stmt);
        if(!SQL_SUCCEEDED(ret)){
        	printf("Error en la ejecucion de la consulta insert into\n");
            printf("No se ha podido agregar la oferta.\n");
            SQLDisconnect(dbc);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc);
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            return 0;
        }
        else{
            printf("La oferta ha sido agregado con exito\n");
            /*Liberamos la tabla*/
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        }
    }

  /*Desconectamos y liberamos*/
  SQLDisconnect(dbc);
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
  return 0;
}
