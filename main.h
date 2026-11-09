#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
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

/* ================= EXTRATO ================= */

/* Monta o nome do arquivo de extrato a partir do CPF (somente os numeros): extrato_12345678903.txt */
void nomeArquivoExtrato(const char cpf[], char destino[], size_t tamanho) {
    char digitos[20];
    int j = 0;

    for (int i = 0; cpf[i] != '\0' && j < 19; i++) {
        if (isdigit((unsigned char)cpf[i])) {
            digitos[j++] = cpf[i];
        }
    }
    digitos[j] = '\0';

    snprintf(destino, tamanho, "extrato_%s.txt", digitos);
}

/* Escreve uma linha no formato: [dd/mm/aaaa hh:mm:ss] mensagem */
void escreverLinhaExtrato(FILE *arquivo, const char formato[], va_list args) {
    char data[32];
    time_t agora = time(NULL);
    struct tm *info = localtime(&agora);

    if (info != NULL) {
        strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", info);
    } else {
        strcpy(data, "data indisponivel");
    }

    fprintf(arquivo, "[%s] ", data);
    vfprintf(arquivo, formato, args);
    fprintf(arquivo, "\n");
}

/* Cria o extrato do usuario (com nome e CPF no cabecalho) caso ainda nao exista.
   Se "formato" nao for NULL, grava tambem uma primeira linha com essa mensagem. */
void garantirExtrato(const char nome[], const char cpf[], const char formato[], ...) {
    char nomeArquivo[50];
    nomeArquivoExtrato(cpf, nomeArquivo, sizeof(nomeArquivo));

    FILE *teste = fopen(nomeArquivo, "r");
    if (teste != NULL) {
        fclose(teste);
        return;
    }

    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo de extrato.\n");
        Sleep(1500);
        return;
    }

    fprintf(arquivo, "========================================\n");
    fprintf(arquivo, "EXTRATO - Sergio Autopecas\n");
    fprintf(arquivo, "Nome: %s\n", nome);
    fprintf(arquivo, "CPF: %s\n", cpf);
    fprintf(arquivo, "========================================\n");

    if (formato != NULL) {
        va_list args;
        va_start(args, formato);
        escreverLinhaExtrato(arquivo, formato, args);
        va_end(args);
    }

    fclose(arquivo);
}

/* Registra uma transacao no extrato do usuario logado */
void registrarExtrato(const char formato[], ...) {
    char nomeArquivo[50];

    garantirExtrato(usuariologado.nome, usuariologado.cpf, NULL);
    nomeArquivoExtrato(usuariologado.cpf, nomeArquivo, sizeof(nomeArquivo));

    FILE *arquivo = fopen(nomeArquivo, "a");
    if (arquivo == NULL) {
        printf("Erro ao registrar a transacao no extrato.\n");
        Sleep(1500);
        return;
    }

    va_list args;
    va_start(args, formato);
    escreverLinhaExtrato(arquivo, formato, args);
    va_end(args);

    fclose(arquivo);
}

/* Cria o extrato de todos os usuarios que ja estao no usuarios.txt e ainda nao tem um */
void criarExtratosExistentes() {
    FILE *arquivo = fopen("usuarios.txt", "r");

    if (arquivo == NULL) {
        return;
    }

    char linha[300];
    char nome[100], cpf[15], senha[20];
    float saldo, btc, eth, rip;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "%99[^;];%14[^;];%19[^;];%f;%f;%f;%f",
                   nome, cpf, senha, &saldo, &btc, &eth, &rip) == 7) {

            garantirExtrato(nome, cpf,
                            "Extrato iniciado | Saldo atual: R$ %.2f | %.6f BTC | %.6f ETH | %.6f XRP",
                            saldo, btc, eth, rip);
        }
    }

    fclose(arquivo);
}

void consultarExtrato() {
    char nomeArquivo[50];
    char linha[300];

    garantirExtrato(usuariologado.nome, usuariologado.cpf, NULL);
    nomeArquivoExtrato(usuariologado.cpf, nomeArquivo, sizeof(nomeArquivo));

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o extrato.\n");
        Sleep(1500);
        return;
    }

    printf("\n");
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        fputs(linha, stdout);
    }
    fclose(arquivo);

    printf("\nPressione Enter para voltar ao menu...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  /* limpa o que sobrou do menu */
    getchar();                                    /* espera o Enter */
}

/* ================= FIM EXTRATO ================= */

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
        if (sscanf(linha, "%99[^;];%14[^;];%19[^;];%f;%f;%f;%f",nomeSalvo, cpfSalvo, senhaSalva, &z1, &z2, &z3, &z4) == 7) {

            if (strcmp(cpfFormatado, cpfSalvo) == 0) {
                printf("\nJa existe uma conta cadastrada com esse CPF.\n");
                Sleep(1500);
                fclose(arquivo);
                return 0;
            }
        }
    }

    fseek(arquivo, 0, SEEK_END);
    fprintf(arquivo, "%s;%s;%s;0.00;0.00;0.00;0.00\n", nome, cpfFormatado, senha);

    fclose(arquivo);

    garantirExtrato(nome, cpfFormatado, "Conta criada | Saldo inicial: R$ 0.00");

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

                fprintf(temporario, "%s;%s;%s;%.2f;%.6f;%.6f;%.6f\n",
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
    printf("Digite um valor: ");
    scanf("%f", &deposito);

    if (deposito <= 0) {
        printf("Valor de deposito invalido.\n");
        Sleep(1500);
        return;
    }

    usuariologado.saldo += deposito;

    atualizarUsuario(usuariologado);

    registrarExtrato("DEPOSITO | + R$ %.2f | Saldo: R$ %.2f", deposito, usuariologado.saldo);

    printf("%.2f R$ depositados com sucesso!", deposito);
    Sleep(1500);
}

void sacar(){
    float saque;

    printf("Quanto você deseja sacar em R$?\n");
    printf("Digite um valor: ");
    scanf("%f", &saque);

    if (saque <= 0 || saque > usuariologado.saldo){
        printf("Valor de saque invalido.\n");
        Sleep(1500);
        return;
    }

    usuariologado.saldo -= saque;

    atualizarUsuario(usuariologado);

    registrarExtrato("SAQUE | - R$ %.2f | Saldo: R$ %.2f", saque, usuariologado.saldo);

    printf("%.2f R$ sacados com sucesso!", saque);
    Sleep(1500);
}

void comprarbtc(){
    float reais;

    printf("Quanto deseja comprar de BTC em R$?\n");
    printf("Digite um valor: ");
    scanf("%f", &reais);

    if (reais <=0 || reais > usuariologado.saldo){
        printf("Valor de compra invalido.\n");
        Sleep(1500);
        return;
    }
    float qtdadquirida = reais/moedas[0].valor;
    float qtdliquida = qtdadquirida*0.98;
    usuariologado.btc += qtdliquida;
    usuariologado.saldo -= reais;

    atualizarUsuario(usuariologado);

    registrarExtrato("COMPRA BTC | - R$ %.2f | Cotacao: R$ %.2f | Recebido (liquido de taxa): %.6f BTC | Saldo: R$ %.2f",
                     reais, moedas[0].valor, qtdliquida, usuariologado.saldo);

    printf("Você adquiriu  BTC %f!", qtdadquirida);
    Sleep(1500);
}

void comprareth(){
    float reais;

    printf("Quanto deseja comprar de ETH em R$?\n");
    printf("Digite um valor: ");
    scanf("%f", &reais);

    if (reais <=0 || reais > usuariologado.saldo){
        printf("Valor de compra invalido.\n");
        Sleep(1500);
        return;
    }
    float qtdadquirida = reais/moedas[1].valor;
    float qtdliquida = qtdadquirida*0.99;
    usuariologado.eth += qtdliquida;
    usuariologado.saldo -= reais;

    atualizarUsuario(usuariologado);

    registrarExtrato("COMPRA ETH | - R$ %.2f | Cotacao: R$ %.2f | Recebido (liquido de taxa): %.6f ETH | Saldo: R$ %.2f",
                     reais, moedas[1].valor, qtdliquida, usuariologado.saldo);

    printf("Você adquiriu ETH %f!", qtdadquirida);
    Sleep(1500);
}

void comprarrip(){
    float reais;

    printf("Quanto deseja comprar de XRP em R$?\n");
    printf("Digite um valor: ");
    scanf("%f", &reais);

    if (reais <=0 || reais > usuariologado.saldo){
        printf("Valor de compra invalido.\n");
        Sleep(1500);
        return;
    }
    float qtdadquirida = reais/moedas[2].valor;
    float qtdliquida = qtdadquirida*0.99;
    usuariologado.rip += qtdliquida;
    usuariologado.saldo -= reais;

    atualizarUsuario(usuariologado);

    registrarExtrato("COMPRA XRP | - R$ %.2f | Cotacao: R$ %.2f | Recebido (liquido de taxa): %.6f XRP | Saldo: R$ %.2f",
                     reais, moedas[2].valor, qtdliquida, usuariologado.saldo);

    printf("Você adquiriu XRP %f!", qtdadquirida);
    Sleep(1500);
}

void comprarcrypto(){
    int decisao;

        printf("Qual moeda deseja comprar?\n");
        printf("1 - Bitcoin\n");
        printf("2 - Ethereum\n");
        printf("3 - Ripple\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &decisao);

        switch (decisao) {
                case 1: comprarbtc(); break;
                case 2: comprareth(); break;
                case 3: comprarrip(); break;
                case 4: exit(0); break;
                default: printf("\nCodigo invalido, tente outro numero.\n"); Sleep(1500);
}
}

void venderbtc(){
    float qtdresgate;

    printf("Quanto deseja vender de BTC? (Você têm BTC %f)\n", usuariologado.btc);
    printf("Digite um valor: ");
    scanf("%f", &qtdresgate);

    if (qtdresgate <=0 || qtdresgate > usuariologado.btc){
        printf("Valor de vênda invalido.\n");
        Sleep(1500);
        return;
    }
    float reaisganhos = qtdresgate*moedas[0].valor;
    usuariologado.btc -= qtdresgate;
    float reaisliquidos = reaisganhos*0.97;
    usuariologado.saldo += reaisliquidos;

    atualizarUsuario(usuariologado);

    registrarExtrato("VENDA BTC | - %.6f BTC | Cotacao: R$ %.2f | + R$ %.2f (liquido de taxa) | Saldo: R$ %.2f",
                     qtdresgate, moedas[0].valor, reaisliquidos, usuariologado.saldo);

    printf("Você adquiriu R$ %.2f!", reaisganhos);
    Sleep(1500);
}

void vendereth(){
    float qtdresgate;

    printf("Quanto deseja vender de ETH? (Você têm ETH %f)\n", usuariologado.eth);
    printf("Digite um valor: ");
    scanf("%f", &qtdresgate);

    if (qtdresgate <=0 || qtdresgate > usuariologado.eth){
        printf("Valor de vênda invalido.\n");
        Sleep(1500);
        return;
    }
    float reaisganhos = qtdresgate*moedas[1].valor;
    usuariologado.eth -= qtdresgate;
    float reaisliquidos = reaisganhos*0.98;
    usuariologado.saldo += reaisliquidos;

    atualizarUsuario(usuariologado);

    registrarExtrato("VENDA ETH | - %.6f ETH | Cotacao: R$ %.2f | + R$ %.2f (liquido de taxa) | Saldo: R$ %.2f",
                     qtdresgate, moedas[1].valor, reaisliquidos, usuariologado.saldo);

    printf("Você adquiriu R$ %.2f!", reaisganhos);
    Sleep(1500);
}

void venderrip(){
    float qtdresgate;

    printf("Quanto deseja vender de XRP? (Você têm XRP %f)\n", usuariologado.rip);
    printf("Digite um valor: ");
    scanf("%f", &qtdresgate);

    if (qtdresgate <=0 || qtdresgate > usuariologado.rip){
        printf("Valor de vênda invalido.\n");
        Sleep(1500);
        return;
    }
    float reaisganhos = qtdresgate*moedas[2].valor;
    usuariologado.rip -= qtdresgate;
    float reaisliquidos = reaisganhos*0.99;
    usuariologado.saldo += reaisliquidos;

    atualizarUsuario(usuariologado);

    registrarExtrato("VENDA XRP | - %.6f XRP | Cotacao: R$ %.2f | + R$ %.2f (liquido de taxa) | Saldo: R$ %.2f",
                     qtdresgate, moedas[2].valor, reaisliquidos, usuariologado.saldo);

    printf("Você adquiriu R$ %.2f!", reaisganhos);
    Sleep(1500);
}

void vendercrypto(){
    int decisao;

        printf("Qual moeda deseja vender?\n");
        printf("1 - Bitcoin\n");
        printf("2 - Ethereum\n");
        printf("3 - Ripple\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &decisao);

        switch (decisao) {
                case 1: venderbtc(); break;
                case 2: vendereth(); break;
                case 3: venderrip(); break;
                case 4: exit(0); break;
                default: printf("\nCodigo invalido, tente outro numero.\n"); Sleep(1500);
}
}