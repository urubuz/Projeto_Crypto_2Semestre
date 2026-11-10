#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <windows.h>

#define MAX_MOEDAS 20

struct Moeda {
    char nome[20];
    float valor;
    float taxaCompra;
    float taxaVenda;
    float variacao;
};

struct Moeda moedas[MAX_MOEDAS] = {
    {"Bitcoin", 1000, 0.02, 0.03, 0.0},
    {"Ethereum", 500, 0.01, 0.02, 0.0},
    {"Ripple", 250, 0.01, 0.01, 0.0}
};

int totalMoedas = 3;

struct User {
    char nome[100];
    char cpf[15];
    char senha[20];
    int tipo;
    float saldo;
    float cripto[MAX_MOEDAS];
};

struct User usuariologado;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int lerUsuario(char linha[], struct User *usuario) {
    char copia[1000];
    char *campos[40];
    int total = 0;

    strncpy(copia, linha, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';
    copia[strcspn(copia, "\r\n")] = '\0';

    char *atual = copia;

    while (total < 40) {
        campos[total] = atual;
        total++;

        char *separador = strchr(atual, ';');

        if (separador == NULL) {
            break;
        }

        *separador = '\0';
        atual = separador + 1;
    }

    if (total < 5) {
        return 0;
    }

    if (strlen(campos[0]) > 99 || strlen(campos[1]) > 14 || strlen(campos[2]) > 19) {
        return 0;
    }


    strcpy(usuario->nome, campos[0]);
    strcpy(usuario->cpf, campos[1]);
    strcpy(usuario->senha, campos[2]);
    usuario->tipo = atoi(campos[3]);
    usuario->saldo = atof(campos[4]);

    for (int i = 0; i < MAX_MOEDAS; i++) {
        if (5 + i < total) {
            usuario->cripto[i] = atof(campos[5 + i]);
        } else {
            usuario->cripto[i] = 0.0;
        }
    }

    return 1;
}

void escreverUsuario(FILE *arquivo, struct User usuario) {
    fprintf(arquivo, "%s;%s;%s;%d;%.2f",
            usuario.nome,
            usuario.cpf,
            usuario.senha,
            usuario.tipo,
            usuario.saldo);

    for (int i = 0; i < totalMoedas; i++) {
        fprintf(arquivo, ";%.6f", usuario.cripto[i]);
    }

    fprintf(arquivo, "\n");
}

int buscarUsuario(char cpfFormatado[], struct User *encontrado) {
    FILE *arquivo = fopen("usuarios.txt", "r");
    char linha[1000];
    struct User lido;

    if (arquivo == NULL) {
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (lerUsuario(linha, &lido) && strcmp(lido.cpf, cpfFormatado) == 0) {
            *encontrado = lido;
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

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

void criarExtratosExistentes() {
    FILE *arquivo = fopen("usuarios.txt", "r");

    if (arquivo == NULL) {
        return;
    }

    char linha[1000];
    char texto[1000];
    struct User lido;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (lerUsuario(linha, &lido) && lido.tipo == 0) {

            sprintf(texto, "Extrato iniciado | Saldo atual: R$ %.2f", lido.saldo);

            for (int i = 0; i < totalMoedas; i++) {
                sprintf(texto + strlen(texto), " | %.6f %s", lido.cripto[i], moedas[i].nome);
            }

            garantirExtrato(lido.nome, lido.cpf, "%s", texto);
        }
    }

    fclose(arquivo);
}

void mostrarExtrato(struct User usuario) {
    char nomeArquivo[50];
    char linha[300];

    garantirExtrato(usuario.nome, usuario.cpf, NULL);
    nomeArquivoExtrato(usuario.cpf, nomeArquivo, sizeof(nomeArquivo));

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
    limparBuffer();
    getchar();
}

void salvarMoedas(struct Moeda moedas[]) {
    FILE *arquivo = fopen("moedas.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao salvar as moedas.\n");
        Sleep(1500);
        return;
    }

    for (int i = 0; i < totalMoedas; i++) {
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
    char linha[1000];

    printf("\n========== CADASTRO ==========\n");

    printf("Digite seu nome: ");
    scanf(" %99[^\n]", nome);

    if (strchr(nome, ';') != NULL) {
        printf("\nNome invalido. Ele nao pode conter ponto e virgula.\n");
        Sleep(1500);
        return 0;
    }

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

    struct User salvo;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (lerUsuario(linha, &salvo)) {

            if (strcmp(cpfFormatado, salvo.cpf) == 0) {
                printf("\nJa existe uma conta cadastrada com esse CPF.\n");
                Sleep(1500);
                fclose(arquivo);
                return 0;
            }
        }
    }

    fseek(arquivo, 0, SEEK_END);
    fprintf(arquivo, "%s;%s;%s;0;0.00", nome, cpfFormatado, senha);

    for (int i = 0; i < totalMoedas; i++) {
        fprintf(arquivo, ";0.000000");
    }

    fprintf(arquivo, "\n");

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
    char linha[1000];

    struct User salvo;

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
        if (lerUsuario(linha, &salvo)) {

            if (strcmp(cpfFormatado, salvo.cpf) == 0 && strcmp(senha, salvo.senha) == 0) {

                printf("\nLogin realizado com sucesso! Bem-vindo, %s.\n", salvo.nome);
                usuariologado = salvo;
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
    struct Moeda lida;

    totalMoedas = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (sscanf(linha, "%19[^;];%f;%f;%f;%f",
                   lida.nome, &lida.valor, &lida.taxaCompra, &lida.taxaVenda, &lida.variacao) == 5) {

            if (totalMoedas < MAX_MOEDAS) {
                moedas[totalMoedas] = lida;
                totalMoedas++;
            }
        }
    }

    fclose(arquivo);
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

    char linha[1000];
    struct User lido;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (lerUsuario(linha, &lido)) {

            if (strcmp(lido.cpf, usuario.cpf) == 0) {
                escreverUsuario(temporario, usuario);
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
    for (int i=0;i<totalMoedas;i++){
        printf("%s, com valor de: %.2f (Variação de: %.2f)\n", moedas[i].nome, moedas[i].valor, moedas[i].variacao);
    }
}

void atualizar(){
    for (int i=0;i<totalMoedas;i++){
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

int confirmarSenha(){
    char senha[20];

    printf("Digite sua senha: ");
    scanf("%19s", senha);

    if (strcmp(senha, usuariologado.senha) != 0) {
        printf("\nSenha incorreta.\n");
        Sleep(1500);
        return 0;
    }

    return 1;
}

void mostrarSaldo(struct User usuario){
    printf("\n========== SALDO ==========\n");
    printf("\nNome: %s\n", usuario.nome);
    printf("CPF: %s\n", usuario.cpf);
    printf("\nReais: R$ %.2f\n", usuario.saldo);
    for (int i=0;i<totalMoedas;i++){
        printf("%s: %.6f\n", moedas[i].nome, usuario.cripto[i]);
    }
    Sleep(1500);
}

void consultarSaldo(){
    if (!confirmarSenha()) {
        return;
    }

    mostrarSaldo(usuariologado);
}

void consultarExtrato(){
    if (!confirmarSenha()) {
        return;
    }

    mostrarExtrato(usuariologado);
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

void comprar(int indice){
    float reais;

    printf("Quanto deseja comprar de %s em R$?\n", moedas[indice].nome);
    printf("Digite um valor: ");
    scanf("%f", &reais);

    if (reais <=0 || reais > usuariologado.saldo){
        printf("Valor de compra invalido.\n");
        Sleep(1500);
        return;
    }
    float qtdadquirida = reais/moedas[indice].valor;
    float qtdliquida = qtdadquirida*(1 - moedas[indice].taxaCompra);
    usuariologado.cripto[indice] += qtdliquida;
    usuariologado.saldo -= reais;

    atualizarUsuario(usuariologado);

    registrarExtrato("COMPRA %s | - R$ %.2f | Cotacao: R$ %.2f | Recebido (liquido de taxa): %.6f %s | Saldo: R$ %.2f",
                     moedas[indice].nome, reais, moedas[indice].valor, qtdliquida, moedas[indice].nome, usuariologado.saldo);

    printf("Você adquiriu %s %f!", moedas[indice].nome, qtdadquirida);
    Sleep(1500);
}

void comprarcrypto(){
    int decisao = 0;

    if (!confirmarSenha()) {
        return;
    }

        printf("Qual moeda deseja comprar?\n");
        for (int i=0;i<totalMoedas;i++){
            printf("%d - %s\n", i+1, moedas[i].nome);
        }
        printf("\nEscolha uma opcao: ");
        scanf("%d", &decisao);

        if (decisao >= 1 && decisao <= totalMoedas) {
            comprar(decisao - 1);
        } else {
            printf("\nCodigo invalido, tente outro numero.\n");
            Sleep(1500);
        }
}

void vender(int indice){
    float qtdresgate;

    printf("Quanto deseja vender de %s? (Você têm %s %f)\n", moedas[indice].nome, moedas[indice].nome, usuariologado.cripto[indice]);
    printf("Digite um valor: ");
    scanf("%f", &qtdresgate);

    if (qtdresgate <=0 || qtdresgate > usuariologado.cripto[indice]){
        printf("Valor de vênda invalido.\n");
        Sleep(1500);
        return;
    }
    float reaisganhos = qtdresgate*moedas[indice].valor;
    float reaisliquidos = reaisganhos*(1 - moedas[indice].taxaVenda);
    usuariologado.cripto[indice] -= qtdresgate;
    usuariologado.saldo += reaisliquidos;

    atualizarUsuario(usuariologado);

    registrarExtrato("VENDA %s | - %.6f %s | Cotacao: R$ %.2f | + R$ %.2f (liquido de taxa) | Saldo: R$ %.2f",
                     moedas[indice].nome, qtdresgate, moedas[indice].nome, moedas[indice].valor, reaisliquidos, usuariologado.saldo);

    printf("Você adquiriu R$ %.2f!", reaisganhos);
    Sleep(1500);
}

void vendercrypto(){
    int decisao = 0;

    if (!confirmarSenha()) {
        return;
    }

        printf("Qual moeda deseja vender?\n");
        for (int i=0;i<totalMoedas;i++){
            printf("%d - %s\n", i+1, moedas[i].nome);
        }
        printf("\nEscolha uma opcao: ");
        scanf("%d", &decisao);

        if (decisao >= 1 && decisao <= totalMoedas) {
            vender(decisao - 1);
        } else {
            printf("\nCodigo invalido, tente outro numero.\n");
            Sleep(1500);
        }
}

int lerCPFInvestidor(char cpfFormatado[]) {
    char cpf[20];

    printf("Digite o CPF do investidor (somente numeros): ");
    scanf("%19s", cpf);

    if (strlen(cpf) != 11 || !validarNumeros(cpf)) {
        printf("\nCPF invalido.\n");
        Sleep(1500);
        return 0;
    }

    formatarCPF(cpf, cpfFormatado);

    return 1;
}

int nomesIguais(char a[], char b[]) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return 0;
        }
        i++;
    }

    return a[i] == b[i];
}

void excluirInvestidor(){
    char cpfFormatado[15];
    char confirmacao;
    char linha[1000];
    char nomeArquivo[50];
    struct User investidor;
    struct User lido;

    printf("\n========== EXCLUIR INVESTIDOR ==========\n");

    if (!lerCPFInvestidor(cpfFormatado)) {
        return;
    }

    if (!buscarUsuario(cpfFormatado, &investidor)) {
        printf("\nInvestidor nao encontrado.\n");
        Sleep(1500);
        return;
    }

    if (investidor.tipo == 1) {
        printf("\nNao e possivel excluir um administrador.\n");
        Sleep(1500);
        return;
    }

    mostrarSaldo(investidor);

    printf("\nDeseja realmente excluir este investidor? (s/n): ");
    scanf(" %c", &confirmacao);
    limparBuffer();

    if (tolower((unsigned char)confirmacao) != 's') {
        printf("\nExclusao cancelada.\n");
        Sleep(1500);
        return;
    }

    FILE *arquivo = fopen("usuarios.txt", "r");
    FILE *temporario = fopen("usuarios_temp.txt", "w");

    if (arquivo == NULL || temporario == NULL) {
        printf("Erro ao excluir investidor.\n");

        if (arquivo != NULL) fclose(arquivo);
        if (temporario != NULL) fclose(temporario);

        Sleep(1500);
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (lerUsuario(linha, &lido) && strcmp(lido.cpf, cpfFormatado) == 0) {
            continue;
        }

        fputs(linha, temporario);
    }

    fclose(arquivo);
    fclose(temporario);

    remove("usuarios.txt");
    rename("usuarios_temp.txt", "usuarios.txt");

    nomeArquivoExtrato(cpfFormatado, nomeArquivo, sizeof(nomeArquivo));
    remove(nomeArquivo);

    printf("\nInvestidor excluido com sucesso!\n");
    Sleep(1500);
}

void cadastrarCripto(){
    char nome[100];
    float valor, taxaCompra, taxaVenda;
    int leitura;

    if (totalMoedas >= MAX_MOEDAS) {
        printf("\nLimite de criptomoedas atingido.\n");
        Sleep(1500);
        return;
    }

    printf("\n========== NOVA CRIPTOMOEDA ==========\n");

    printf("Digite o nome da criptomoeda: ");
    scanf(" %99[^\n]", nome);
    limparBuffer();

    if (strlen(nome) > 19 || strchr(nome, ';') != NULL) {
        printf("\nNome invalido. Use ate 19 caracteres, sem ponto e virgula.\n");
        Sleep(1500);
        return;
    }

    for (int i = 0; i < totalMoedas; i++) {
        if (nomesIguais(nome, moedas[i].nome)) {
            printf("\nJa existe uma criptomoeda com esse nome.\n");
            Sleep(1500);
            return;
        }
    }

    printf("Digite a cotacao inicial em R$: ");
    leitura = scanf("%f", &valor);
    limparBuffer();

    if (leitura != 1 || valor <= 0) {
        printf("\nCotacao invalida.\n");
        Sleep(1500);
        return;
    }

    printf("Digite a taxa de compra em %%: ");
    leitura = scanf("%f", &taxaCompra);
    limparBuffer();

    if (leitura != 1 || taxaCompra < 0 || taxaCompra >= 100) {
        printf("\nTaxa de compra invalida.\n");
        Sleep(1500);
        return;
    }

    printf("Digite a taxa de venda em %%: ");
    leitura = scanf("%f", &taxaVenda);
    limparBuffer();

    if (leitura != 1 || taxaVenda < 0 || taxaVenda >= 100) {
        printf("\nTaxa de venda invalida.\n");
        Sleep(1500);
        return;
    }

    strcpy(moedas[totalMoedas].nome, nome);
    moedas[totalMoedas].valor = valor;
    moedas[totalMoedas].taxaCompra = taxaCompra / 100;
    moedas[totalMoedas].taxaVenda = taxaVenda / 100;
    moedas[totalMoedas].variacao = 0.0;
    totalMoedas++;

    salvarMoedas(moedas);

    printf("\nCriptomoeda cadastrada com sucesso!\n");
    Sleep(1500);
}

void excluirCripto(){
    int decisao = 0;
    int leitura;
    int indice;
    char confirmacao;
    char linha[1000];
    char nomeExcluido[20];
    struct User lido;

    if (totalMoedas == 0) {
        printf("\nNenhuma criptomoeda cadastrada.\n");
        Sleep(1500);
        return;
    }

    printf("\nQual moeda deseja excluir?\n");
    for (int i=0;i<totalMoedas;i++){
        printf("%d - %s\n", i+1, moedas[i].nome);
    }
    printf("\nEscolha uma opcao: ");
    leitura = scanf("%d", &decisao);
    limparBuffer();

    if (leitura != 1 || decisao < 1 || decisao > totalMoedas) {
        printf("\nCodigo invalido, tente outro numero.\n");
        Sleep(1500);
        return;
    }

    indice = decisao - 1;
    strcpy(nomeExcluido, moedas[indice].nome);

    FILE *arquivo = fopen("usuarios.txt", "r");

    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo de usuarios.\n");
        Sleep(1500);
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (lerUsuario(linha, &lido) && lido.cripto[indice] > 0) {
            printf("\nNao e possivel excluir %s, ainda existem investidores com saldo dessa moeda.\n", nomeExcluido);
            Sleep(1500);
            fclose(arquivo);
            return;
        }
    }

    fclose(arquivo);

    printf("\nDeseja realmente excluir %s? (s/n): ", nomeExcluido);
    scanf(" %c", &confirmacao);
    limparBuffer();

    if (tolower((unsigned char)confirmacao) != 's') {
        printf("\nExclusao cancelada.\n");
        Sleep(1500);
        return;
    }

    arquivo = fopen("usuarios.txt", "r");
    FILE *temporario = fopen("usuarios_temp.txt", "w");

    if (arquivo == NULL || temporario == NULL) {
        printf("Erro ao excluir criptomoeda.\n");

        if (arquivo != NULL) fclose(arquivo);
        if (temporario != NULL) fclose(temporario);

        Sleep(1500);
        return;
    }

    for (int i = indice; i < totalMoedas - 1; i++) {
        moedas[i] = moedas[i + 1];
    }
    totalMoedas--;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        if (lerUsuario(linha, &lido)) {

            for (int i = indice; i < MAX_MOEDAS - 1; i++) {
                lido.cripto[i] = lido.cripto[i + 1];
            }
            lido.cripto[MAX_MOEDAS - 1] = 0.0;

            escreverUsuario(temporario, lido);

        } else {
            fputs(linha, temporario);
        }
    }

    fclose(arquivo);
    fclose(temporario);

    remove("usuarios.txt");
    rename("usuarios_temp.txt", "usuarios.txt");

    salvarMoedas(moedas);

    printf("\nCriptomoeda excluida com sucesso!\n");
    Sleep(1500);
}

void consultarSaldoInvestidor(){
    char cpfFormatado[15];
    struct User investidor;

    if (!lerCPFInvestidor(cpfFormatado)) {
        return;
    }

    if (!buscarUsuario(cpfFormatado, &investidor)) {
        printf("\nInvestidor nao encontrado.\n");
        Sleep(1500);
        return;
    }

    mostrarSaldo(investidor);
}

void consultarExtratoInvestidor(){
    char cpfFormatado[15];
    struct User investidor;

    if (!lerCPFInvestidor(cpfFormatado)) {
        return;
    }

    if (!buscarUsuario(cpfFormatado, &investidor)) {
        printf("\nInvestidor nao encontrado.\n");
        Sleep(1500);
        return;
    }

    mostrarExtrato(investidor);
}