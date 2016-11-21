#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <time.h>
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
  Loads in the string title the title of the book with the isbn given
  Price will be load with price of the book or -1 if error
*/
void title_price(SQLHDBC dbc,char *isbn,char title[],float *price){
    SQLHSTMT stmt;
    SQLRETURN ret;
    char query[1000];

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    sprintf(query,"select \"Titulo\",\"Precio\" from public.\"Edicion\""
            " where \"ISBN\"='%s'",isbn);
    SQLBindCol(stmt,1,SQL_C_CHAR,title,sizeof(char)*100,NULL);
    SQLBindCol(stmt,2,SQL_C_FLOAT,price,sizeof(int),NULL);
    SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
    SQLExecute(stmt);
    ret=SQLFetch(stmt);
    if(!SQL_SUCCEEDED(ret)){
      *price=-1;
    }
    SQLFreeHandle(SQL_HANDLE_STMT,stmt);
    return;
}

/*
  Returns the field descuento of a given isbn or 0 if theres nothing in that field
*/
int descDisp(SQLHDBC dbc,char *isbn){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];
  int descuento;

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"select \"descuento\" from public.\"Ofertas\""
          " where \"ISBN\"='%s' and"
          " \"Inicio\"<=current_date and current_date<=\"Fin\"",isbn);
  SQLBindCol(stmt,1,SQL_C_SLONG,&descuento,sizeof(int),NULL);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  SQLExecute(stmt);
  ret=SQLFetch(stmt);
  if(!SQL_SUCCEEDED(ret))
    descuento=0;
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  return descuento;
}

/*
  Returns the field dni of a given screen_name or -1 if theres no dni for that scr
*/
int DNI(SQLHDBC dbc,char *scr){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];
  int dni;
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"select \"Dni\" from public.\"Fidelizado\" "
          "where \"Screen_name\"='%s'",scr);
  SQLBindCol(stmt,1,SQL_C_SLONG,&dni,sizeof(int),NULL);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  SQLExecute(stmt);
  ret=SQLFetch(stmt);
  if(!SQL_SUCCEEDED(ret))
    dni=-1;
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  return dni;
}

/*
  Inserts a Venta with the fields given and 'E' marking the user payed in cash
  in case of error it returns 0;
*/
int insVentas(SQLHDBC dbc,int total,int dni){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"insert into public.\"Ventas\""
          "(\"Precio_Total\", \"Fecha\", \"Efectivo/Tarjeta\", \"DNI\")"
          " values ('%d', current_date, 'E', '%d')",total,dni);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  ret=SQLExecute(stmt);
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  if(!SQL_SUCCEEDED(ret))
    return 0;
  return 1;
}

/*
  Returns the id of a Venta done that day by the dni with the price total
  which we assume its unique or -1 in case of error
*/
int idVenta(SQLHDBC dbc,float total,int dni){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];
  int id;

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"select \"id_venta\" from public.\"Ventas\""
          " where \"Precio_Total\"='%f' and \"Fecha\"=current_date and"
          " \"DNI\"='%d'",total,dni);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  SQLBindCol(stmt,1,SQL_C_SLONG,&id,sizeof(int),NULL);
  SQLExecute(stmt);
  ret=SQLFetch(stmt);
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  if(!SQL_SUCCEEDED(ret))
    return -1;
  return id;
}

/*Hacemos una funcion que introduce una relacion isbn idoferta*/
void insVendido(SQLHDBC dbc,char *isbn, int idventa){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  sprintf(query,"insert into public.\"Vendido\"(\"ISBN\", \"id_venta\")"
  " values ('%s', '%d')",isbn,idventa);
  SQLPrepare(stmt,(SQLCHAR*)query,SQL_NTS);
  ret=SQLExecute(stmt);
  SQLFreeHandle(SQL_HANDLE_STMT,stmt);
  if(!SQL_SUCCEEDED(ret))
    printf("Error al introducir la relacion con el isbn %s\n",isbn);
  return;
}

int main(int argc, char **argv){
  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  int descuento,dni,id_venta,i;
  float precio,total=0;
  char titulo[100];

  /*Revisamos cosas basicas de los argumentos*/
  if(argc<3){
    printf("ERROR, argumentos insuficientes.\n");
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
  /*Revisemos si el screen_name esta en la base de datos*/
  if(isScr(dbc,argv[1])!=0){
    printf("Error, el screen_name no esta en la base de datos.\n");
    goto DESCONECTAR;
  }
  /*Revisemos que todos los isbns estan en nuestra base de datos*/
  if(areIsbn(dbc,argv+2,argc-2)!=0){
    printf("ERROR, algun isbn no esta en nuestra base de datos.\n");
    goto DESCONECTAR;
  }

  /*Calculemos el precio total y vayamos imprimiendo uno a uno los precios de cada libro*/
  for(i=2;i<argc;i++){
      /*Tomamos el titulo y precio del isbn en argumento argv[i]*/
      title_price(dbc,argv[i],titulo,&precio);
      if(precio==-1){/*si ha habido error en la ejecucion de la consulta*/
        printf("Error en la ejecucion de la consulta.\n"
               "No se ha podido ejecutar la compra.\n ");
        goto DESCONECTAR;
      }
      /*Tomamos el descuento que haya para el isbn argv[i]*/
      descuento=descDisp(dbc,argv[i]);
      printf("El libro %s tiene un precio de %.2f euros.\n",
              titulo,precio-precio*((float)descuento/100)-precio*0.1);
      total=total+precio-precio*((float)descuento/100)-precio*0.1;
      /*0.1 por ser usr*/
  }

  /*Sacamos el dni del screen_name dado*/
  dni=DNI(dbc,argv[1]);
  if(dni==-1){
    printf("Error, en la ejecucion de la consulta.\n");
    goto DESCONECTAR;
  }
  /*insertamos la venta del usuario con el dni y el total pagado*/
  if(insVentas(dbc,total,dni)==0){
    printf("Error al introducir la venta\n");
    goto DESCONECTAR;
  }
  printf("Venta introducida con exito.\n");
  /*Sacamos el id de la venta*/
  id_venta=idVenta(dbc,total,dni);
  /*sabemos que no da error porque la acabamos de introducir*/

  /*Ahora hacemos un bucle introduciendo todas las relaciones*/
  for(i=2;i<argc;i++)
    insVendido(dbc,argv[i],id_venta);
  printf("Insertadas relaciones sin mensaje de error.\n");
  DESCONECTAR:
  SQLDisconnect(dbc);
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
  return 0;
}
