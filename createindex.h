/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#ifndef CREATEINDEX_H

    #define CREATEINDEX_H

    #include "funcoes_gerais.h"

    int indexarDados(char *nomeArquivoDados, char *nomeArquivoIndice);
    registro_indice *criarVetorIndices(FILE *arquivoDados, int quantidadeRegistros);
#endif
