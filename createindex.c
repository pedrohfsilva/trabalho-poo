/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "funcoes_gerais.h"
#include "createindex.h"

//função para comparar dois registros de índice
int comparar(const void *a, const void *b){
    registro_indice *regA = (registro_indice *)a;
    registro_indice *regB = (registro_indice *)b;
    return(regA->id - regB->id);
}

//função para ler um registro do arquivo de dados
int lerRegistro(FILE *arquivoDados, long int byteOffset, int *id, int *tamReg, char *removido){
    fseek(arquivoDados, byteOffset, SEEK_SET);

    if(fread(removido, sizeof(char), 1, arquivoDados) < 1){
        return 0; //fim do arquivo
    }

    fread(tamReg, sizeof(int), 1, arquivoDados);
    if(strcmp(removido, "1") == 0){
        return 1; //registro removido
    }

    fseek(arquivoDados, sizeof(long int), SEEK_CUR); //pula a variável prox (long int)
    fread(id, sizeof(int), 1, arquivoDados);
    return 2; //registro válido
}

//função para escrever os registros ordenados no arquivo de índice
void escreverIndice(FILE *arquivoIndice, registro_indice *registros, int quantidadeRegistros){
    for(int i = 0; i < quantidadeRegistros; i++){
        fwrite(&registros[i].id, sizeof(int), 1, arquivoIndice);
        fwrite(&registros[i].byteOffset, sizeof(long int), 1, arquivoIndice);
    }

    fseek(arquivoIndice, 0, SEEK_SET);
    char status = '1';
    fwrite(&status, sizeof(char), 1, arquivoIndice);
}

//função para criar vetor de índices
registro_indice *criarVetorIndices(FILE *arquivoDados, int quantidadeRegistros){
    int id, tamReg;
    char removido[1];
    long int byteOffsetAtual = 25; //posição inicial de leitura após o cabeçalho do arquivo de dados

    //cria e aloca o vetor de registros
    registro_indice *registros = (registro_indice *)malloc(quantidadeRegistros * sizeof(registro_indice));
    if(registros == NULL){
        printf("Falha ao alocar memória.\n");
        return NULL;
    }

    int indice = 0;
    while(1){
        int resultado = lerRegistro(arquivoDados, byteOffsetAtual, &id, &tamReg, removido);
        if(resultado == 0){
            break; //fim do arquivo
        }
        if(resultado == 1){
            byteOffsetAtual += tamReg;
            continue; //registro removido
        }

        //insere os valores na lista de registros e atualiza o indice
        registros[indice].id = id;
        registros[indice].byteOffset = byteOffsetAtual;
        indice++;
        byteOffsetAtual += tamReg;
    }

    //ordena a lista de registros em relação a id
    qsort(registros, quantidadeRegistros, sizeof(registro_indice), comparar);

    return registros;
}

//função para indexar dados
int indexarDados(char *nomeArquivoDados, char *nomeArquivoIndice){
    //abre os arquivos de dados e de indice
    FILE *arquivoDados = abrirArquivoBinario_leitura(nomeArquivoDados);
    if(arquivoDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    FILE *arquivoIndice = abrirArquivoIndex_escrita_e_leitura(nomeArquivoIndice);
    if(arquivoIndice == NULL){
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoDados);
        return -1;
    }

    //verifica os status de cada arquivo
    if(!verificarConfiabilidade(arquivoDados)){
        fclose(arquivoDados);
        fclose(arquivoIndice);
        return -1;
    }
    
    //lê o cabeçalho do arquivo de dados
    cabecalho_ *cabecalho = lerCabecalho(arquivoDados);

    //cria um vetor de indices ordenado do arquivo de dados
    registro_indice *registros = criarVetorIndices(arquivoDados, cabecalho->nroRegArq);
    if(registros == NULL){
        fclose(arquivoDados);
        fclose(arquivoIndice);
        return -1;
    }

    //escreve esse vetor de indices no arquivo de indices
    escreverIndice(arquivoIndice, registros, cabecalho->nroRegArq);

    //libera memória das alocações utilizadas
    free(cabecalho);
    free(registros);
    fclose(arquivoDados);
    fclose(arquivoIndice);

    return 0;
}
