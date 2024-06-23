/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#ifndef FUNCOES_GERAIS_H
    #define FUNCOES_GERAIS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    

    //struct para os criterios de busca da funcionalidade 3
    typedef struct {
        char tipo_buscado[50];
        char buscado[50];
    } criterio_busca;

    //struct para os dados variaveis
    typedef struct {
        int tamanho;
        char *string;
    } string_variavel;

    //struct para o registro de dados
    typedef struct {
        char removido[1];
        int tamanhoRegistro;
        long int prox;
        int id;
        int idade;
        string_variavel nomeJogador;
        string_variavel nacionalidade;
        string_variavel nomeClube;
    } registro_dados;

    //struct para o cabeçalho
    typedef struct {
        char status; //enquanto o arquivo estiver aberto, sera 0. Ao finalizar o uso do arquivo, o status sera 1.
        long int topo;
        long int proxByteOffset;
        int nroRegArq;
        int nroRegRem;   
    } cabecalho_;

    //struct que sera escrita no arquivo de indice
    typedef struct {
        int id; 
        long int byteOffset;
    } registro_indice;

    //funçoes utilizadas em todas as funcionalidades:

    //funcoes para abrir arquivos para escrita ou leitura
    FILE *abrirArquivoCSV_leitura(char* nomeArquivo);
    FILE *abrirArquivoBinario_escrita_e_leitura(char* nomeArquivo);
    FILE *abrirArquivoBinario_leitura(char* nomeArquivo);
    FILE *abrirArquivoIndex_escrita_e_leitura(char *nomeArquivo);
    FILE *abrirArquivoIndex_leitura(char *nomeArquivo);

    //funcao para verificar o status do arquivo (0 ou 1)
    int verificarConfiabilidade(FILE *arquivoDados);

    //funcoes para lidar com o cabecalho:
    //le o cabecalho e retorna
    cabecalho_ *lerCabecalho(FILE* arquivobin);
    //reescreve todo o cabecalho no arquivo binario
    void atualizarCabecalho(FILE *arquivoDados, cabecalho_ *cabecalho); 

    //funcoes para lidar com os registros:
    //le o registro e retorna
    registro_dados* lerReg(FILE* arquivobin);
    //libera o registro 
    void liberarRegistro(registro_dados* reg);
    
    //funcao utilizada nas funcionalidades que precisam realizar busca por registros
    bool verificarCriterio(registro_dados* reg, criterio_busca* criterio);
#endif
