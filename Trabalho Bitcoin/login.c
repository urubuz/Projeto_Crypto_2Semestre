#include <stdio.h>
#include <stdlib.h>
#include "saldo.h"

int login(int login_decisao){
    if (login_decisao == 1)
        teste();
    if (login_decisao == 2)
        printf("Teste2");
    if (login_decisao == 3)
        printf("Teste3");
    if (login_decisao == 4)
        printf("Teste4");
    if (login_decisao == 5)
        printf("Teste5");
    if (login_decisao == 6)
        printf("Teste6");
    if (login_decisao == 7)
        printf("Teste7");
    if (login_decisao == 8)
        exit(0);
}