#include <stdio.h>
#include <stdlib.h>
#include "saldo.h"

int login(int login_decisao){
    if (login_decisao == 1)
        teste(); //criar função para consultar saldo
    if (login_decisao == 2)
        printf("Teste2"); // criar função para consultar extrato
    if (login_decisao == 3)
        printf("Teste3"); // criar função para depositar dinheiros
    if (login_decisao == 4)
        printf("Teste4"); // criar função para sacar dinheiros
    if (login_decisao == 5)
        printf("Teste5"); // criar função para comprar cripto
    if (login_decisao == 6)
        printf("Teste6"); // criar função para vender cripto
    if (login_decisao == 7)
        printf("Teste7"); // criar função de atualizar
    if (login_decisao == 8)
        exit(0); // só vaza do programa
}
