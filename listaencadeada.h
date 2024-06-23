/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#ifndef LISTA_ENCADEADA_H
    #define LISTA_ENCADEADA_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include "funcoes_gerais.h"

    //struct NO utilizada para a lista de removidos com as informaçoes necessarias
    typedef struct NO {
        int id;
        int tamanhoRegistro;
        long int byteOffset;
        struct NO *prox;
    } NO;

    //cria um NO e adiciona na lista de removidos
    void adicionarNoRemocao(NO **listaNovosRem, registro_dados *reg, long int byteoffset);
    //auxiliar da funcao adicionarNoRemocao, para buscar a posicao na lista em que sera inserido o novo NO e inserir
    void adicionarNO(NO **listaRem, NO *novo);
    //cria a lista com todos os removidos do arquivo binario
    NO *listarRemovidos(FILE *arquivoDados, cabecalho_ *cabecalho);
    //atualiza os campos "removido" e "prox" dos registros logicamente removidos
    void atualiza_rem(FILE *arquivobin, NO *listaRemovidos); 
    //libera a lista de removidos
    void liberarLista(NO *lista);

#endif
