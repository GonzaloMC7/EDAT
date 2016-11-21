#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>

/*
  Returns: 0 if the screename is already in the database or 1 if not.
*/
int isScr(SQLHDBC dbc, char *scr){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"select \"Screen_name\" from public.\"Fidelizado\" "
          "where \"Screen_name\"='%s'",scr);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  SQLExecute(stmt);
  ret=SQLFetch(stmt);
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  if(SQL_SUCCEEDED(ret))/*si el fetch es exitoso hay una fila*/
    return 0;
  return 1;
}

/*
  Aniade un usuario en la tabla con el scr como screenname y con nombre name
  Si el scr ya esta en la tabla imprime un mensaje de error y sale
*/
void addUsr(SQLHDBC dbc, char *scr, char *name){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];

  if(isScr(dbc,scr)==0){
    printf("Error, el screen name ya esta en la base de datos.\n");
    printf("No se ha podido agregar el usuario.\n");
    return;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"insert into public.\"Fidelizado\" (\"Nombre\","
                " \"Screen_name\",\"y-n\") values ('%s','%s','y')",name,scr);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  ret=SQLExecute(stmt);
  if(!SQL_SUCCEEDED(ret))
    printf("Error en la ejecucion de la consulta.\n"
           "El usuario no ha sido agregado.\n");
  else
    printf("Usuario agregado con exito.\n");
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  return;
}

/*
  Si hay un usuario con el scr como screenname pone su columna y-n a n
  indicando que ha sido borrado
*/
void delUsr(SQLHDBC dbc, char *scr){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];

  if(isScr(dbc,scr)==1){
    printf("El screen_name dado no existe\n No ha sido borrado el usuario\n");
    return;
  }
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"update public.\"Fidelizado\" set  \"y-n\"='n'"
         " where \"Screen_name\"='%s'",scr);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  ret=SQLExecute(stmt);
  if(!SQL_SUCCEEDED(ret))
    printf("Error en la ejecucion de la consulta.\n"
           "El usuario no ha sido borrado.\n");
  else
    printf("Usuario borrado con exito.\n");
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  return;
}


int main(int argc, char **argv){
  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  char aux;

  if(argc<3){
    printf("Error en el paso de argumentos, faltan argumentos.\n");
    return 0;
  }
  /*First we read the character +/- and the Screen_name*/
  /*We check the pass of the first argument is correct*/
  if(strlen(argv[1])!=1 || (argv[1][0]!='+' && argv[1][0]!='-')){
    printf("Error en el paso del caracter +/-\n");
    return 0;
  }
  aux=argv[1][0];

  /*CONNECTION*/
  /* Allocate an environment handle */
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
  /* We want ODBC 3 support */
  SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
  /* Allocate a connection handle */
  SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
  /* Connect to the DSN mydsn */
  ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*) "DRIVER=PostgreSQL ANSI;"
                        "DATABASE=PRACT_2;SERVER=localhost;PORT=5432;"
                        "UID=alumnodb;PWD=alumnodb;", SQL_NTS, outstr,
                        sizeof(outstr), &outstrlen, SQL_DRIVER_NOPROMPT);
  /*Probamos la conexion*/
  if(SQL_SUCCEEDED(ret)){
    printf("Conectado a la base de datos\n\n");
  }
  else{
    printf("Error en la conexión con la base de datos\n");
    goto DESCONECTAR;
  }

  /*Si el programa esta en modo agregar*/
  if(aux=='+'){
      if(argv[2]==NULL || argv[3]==NULL)
        goto DESCONECTAR;
      addUsr(dbc,argv[2],argv[3]);/*Agregamos el usuario*/
  }else{/*si el programa esta en modo borrar*/
      if(argv[2]==NULL)
        goto DESCONECTAR;
      delUsr(dbc,argv[2]);/*borra el usuario*/
  }

  DESCONECTAR:
  /* disconnect from database and free up allocated handles */
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
  SQLDisconnect(dbc);
  return 0;
}
