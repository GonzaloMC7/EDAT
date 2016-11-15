#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int main(int argc, char **argv){
  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  SQLHSTMT stmt;
  char aux, buff[1000],name[100],scrname[100], scr[100];


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
  /*We assign the screen name*/
  strcpy(scrname,argv[2]);


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



  if (SQL_SUCCEEDED(ret)) {
    printf("Conectado a la base de datos\n\n");

    /*Ahora veremos si el screen_name esta o no en la base de datos*/
    /*Guardamos memoria para guardar la tabla en stmt*/
    ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if(!SQL_SUCCEEDED(ret)){
      printf("Error allocating statement\n");
      SQLDisconnect(dbc);
      SQLFreeHandle(SQL_HANDLE_DBC, dbc);
      SQLFreeHandle(SQL_HANDLE_ENV, env);
      return 0;
    }

    /*Creamos un string con la consulta*/
    strcpy(buff,"select \"Screen_name\" from public.\"Fidelizado\" where \"Screen_name\"=?");
    SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
    SQLBindParameter(stmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,0,0,scrname,0,NULL);

    /*Realizamos la consulta y la guardamos en stmt*/
    ret=SQLExecute(stmt);
    if(!SQL_SUCCEEDED(ret)){
      printf("Error en la ejecucion de la consulta\n");
      SQLDisconnect(dbc);
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
      SQLFreeHandle(SQL_HANDLE_DBC, dbc);
      SQLFreeHandle(SQL_HANDLE_ENV, env);
      return 0;
    }


    /*Si el programa esta en modo agregar*/
    if(aux=='+'){

        /*Revisamos si la tabla tiene al menos una fila*/
        ret = SQLFetch(stmt);
        /*liberamos la tabla para utilizar despues*/
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        /*si tiene una fila*/
        if(SQL_SUCCEEDED(ret)){
          printf("Error, el screen name ya esta en la base de datos.\n");
          printf("No se ha podido agregar el usuario.\n");
        }else{/*si no tiene, agregamos al usuario*/
          /*Leemos el nombre del usuario*/
          strcpy(name,argv[3]);

          /*Guardamos memoria para guardar la tabla en stmt*/
          ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
          if(!SQL_SUCCEEDED(ret)){
            printf("Error allocating statement\n");
            SQLDisconnect(dbc);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc);
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            return 0;
          }

          /*Creamos la consulta*/
          strcpy(buff, "insert into public.\"Fidelizado\" (\"Nombre\", \"Screen_name\",\"y-n\") values (?,?,'y')");
          SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
          SQLBindParameter(stmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,0,0,name,0,NULL);
          SQLBindParameter(stmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,0,0,scrname,0,NULL);


          /*Realizamos la consulta y la guardamos en stmt*/
          ret=SQLExecute(stmt);
          if(!SQL_SUCCEEDED(ret)){
            printf("Error en la ejecucion de la consulta insert into\n");
            printf("No se ha podido agregar al usuario.\n");
            SQLDisconnect(dbc);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc);
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            return 0;
          }
          else{
            printf("El usuario ha sido agregado con exito\n");
            /*Liberamos la tabla*/
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          }
        }
    }else{/*si el programa esta en modo borrar*/

        /*En caso de que haya algun screenName lo guardamos en scr*/
        SQLBindCol(stmt,1,SQL_C_CHAR, scr, sizeof(scr),NULL);
        /*Hacemos el fetch para ver si hay o no alguna fila*/
        ret = SQLFetch(stmt);
        /*liberamos la tabla para utilizar despues*/
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);


        /*si tiene una fila, el fetch tuvo exito*/
        if(SQL_SUCCEEDED(ret)){

          /*Guardamos memoria para guardar la tabla en stmt*/
          ret=SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
          if(!SQL_SUCCEEDED(ret)){
            printf("Error allocating statement\n");
            SQLDisconnect(dbc);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc);
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            return 0;
          }
          /*creamos la consulta*/
          strcpy(buff,"update public.\"Fidelizado\" set  \"y-n\"='n' where \"Screen_name\"=?");
          SQLPrepare(stmt,(SQLCHAR*)buff,SQL_NTS);
          SQLBindParameter(stmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,0,0,scr,0,NULL);
          /*La ejecutamos*/
          ret=SQLExecute(stmt);

          /*si hubo un error indicalo*/
          if(!SQL_SUCCEEDED(ret)){
            printf("Error en la ejecucion de la consulta  update\n");
            printf("No se ha podido eliminar al usuario.\n");
            SQLDisconnect(dbc);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc);
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            return 0;
          }
          else{
            printf("El usuario ha sido eliminado con exito\n");
            /*Liberamos la tabla*/
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          }
        }else{/*Si el fetch no recogio ninguna fila*/
          printf("Error, ese screen name no esta en la base de datos\n");
        }
    }

    }else {
      fprintf(stderr, "Error al conectarse a la base de datos\n");
      odbc_extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
   }



  /* disconnect from database and free up allocated handles */
  SQLDisconnect(dbc);
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);



  return 0;

}
