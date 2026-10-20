#include <stdio.h>
#include <windows.h>
#include "login.h"

int main(){
    SetConsoleOutputCP(CP_UTF8);
    int decisao = 0; 
    
    do {
        printf("==========================================================================\n");
        printf("Bem vindo ao Sergio autopeças, trocas de criptomoedas. O que deseja fazer?\n");
        printf("==========================================================================\n");
        printf("\n");
        printf("1 - Consultar Saldo\n");
        printf("2 - Consultar Extrato\n");
        printf("3 - Depositar\n");
        printf("4 - Sacar\n");
        printf("5 - Comprar criptomoedas\n");
        printf("6 - Vender criptomoedas\n");
        printf("7 - Atualizar cotação\n");
        printf("8 - Sair\n");
        printf("\n");
        
        scanf("%d", &decisao);
        
        if (decisao > 8 || decisao < 1){
            printf("\nCodigo invalido, tente outro número.\n\n");
            Sleep(2500);
        }

    } while (decisao > 8 || decisao < 1);
    
    login(decisao);
}
