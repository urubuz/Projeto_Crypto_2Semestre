
#include <stdio.h>
#include <windows.h>
#include "main.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    carregarMoedas(moedas);

    int decisao = 0;
    int autenticado = 0;

    do {
        printf("\n========================================\n");
        printf("Bem-vindo ao Sergio Autopecas!\n");
        printf("========================================\n");

        printf("\n1 - Fazer login\n");
        printf("2 - Criar conta\n");
        printf("3 - Sair\n");

        printf("\nEscolha uma opcao: ");
        scanf("%d", &decisao);

        switch (decisao) {
            case 1:
                autenticado = login();
                break;

            case 2:
                cadastrar();
                break;

            case 3:
                printf("\nEncerrando o programa...\n");
                break;

            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                Sleep(1500);
                break;
        }

    } while (decisao != 3 && autenticado == 0);

    if (autenticado == 1) {
        decisao = 0;

        do {
            printf("\n==========================================================================\n");
            printf("Bem-vindo ao Sergio autopecas, trocas de criptomoedas. O que deseja fazer?\n");
            printf("==========================================================================\n");

            printf("\n1 - Consultar saldo\n");
            printf("2 - Consultar cotação de moedas\n");
            printf("3 - Comprar criptomoedas\n");
            printf("4 - Vender criptomoedas\n");
            printf("5 - Depositar\n");
            printf("6 - Sacar\n");
            printf("7 - Consultar extrato\n");
            printf("8 - Atualizar cotacao\n");
            printf("9 - Sair\n");

            printf("\nEscolha uma opcao: ");
            scanf("%d", &decisao);

            if (decisao >= 1 && decisao <= 9) {
                    if (decisao == 1)
                        printf("Teste1");
                    if (decisao == 2)
                        consultar(moedas);
                    if (decisao == 3)
                        printf("Teste3");
                    if (decisao == 4)
                        printf("Teste4");
                    if (decisao == 5)
                        printf("Teste5");
                    if (decisao == 6)
                        printf("Teste6");
                    if (decisao == 7)
                        printf("Teste7");
                    if (decisao == 8)
                        atualizar();
                    if (decisao == 9)
                        exit(0);
            } else {
                printf("\nCodigo invalido, tente outro numero.\n");
                Sleep(1500);
            }

        } while (decisao != 9);
    }

    return 0;
}