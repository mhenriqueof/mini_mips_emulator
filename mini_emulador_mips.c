# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

// Funcoes
void linha() {
    printf("--------------------------------------------------\n");
}

void abertura() {
    printf("Mini Emulador MIPS\n");
    sleep(1);
    printf("  Operacoes disponiveis\n");
    printf("    Aritmeticas: add, sub, mul, div\n");
    printf("    Imediatas: addi, li\n");
    printf("    Salto: j\n");
    sleep(1);
    printf("  Orientacoes\n");
    printf("    Utilizar apenas letras minusculas\n");
    printf("    Utilizar espaco depois da virgula (div $s3, $s0, $s5)\n");
    printf("    Utilizar somente registradores do tipo saved (s0, s1, ..., s7)\n");
    sleep(1);
    printf("  Digite 0 para sair\n");
    sleep(1);
    linha();
} 

int mostra_main_pc_instrucoes_registradores(char **log_instrucoes, int quantidade_instrucoes, int registradores[]) {
    printf("main:\n");
    for (int i = 0; i < quantidade_instrucoes; i++) {
        printf("    %02d - %s\n", 4*i, log_instrucoes[i]);
    }
    printf("\n");
    for (int i = 0; i <= 7; i++) {
        printf("s%d: %d\n", i, registradores[i]);
    }
    linha();
    return 1;
}

int recebe_instrucao(char instrucao[], int tamanho_instrucao,int quantidade_log) {
    printf("Instrucao %d: ", quantidade_log+1);
    fflush(stdin);
    fgets(instrucao, tamanho_instrucao, stdin);
    instrucao[strcspn(instrucao, "\n")] = '\0';
    if (instrucao[0] == '0') {return 0;}
    printf("\n");
    return 1;
}

void atualiza_log_quantidade_instrucoes(char **log_instrucoes, char instrucao[], int *quantidade_instrucoes) {
    if (instrucao[0] == 'j') {return;}
    strcpy(log_instrucoes[*quantidade_instrucoes], instrucao);
    *quantidade_instrucoes += 1;
}

void instrucao_invalida() {
    printf("Instrucao invalida.\n");
    sleep(1.5);
    linha();
}

void falha_execucao() {
    printf("Falha na execucao da instrucao.\n");
    sleep(1.5);
    linha();
}

// Verificar instrucao
int verifica_quantidade_partes();
int verifica_parte_inicial();
int verifica_parte_central();
int verifica_parte_final();
int verifica_instrucao(char instrucao[], int *quantidade_partes) {
    if (!verifica_quantidade_partes(instrucao, quantidade_partes)) {return 0;} 
    if (!verifica_parte_inicial(instrucao, *quantidade_partes)) {return 0;}
    if (!verifica_parte_central(instrucao, *quantidade_partes)) {return 0;}
    if (!verifica_parte_final(instrucao, *quantidade_partes)) {return 0;}
    return 1;
}

int verifica_quantidade_partes(char instrucao[], int *quantidade_partes) {
    char copia_instrucao[30];
    strcpy(copia_instrucao, instrucao);

    char *token = strtok(copia_instrucao, " ");

    int cont = 0;
    while (token != NULL) {
        token = strtok(NULL, " ");
        cont++;
        if (cont > 4) {return 0;}
    } if (cont < 2) {return 0;}
    *quantidade_partes = cont;
    return 1;
}

int instrucao_com_2_partes();
int instrucao_com_3_partes();
int instrucao_com_4_partes();
int verifica_parte_inicial(char instrucao[], int quantidade_partes) {
    char copia_instrucao[30];
    strcpy(copia_instrucao, instrucao);

    char *operacao = strtok(copia_instrucao, " ");

    if (instrucao_com_2_partes(operacao) && quantidade_partes == 2) {return 1;}
    if (instrucao_com_3_partes(operacao) && quantidade_partes == 3) {return 1;}
    if (instrucao_com_4_partes(operacao) && quantidade_partes == 4) {return 1;} 
    return 0;
} int instrucao_com_2_partes(char operacao[]) {
    return strcmp(operacao, "j") == 0;
} int instrucao_com_3_partes(char operacao[]) {
    return strcmp(operacao, "li") == 0;
} int instrucao_com_4_partes(char operacao[]) {
    return strcmp(operacao, "add") == 0 || strcmp(operacao, "sub") == 0 || strcmp(operacao, "mul") == 0 || strcmp(operacao, "div") == 0 || strcmp(operacao, "addi") == 0;
}

int verifica_ser_registrador();
int verifica_parte_central(char instrucao[], int quantidade_partes) {
    if (quantidade_partes == 2) {return 1;}

    char copia_instrucao[30];
    strcpy(copia_instrucao, instrucao);

    char *operacao = strtok(copia_instrucao, " ");

    for (int i = 2; i <= quantidade_partes-1; i++) {
        char *token = strtok(NULL, " ");
        if (!verifica_ser_registrador(token, i)) {return 0;}
    }
    return 1;
} int verifica_ser_registrador(char token[], int i) {
    int tamanho = strlen(token);
    if (i == 2 && tamanho != 4) {return 0;}
    if (i == 3 && tamanho != 4) {return 0;}
    if (i == 4 && tamanho != 3) {return 0;}
    if (token[0] != '$') {return 0;}
    if (token[1] != 's') {return 0;}
    if (token[2] < 48 || token[2] > 55) {return 0;}
    if (token[3] != ',' && tamanho == 4) {return 0;}
    return 1;
}

int final_com_registrador();
int final_com_numero();
int final_com_label();
int verifica_ser_registrador();
int verifica_ser_numero();
int verifica_ser_label();
int verifica_parte_final(char instrucao[], int quantidade_partes) {
    char copia_instrucao[30];
    strcpy(copia_instrucao, instrucao);

    char *operacao = strtok(copia_instrucao, " ");

    if (final_com_registrador(operacao)) {
        for (int i = 2; i <= quantidade_partes; i++) {
            char *token = strtok(NULL, " ");
            if (i == quantidade_partes) {
                if (!verifica_ser_registrador(token, 4)) {return 0;}
            }
        }
    } 
    if (final_com_numero(operacao)) {
        for (int i = 2; i <= quantidade_partes; i++) {
            char *token = strtok(NULL, " ");
            if (i == quantidade_partes) {
                if (!verifica_ser_numero(token)) {return 0;}
            }
        }
    }
    if (final_com_label(operacao)) {
        for (int i = 2; i <= quantidade_partes; i++) {
            char *token = strtok(NULL, " ");
            if (i == quantidade_partes) {
                if (!verifica_ser_label(token)) {return 0;}
            }
        }
    }  
    return 1;
} int final_com_registrador(char operacao[]) {
    return strcmp(operacao, "add") == 0 || strcmp(operacao, "sub") == 0 || strcmp(operacao, "mul") == 0 || strcmp(operacao, "div") == 0;
} int final_com_numero(char operacao[]) {
    return strcmp(operacao, "li") == 0 || strcmp(operacao, "addi") == 0;
} int final_com_label(char operacao[]) {
    return strcmp(operacao, "j") == 0;
} int verifica_ser_numero(char token[]) {
    int tamanho = strlen(token);
    for (int i = 0; i < tamanho; i++) {
        if (i == 0 && token[0] == '-' && tamanho != 1) {continue;}
        if (token[i] < 48 || token[i] > 57) {return 0;}
    }
    return 1;
} int verifica_ser_label(char token[]) {
    if (strcmp(token, "main") != 0) {return 0;}
    return 1;
}  

// Executar instrucao
int *obtem_ponteiro(char operando, int registradores[]);
int executa_instrucao(char instrucao[], int quantidade_partes, int *quantidade_instrucoes, int registradores[]) {
    char copia_instrucao[30];
    strcpy(copia_instrucao, instrucao);

    char *operacao = strtok(copia_instrucao, " ");
    char *parte1, *parte2, *parte3;

    if (quantidade_partes == 4) {
        parte1 = strtok(NULL, " ");
        parte2 = strtok(NULL, " ");
        parte3 = strtok(NULL, " ");
        
        char operando1 = parte1[2], operando2 = parte2[2], operando3 = parte3[2];
        int *registrador1 = obtem_ponteiro(operando1, registradores);
        int *registrador2 = obtem_ponteiro(operando2, registradores);
        int *registrador3 = obtem_ponteiro(operando3, registradores);

        if (strcmp(operacao, "add") == 0) {
            *registrador1 = *registrador2 + *registrador3;
            return 1;
        }
        if (strcmp(operacao, "sub") == 0) {
            *registrador1 = *registrador2 - *registrador3;
            return 1;
        }
        if (strcmp(operacao, "mul") == 0) {
            *registrador1 = *registrador2 * *registrador3;
            return 1;
        }
        if (strcmp(operacao, "div") == 0) {
            *registrador1 = *registrador2 / *registrador3;
            return 1;
        }
        if (strcmp(operacao, "addi") == 0) {
            *registrador1 = *registrador2 + atoi(parte3);
            return 1;
        }
    }
    if (quantidade_partes == 3) {
        parte1 = strtok(NULL, " ");
        parte2 = strtok(NULL, " ");
        
        char operando1 = parte1[2];
        int *registrador1 = obtem_ponteiro(operando1, registradores);

        if (strcmp(operacao, "li") == 0) {
            *registrador1 = atoi(parte2);
            return 1;
        }
    }
    if (quantidade_partes == 2) {
        if (strcmp(operacao, "j") == 0) {
            *quantidade_instrucoes = 0;
            return 1;
        }
    }
    return 0;
} int *obtem_ponteiro(char operando, int registradores[]) {
    if (operando == '0') {return &registradores[0];}
    if (operando == '1') {return &registradores[1];}
    if (operando == '2') {return &registradores[2];}
    if (operando == '3') {return &registradores[3];}
    if (operando == '4') {return &registradores[4];}
    if (operando == '5') {return &registradores[5];}
    if (operando == '6') {return &registradores[6];}
    if (operando == '7') {return &registradores[7];}
}

// Alocacao dinamica
char **aloca_log_instrucoes(int linhas, int colunas) {
    char **log_instrucoes = (char**) malloc(linhas * sizeof(char*));
    if (log_instrucoes == 0) {
        printf("Falha ao alocar log instrucoes.\n");
        exit(1);
    }
    for (int i = 0; i < linhas; i++) {
        log_instrucoes[i] = (char*) malloc(colunas * sizeof(char));
        if (log_instrucoes[i] == 0) {
            printf("Falha ao alocar log instrucoes.\n");
            for (int j = 0; j < i; j++) {
                free(log_instrucoes[j]);
            }
            free(log_instrucoes);
            exit(1);
        }
    }
    return log_instrucoes;
}

char **realoca_log_instrucoes(char **log_instrucoes, int linhas_atual, int linhas_novas, int colunas) {
    char **novo_log_instrucoes = (char**) realloc(log_instrucoes, linhas_novas * sizeof(char*));
    if (novo_log_instrucoes == 0) {
        printf("Falha ao realocar log instrucoes.\n");
        exit(1);
    }
    for (int i = linhas_atual; i < linhas_novas; i++) {
        novo_log_instrucoes[i] = (char*) malloc(colunas* sizeof(char));
        if (novo_log_instrucoes[i] == 0) {
            printf("Falha ao realocar log instrucoes.\n");
            for (int j = linhas_atual; j < i; j++) {
                free(novo_log_instrucoes[j]);
            }
            exit(1);
        }
    }
    return novo_log_instrucoes;
}

void libera_log_instrucoes(char **log_instrucoes, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(log_instrucoes[i]);
    }
    free(log_instrucoes);
}

// Emulador
int main() {
    int registradores[] = {0,0,0,0,0,0,0,0}, tamanho_instrucao = 30, quantidade_instrucoes = 0, quantidade_partes;
    char **log_instrucoes = aloca_log_instrucoes(1, tamanho_instrucao), instrucao[tamanho_instrucao];
    
    abertura();
    
    while (mostra_main_pc_instrucoes_registradores(log_instrucoes, quantidade_instrucoes, registradores)) {
        if (recebe_instrucao(instrucao, tamanho_instrucao, quantidade_instrucoes)) {
            if (verifica_instrucao(instrucao, &quantidade_partes)) {
                if (executa_instrucao(instrucao, quantidade_partes, &quantidade_instrucoes, registradores)) {
                    atualiza_log_quantidade_instrucoes(log_instrucoes, instrucao, &quantidade_instrucoes);
                    log_instrucoes = realoca_log_instrucoes(log_instrucoes, quantidade_instrucoes, quantidade_instrucoes+1, tamanho_instrucao);
                } else {falha_execucao();}
            } else {instrucao_invalida();}
        } else {break;}
    }

    libera_log_instrucoes(log_instrucoes, quantidade_instrucoes);
    return 0;
}