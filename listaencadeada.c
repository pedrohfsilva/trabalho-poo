/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "listaencadeada.h"
#include "funcoes_gerais.h"

//auxiliar da funcao adicionarNoRemocao, para buscar a posicao na lista em que sera inserido o novo NO e inserir
//a lista encadeada sera ordenada pelo tamanho dos registros
void adicionarNO(NO **listaRem, NO *novo) {
    if (*listaRem == NULL || (*listaRem)->tamanhoRegistro > novo->tamanhoRegistro) {
        novo->prox = *listaRem;
        *listaRem = novo;
    } else {
        NO *atual = *listaRem;
        while (atual->prox != NULL && atual->prox->tamanhoRegistro <= novo->tamanhoRegistro) {            
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        atual->prox = novo;
    }
}

//função para criar um NO e adicionar na lista de removidos
void adicionarNoRemocao(NO **listaNovosRem, registro_dados *reg, long int byteoffset) {
    //aloca o novo nó
    NO *novo = (NO *)malloc(sizeof(NO));
    if(novo == NULL) {
        printf("Falha ao alocar memória.\n");
        return;
    }

    //atribui os valores do novo nó a ele
    novo->tamanhoRegistro = reg->tamanhoRegistro;
    novo->byteOffset = byteoffset;
    novo->prox = NULL;
    novo->id = reg->id;

    //adiciona o nó na lista de novos removidos
    adicionarNO(listaNovosRem, novo);
}

//funcao para ler o arquivo de dados e criar uma lista de NOS com todos os registros removidos
NO *listarRemovidos(FILE *arquivoDados, cabecalho_ *cabecalho){
    NO *listaRem = NULL, *novo, *ultimo = NULL;
    long int byteOffsetAtual;
    char removido[1];
    int quantidadeRem = 0;
    //para pegar os registros removidos, basta iniciar do topo do cabecalho
    byteOffsetAtual = cabecalho->topo;

    //o ultimo removido tera como "prox" -1
    //logo, enquanto o "prox" dos registros removidos visitados nao for -1, ainda havera removidos para adicionar
    while(byteOffsetAtual != -1) {
        //vai para o byteoffset atual, cria e le o registro
        fseek(arquivoDados, byteOffsetAtual, SEEK_SET);
        registro_dados *reg = lerReg(arquivoDados);

        //funcao para inserir o novo registro na lista de removidos
        adicionarNoRemocao(&listaRem, reg, byteOffsetAtual);

        //atualiza o byteoffset atual
        byteOffsetAtual = reg->prox;

        //libera o registro
        liberarRegistro(reg);
    }

    //retorna a lista
    return listaRem;
}

//função para liberar memória da lista de removidos
void liberarLista(NO *lista){
    NO *temp;
    while(lista != NULL){
        temp = lista;
        lista = lista->prox;
        free(temp);
    }
}


//função que atualiza os prox dos registros
void atualiza_rem(FILE *arquivobin, NO *listaRemovidos) {
    NO *tmp = listaRemovidos;
    
    //enquanto houver registros removidos na lista, entra no loop
    while(tmp != NULL) {
        //posiciona o ponteiro para o byteoffset do registro removido atual
        fseek(arquivobin, tmp->byteOffset, SEEK_SET);

        //marca o registro como removido escrevendo '1' no dado "removido" do registro
        fwrite("1", sizeof(char), 1, arquivobin);

        //vai para a posicao do "prox" do registro removido para atualizar 
        fseek(arquivobin, tmp->byteOffset + sizeof(char) + sizeof(int), SEEK_SET);
        //se existir um proximo removido, escreve o byteoffset dele no "prox" do registro removido atual
        if(tmp->prox != NULL) {
            fwrite(&tmp->prox->byteOffset, sizeof(long int), 1, arquivobin);
        } else {
        //se nao existir, escreve "-1"
            long int escrever = -1;
            fwrite(&escrever, sizeof(long int), 1, arquivobin);
        }

        //continua percorrendo a lista de removidos
        tmp = tmp->prox;
    }
    //tmp recebe o primeiro removido da lista
    tmp = listaRemovidos;

    //vai para o campo "topo" do cabecalho para escrever o byteoffset do primeiro registro da lista
    fseek(arquivobin, 1, SEEK_SET);
    fwrite(&tmp->byteOffset, sizeof(long int), 1, arquivobin);
}
