#!/bin/bash

COMMAND="./esecuele"

#rm -rf bank_db

#$COMMAND createdb bank_db

#$COMMAND define bank_db << EOF
#TABLE clients 2 INT STR
#TABLE accounts 3 INT STR INT
#TABLE accounts_clients 2 INT INT
#EOF

#$COMMAND insert bank_db << EOF
#COPY clients ./example_files/clients.txt
#COPY accounts ./example_files/accounts.txt
#COPY accounts_clients ./example_files/accounts_clients.txt
#EOF

## Test the following queries:
## - Which are the accounts of the Brooklyn branch? Show account_id and amount
#$COMMAND query bank_db << EOF
#accounts SEQUENTIAL 1 STR Brooklyn C_COLEQCTE SELECT INT 0 P_COL INT 2 P_COL 2 PROJECT
#EOF

## - Who owns an account in the Queens branch? Show account number, amount and the name of the owner
$COMMAND query bank_db << EOF
accounts SEQUENTIAL 1 STR Queens C_COLEQCTE SELECT INT 0 P_COL INT 2 P_COL 2 PROJECT accounts_clients SEQUENTIAL P#RODUCT 0 2 C_COLEQCOL SELECT INT 0 P_COL INT 1 P_COL INT 3 P_COL 3 PROJECT clients SEQUENTIAL PRODUCT 2 3 C_COLEQC#OL SELECT INT 0 P_COL INT 1 P_COL STR 4 P_COL 3 PROJECT
EOF

## Probando la operacion COUNT
## - Count the number of clients
#$COMMAND query bank_db << EOF
#clients SEQUENTIAL COUNT
#EOF

## Probando la operacion UNION
## - Do the union of the same table accounts
#$COMMAND query bank_db << EOF
#accounts SEQUENTIAL accounts SEQUENTIAL UNION
#EOF

## Probando la operacion LIMIT
## -Show the first five rows of table accounts
#$COMMAND query bank_db << EOF
#accounts SEQUENTIAL 5 LIMIT
#EOF

## Probando la operacion OFFSET
## -Show the last five rows of table accounts
#$COMMAND query bank_db << EOF
#accounts SEQUENTIAl 5 OFFSET
#EOF

## Probando todas las operaciones
## -Count the number of first five accounts union five last accounts
#$COMMAND query bank_db << EOF
#accounts SEQUENTIAL 5 OFFSET accounts SEQUENTIAL 5 LIMIT UNION COUNT
#EOF