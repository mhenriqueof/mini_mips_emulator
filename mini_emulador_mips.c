# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

// Funcoes
void linha() {
    printf("--------------------------------------------------\n");
}

void menu_inicial() {
    printf("Mini Emulador MIPS\n");
    sleep(1);
    printf("  Operacoes disponiveis\n");
    printf("    Aritmeticas: add, sub, mul, div\n");
    printf("    Imediatas: addi, li\n");
    printf("    Salto: j\n");
    sleep(1);
    printf("  Regras\n");
    printf("    Utilizar apenas letras minusculas\n");
    printf("    Utilizar espaco depois das virgulas (div $s3, $s0, $s5)\n");
    printf("    Utilizar somente registradores do tipo saved (s0, s1, ..., s7)\n");
    sleep(1);
    linha();
} 

void mostra_main_pc_instrucoes_registradores(int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7, int pc, char **log_instrucoes) {
    printf("main:\n");
    int tamanho_instrucoes = pc / 4;
    for (int i = 0; i < tamanho_instrucoes; i++) {
        printf("    %02d - %s\n", 4*i, log_instrucoes[i+1]);
    }
    printf("\n");
    printf("s0: %d\n", s0);
    printf("s1: %d\n", s1);
    printf("s2: %d\n", s2);
    printf("s3: %d\n", s3);
    printf("s4: %d\n", s4);
    printf("s5: %d\n", s5);
    printf("s6: %d\n", s6);
    printf("s7: %d\n", s7);
    linha();
}

void recebe_instrucao(char instrucao[], int quantidade_log) {
    printf("Instrucao %d: ", quantidade_log);
    fflush(stdin);
    gets(instrucao);
    printf("\n");
}

void atualiza_quantidade_log_pc_log_instrucoes(int *quantidade_log, int *pc, char **log_instrucoes, char instrucao[]) {
    int posicao_log = *pc / 4;
    strcpy(log_instrucoes[posicao_log+1], instrucao);
    *pc += 4;
    *quantidade_log = (*pc+4)/4;
}

void instrucao_invalida() {
    printf("Instrucao invalida.\n");
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
    } 
    if (cont == 1) {return 0;}
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
    if (tamanho == 4) {
        if (token[3] != ',') {return 0;}
    }
    return 1;
}

int final_com_registrador();
int final_com_label();
int final_com_numero();
int verifica_ser_registrador();
int verifica_ser_label();
int verifica_ser_numero();
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
    if (final_com_label(operacao)) {
        for (int i = 2; i <= quantidade_partes; i++) {
            char *token = strtok(NULL, " ");
            if (i == quantidade_partes) {
                if (!verifica_ser_label(token)) {return 0;}
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
    return 1;
} int final_com_registrador(char operacao[]) {
    return strcmp(operacao, "add") == 0 || strcmp(operacao, "sub") == 0 || strcmp(operacao, "mul") == 0 || strcmp(operacao, "div") == 0;
} int final_com_label(char operacao[]) {
    return strcmp(operacao, "j") == 0;
} int final_com_numero(char operacao[]) {
    return strcmp(operacao, "li") == 0 || strcmp(operacao, "addi") == 0;
} int verifica_ser_label(char token[]) {
    if (strcmp(token, "main") != 0) {return 0;}
    return 1;
}  int verifica_ser_numero(char token[]) {
    int tamanho = strlen(token);
    for (int i = 0; i < tamanho; i++) {
        if (token[0] == '-') {continue;}
        if (token[i] < 48 || token[i] > 57) {return 0;}
    }
    return 1;
} 


// Executar instrucao
int *obtem_ponteiro(char operando, int *s0, int *s1, int *s2, int *s3, int *s4, int *s5, int *s6, int *s7);
int executa_instrucao(char instrucao[], int quantidade_partes, int *s0, int *s1, int *s2, int *s3, int *s4, int *s5, int *s6, int *s7, int *pc) {
    char copia_instrucao[30];
    strcpy(copia_instrucao, instrucao);

    char *operacao = strtok(copia_instrucao, " ");
    char *parte1, *parte2, *parte3;

    if (quantidade_partes == 4) {
        parte1 = strtok(NULL, " ");
        parte2 = strtok(NULL, " ");
        parte3 = strtok(NULL, " ");
        
        char operando1 = parte1[2], operando2 = parte2[2], operando3 = parte3[2];
        int *registrador1 = obtem_ponteiro(operando1, s0, s1, s2, s3, s4, s5, s6, s7);
        int *registrador2 = obtem_ponteiro(operando2, s0, s1, s2, s3, s4, s5, s6, s7);
        int *registrador3 = obtem_ponteiro(operando3, s0, s1, s2, s3, s4, s5, s6, s7);

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
        int *registrador1 = obtem_ponteiro(operando1, s0, s1, s2, s3, s4, s5, s6, s7);

        if (strcmp(operacao, "li") == 0) {
            *registrador1 = atoi(parte2);
            return 1;
        }
    }
    if (quantidade_partes == 2) {
        if (strcmp(operacao, "j") == 0) {
            *pc = -4;
            return 1;
        }
    }
    return 0;
} int *obtem_ponteiro(char operando, int *s0, int *s1, int *s2, int *s3, int *s4, int *s5, int *s6, int *s7) {
    if (operando == '0') {return s0;}
    if (operando == '1') {return s1;}
    if (operando == '2') {return s2;}
    if (operando == '3') {return s3;}
    if (operando == '4') {return s4;}
    if (operando == '5') {return s5;}
    if (operando == '6') {return s6;}
    if (operando == '7') {return s7;}
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

char **realoca_log_instrucoes(char **log_instrucoes, int linhas_atual, int novas_linhas, int colunas) {
    char **novo_log_instrucoes = (char**) realloc(log_instrucoes, novas_linhas * sizeof(char*));
    if (novo_log_instrucoes == 0) {
        printf("Falha ao realocar log instrucoes.\n");
        return 0;
    }

    for (int i = linhas_atual; i < novas_linhas; i++) {
        novo_log_instrucoes[i] = (char*) malloc(colunas* sizeof(char));
        if (novo_log_instrucoes[i] == 0) {
            printf("Falha ao realocar log instrucoes.\n");
            for (int j = linhas_atual; j < i; j++) {
                free(novo_log_instrucoes[j]);
            }
            return 0;
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
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0, pc = 0, quantidade_partes = 0, quantidade_log = 1;
    char **log_instrucoes = aloca_log_instrucoes(quantidade_log+1, 30);
    if (log_instrucoes == 0) {return 1;}
    
    menu_inicial();

    while (1) {
        mostra_main_pc_instrucoes_registradores(s0, s1, s2, s3, s4, s5, s6, s7, pc, log_instrucoes);

        char instrucao[30];
        recebe_instrucao(instrucao, quantidade_log);

        if (verifica_instrucao(instrucao, &quantidade_partes)) {
            if (executa_instrucao(instrucao, quantidade_partes, &s0, &s1, &s2, &s3, &s4, &s5, &s6, &s7, &pc)) {
                atualiza_quantidade_log_pc_log_instrucoes(&quantidade_log, &pc, log_instrucoes, instrucao);
                log_instrucoes = realoca_log_instrucoes(log_instrucoes, quantidade_log, quantidade_log+1, 30);
                if (log_instrucoes == 0) {break;}
                continue;
            }
        }
        instrucao_invalida();
    }
    libera_log_instrucoes(log_instrucoes, quantidade_log);
    return 0;
}
