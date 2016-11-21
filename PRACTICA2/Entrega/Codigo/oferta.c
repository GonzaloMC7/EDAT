#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>

/*
  Parameters: dbc , array of isbns, size is the size of the array
  Returns:
    0 if all isbns are in the database and 1 otherwise
*/
int areIsbn(SQLHDBC dbc,char **isbns,int size){
  SQLRETURN ret;
  SQLHSTMT stmt;
  char query[1000];
  int i;

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  for(i=0;i<size;i++){
    sprintf(query,"select \"ISBN\" from public.\"Edicion\""
            " where \"ISBN\"='%s'",isbns[i]);
    SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
    SQLExecute(stmt);
    ret=SQLFetch(stmt);
    SQLCloseCursor(stmt);
    if(!SQL_SUCCEEDED(ret)){
      SQLFreeHandle(SQL_HANDLE_STMT,stmt);
      return 1;
    }
  }
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  return 0;
}

/*
  Inserts offers in the isbns give, with the descuento given and in the
  dates given.
  Parameters: array of isbns, size is the size of the array,
              descuento is the field descuento, idate starting date,
              edate ending date;
*/
void insOffers(SQLHDBC dbc, char **isbns, int size, int descuento, char *idate, char *edate){
  SQLRETURN ret;
  SQLHSTMT stmt;
  char query[1000];
  int i;

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  for(i=0;i<size;i++){
    sprintf(query,"insert into public.\"Ofertas\" "
            "(\"Inicio\", \"Fin\",\"descuento\",\"ISBN\")"
            " values ('%s','%s','%d','%s')",idate,edate,descuento,isbns[i]);
    SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
    ret=SQLExecute(stmt);
    SQLCloseCursor(stmt);
    if(!SQL_SUCCEEDED(ret)){
      printf("Error al introducir una oferta.\n");
      SQLFreeHandle(SQL_HANDLE_STMT,stmt);
      return;
    }
  }
  printf("Todas las ofertas han sido agregadas con exito\n");
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  return;
}

int main(int argc, char **argv){
  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  int descuento;


  /*REVISAMOS LOS DATOS DE ENTRADA*/
  if(argc<5){
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
  /*REVISEMOS QUE TODOS LOS ISBNS ESTAN EN NUESTRA BASE DE DATOS*/
  if(areIsbn(dbc,argv+4, argc-4)!=0){
    printf("Algun ISBN no esta en nuestra base de datos.\n"
           "No se han podido introducir las ofertas\n");
    goto DESCONECTAR;
  }
  /*insertamos ofertas*/
  insOffers(dbc,argv+4,argc-4,descuento,argv[2],argv[3]);

  DESCONECTAR:
  /*Desconectamos y liberamos*/
  SQLDisconnect(dbc);
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
  return 0;
}
