#include <stdio.h>
#include <windows.h>
#include "main.h"

int main() {
    char caminhoExe[MAX_PATH];
    GetModuleFileNameA(NULL, caminhoExe, MAX_PATH);
    char *ultimaBarra = strrchr(caminhoExe, '\\');
    if (ultimaBarra != NULL) {
        *ultimaBarra = '\0';
        SetCurrentDirectoryA(caminhoExe);
    }
    
    SetConsoleOutputCP(CP_UTF8);
    carregarMoedas(moedas);
    criarExtratosExistentes();

    char pasta[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, pasta);
    printf("Pasta em uso: %s\n", pasta);

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

    if (autenticado == 1 && usuariologado.tipo == 0) {
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

            switch (decisao) {
                case 1: consultarSaldo(); break;
                case 2: consultar(moedas); break;
                case 3: comprarcrypto(); break;
                case 4: vendercrypto(); break;
                case 5: depositar(); break;
                case 6: sacar(); break;
                case 7: consultarExtrato(); break;
                case 8: atualizar(); break;
                case 9: exit(0);
                default: printf("\nCodigo invalido, tente outro numero.\n"); Sleep(1500);
}

        } while (decisao != 9);
    }

    if (autenticado == 1 && usuariologado.tipo == 1) {
        decisao = 0;

        do {
            printf("\n==========================================================================\n");
            printf("Bem-vindo ao Sergio autopecas, administrador. O que deseja fazer?\n");
            printf("==========================================================================\n");
            printf("\n1 - Cadastrar novo investidor\n");
            printf("2 - Excluir investidor\n");
            printf("3 - Cadastrar nova criptomoeda\n");
            printf("4 - Excluir criptomoeda\n");
            printf("5 - Consultar saldo de um investidor\n");
            printf("6 - Consultar extrato de um investidor\n");
            printf("7 - Atualizar cotacao\n");
            printf("8 - Sair\n");
            printf("\nEscolha uma opcao: ");
            scanf("%d", &decisao);

            switch (decisao) {
                case 1: cadastrar(); break;
                case 2: excluirInvestidor(); break;
                case 3: cadastrarCripto(); break;
                case 4: excluirCripto(); break;
                case 5: consultarSaldoInvestidor(); break;
                case 6: consultarExtratoInvestidor(); break;
                case 7: atualizar(); break;
                case 8: exit(0);
                default: printf("\nCodigo invalido, tente outro numero.\n"); Sleep(1500);
}

        } while (decisao != 8);
    }

    return 0;
}