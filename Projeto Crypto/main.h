#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

float z1 = 0.0, z2 = 0.0, z3 = 0.0, z4 = 0.0;

struct Moeda {
    char nome[20];
    float valor;
    float taxaCompra;
    float taxaVenda;
    float variacao;
};

struct Moeda moedas[3] = {
    {"Bitcoin", 1000, 0.02, 0.03, 0.0},
    {"Ethereum", 500, 0.01, 0.02, 0.0},
    {"Ripple", 250, 0.01, 0.01, 0.0}
};

struct User {
    char nome[100];
    char cpf[15];
    char senha[20];
    float saldo;
    float btc;
    float eth;
    float rip;
};

struct User usuariologado;

void salvarMoedas(struct Moeda moedas[]) {
    FILE *arquivo = fopen("moedas.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao salvar as moedas.\n");
        Sleep(1500);
        return;
    }

    for (int i = 0; i < 3; i++) {
        fprintf(arquivo, "%s;%.6f;%.6f;%.6f;%.6f\n",
                moedas[i].nome,
                moedas[i].valor,
                moedas[i].taxaCompra,
                moedas[i].taxaVenda,
                moedas[i].variacao);
    }

    fclose(arquivo);
}

int validarNumeros(char texto[]) {
    for (int i = 0; texto[i] != '\0'; i++) {
        if (!isdigit((unsigned char)texto[i])) {
            return 0;
        }
    }

    return 1;
}

void formatarCPF(char cpf[], char cpfFormatado[]) {
    sprintf(cpfFormatado, "%.3s.%.3s.%.3s-%.2s",
            cpf, cpf + 3, cpf + 6, cpf + 9);
}

int cadastrar() {
    FILE *arquivo;
    char nome[100];
    char cpf[20];
    char cpfFormatado[15];
    char senha[20];
    char linha[200];

    printf("\n========== CADASTRO ==========\n");

    printf("Digite seu nome: ");
    scanf(" %99[^\n]", nome);

    printf("Digite seu CPF (somente numeros): ");
    scanf("%19s", cpf);

    if (strlen(cpf) != 11 || !validarNumeros(cpf)) {
        printf("\nCPF invalido. Ele deve conter exatamente 11 numeros.\n");
        Sleep(1500);
        return 0;
    }

    formatarCPF(cpf, cpfFormatado);

    printf("Digite sua senha (6 numeros): ");
    scanf("%19s", senha);

    if (strlen(senha) != 6 || !validarNumeros(senha)) {
        printf("\nSenha invalida. Ela deve conter exatamente 6 numeros.\n");
        Sleep(1500);
        return 0;
    }

    arquivo = fopen("usuarios.txt", "a+");

    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo de usuarios.\n");
        Sleep(1500);
        return 0;
    }

    rewind(arquivo);

    char nomeSalvo[100];
    char cpfSalvo[15];
    char senhaSalva[20];


    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "%99[^;];%14[^;];%19[^;];%.2f;%.2f;%.2f;%.2f\n",nomeSalvo, cpfSalvo, senhaSalva, &z1, &z2, &z3, &z4) == 7) {

            if (strcmp(cpfFormatado, cpfSalvo) == 0) {
                printf("\nJa existe uma conta cadastrada com esse CPF.\n");
                Sleep(1500);
                fclose(arquivo);
                return 0;
            }
        }
    }

    fprintf(arquivo, "%s;%s;%s;%.2f;%.2f;%.2f;%.2f\n", nome, cpfFormatado, senha, z1, z2, z3, z4);

    fclose(arquivo);

    printf("\nCadastro realizado com sucesso!\n");
    Sleep(1500);

    return 1;
}

int login() {
    FILE *arquivo;
    char cpf[20];
    char cpfFormatado[15];
    char senha[20];
    char linha[200];

    char nomeSalvo[100];
    char cpfSalvo[15];
    char senhaSalva[20];

    printf("\n========== LOGIN ==========\n");

    printf("Digite seu CPF (somente numeros): ");
    scanf("%19s", cpf);

    if (strlen(cpf) != 11 || !validarNumeros(cpf)) {
        printf("\nCPF invalido.\n");
        Sleep(1500);
        return 0;
    }

    formatarCPF(cpf, cpfFormatado);

    printf("Digite sua senha: ");
    scanf("%19s", senha);

    if (strlen(senha) != 6 || !validarNumeros(senha)) {
        printf("\nSenha invalida.\n");
        Sleep(1500);
        return 0;
    }

    arquivo = fopen("usuarios.txt", "r");

    if (arquivo == NULL) {
        printf("\nNenhum usuario cadastrado.\n");
        Sleep(1500);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "%99[^;];%14[^;];%19[^;];%f;%f;%f;%f",nomeSalvo, cpfSalvo, senhaSalva, &z1, &z2, &z3, &z4) == 7) {

            if (strcmp(cpfFormatado, cpfSalvo) == 0 && strcmp(senha, senhaSalva) == 0) {

                printf("\nLogin realizado com sucesso! Bem-vindo, %s.\n", nomeSalvo);
                strcpy(usuariologado.nome, nomeSalvo);
                strcpy(usuariologado.cpf, cpfSalvo);
                strcpy(usuariologado.senha, senhaSalva);
                usuariologado.saldo = z1;
                usuariologado.btc = z2;
                usuariologado.eth = z3;
                usuariologado.rip = z4;
                Sleep(1500);

                fclose(arquivo);
                return 1;
            }
        }
    }

    fclose(arquivo);

    printf("\nCPF ou senha incorretos.\n");
    Sleep(1500);

    return 0;
}

void carregarMoedas(struct Moeda moedas[]) {
    FILE *arquivo = fopen("moedas.txt", "r");

    if (arquivo == NULL) {
        salvarMoedas(moedas);
        return;
    }

    char linha[200];
    char nome[20];
    float valor, taxaCompra, taxaVenda, variacao;
    int encontrou = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        encontrou = 1;

        if (sscanf(linha, "%19[^;];%f;%f;%f;%f",
                   nome, &valor, &taxaCompra, &taxaVenda, &variacao) == 5) {

            for (int i = 0; i < 3; i++) {
                if (strcmp(nome, moedas[i].nome) == 0) {
                    moedas[i].valor = valor;
                    moedas[i].taxaCompra = taxaCompra;
                    moedas[i].taxaVenda = taxaVenda;
                    moedas[i].variacao = variacao;
                }
            }
        }
    }

    fclose(arquivo);

    if (!encontrou) {
        salvarMoedas(moedas);
    }
}

void atualizarUsuario(struct User usuario) {
    FILE *arquivo = fopen("usuarios.txt", "r");
    FILE *temporario = fopen("usuarios_temp.txt", "w");

    if (arquivo == NULL || temporario == NULL) {
        printf("Erro ao atualizar usuario.\n");

        if (arquivo != NULL) fclose(arquivo);
        if (temporario != NULL) fclose(temporario);

        return;
    }

    char linha[300];
    char nome[100], cpf[15], senha[20];
    float saldo, btc, eth, rip;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (sscanf(linha, "%99[^;];%14[^;];%19[^;];%f;%f;%f;%f",
                   nome, cpf, senha, &saldo, &btc, &eth, &rip) == 7) {

            if (strcmp(cpf, usuario.cpf) == 0) {

                fprintf(temporario, "%s;%s;%s;%.2f;%.2f;%.2f;%.2f\n",
                        usuario.nome,
                        usuario.cpf,
                        usuario.senha,
                        usuario.saldo,
                        usuario.btc,
                        usuario.eth,
                        usuario.rip);

            } else {
                fputs(linha, temporario);
            }

        } else {
            fputs(linha, temporario);
        }
    }

    fclose(arquivo);
    fclose(temporario);

    remove("usuarios.txt");
    rename("usuarios_temp.txt", "usuarios.txt");
}

void consultar(struct Moeda moedas[]){
    for (int i=0;i<3;i++){
        printf("%s, com valor de: %.2f (Variação de: %.2f)\n", moedas[i].nome, moedas[i].valor, moedas[i].variacao);
    }
}

void atualizar(){
    for (int i=0;i<3;i++){
         moedas[i].variacao = ((float)rand() / RAND_MAX) * 5.0;

        int direcao = rand() % 2;

        if (direcao == 0) {
            moedas[i].variacao = -moedas[i].variacao;
        }

        moedas[i].valor *= (1 + moedas[i].variacao / 100);
    }
    salvarMoedas(moedas);
    printf("Valores atualizados.");
}

void consultarSaldo(){
    printf("\n========== SALDO ==========\n");
    printf("\nNome: %s\n", usuariologado.nome);
    printf("CPF: %s\n", usuariologado.cpf);
    printf("\nReais: R$ %.2f\n", usuariologado.saldo);
    printf("Bitcoin: %.6f BTC\n", usuariologado.btc);
    printf("Ethereum: %.6f ETH\n", usuariologado.eth);
    printf("Ripple: %.6f XRP\n", usuariologado.rip);
    Sleep(1500);
}

void depositar(){
    float deposito;

    printf("Quanto você deseja depositar em R$?\n");
    scanf("%f", &deposito);

    if (deposito <= 0) {
        printf("Valor de deposito invalido.\n");
        Sleep(1500);
        return;
    }

    usuariologado.saldo += deposito;

    atualizarUsuario(usuariologado);

    printf("%.2f R$ depositados com sucesso!", deposito);
    Sleep(1500);
}

void sacar(){
    float saque;

    printf("Quanto você deseja sacar em R$?\n");
    scanf("%f", &saque);

    if (saque <= 0 || saque > usuariologado.saldo){
        printf("Valor de saque invalido.\n");
        Sleep(1500);
        return;
    }

    usuariologado.saldo -= saque;

    atualizarUsuario(usuariologado);

    printf("%.2f R$ sacados com sucesso!", saque);
    Sleep(1500);
}

void comprarbtc(){
    
}