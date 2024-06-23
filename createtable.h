#ifndef CREATETABLE_H
    #define CREATETABLE_H

    #include "funcoes_gerais.h"

    void escreverBinario(FILE *arquivobin, registro_dados *reg);
    void inserirEspaco(char *str);
    void transferirDados_CSV_BINARIO(char *nomeArquivo, char *nomeArquivoBin);
#endif
