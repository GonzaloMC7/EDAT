#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*Fomato de entrada: nombre dni tarjeta de Credito*/
/*El dni sin letra*/
int main(int argc, char **argv){
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret;
  long int dni, ccard;
  char name[50];

  /*CONNECT*/
  ret=odbc_connect(&env,&dbc);
  if(!SQL_SUCCEEDED(ret))
    return 1;

  /*Taking info passed in the arguments*/
  strcpy(name,argv[1]);
  dni=atoi(argv[2]);
  ccard=strtol(argv[3],NULL, 10);/*string to long int*/


    /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return 1;
  }

  return 0;

}
