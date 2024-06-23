/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "funcoes_gerais.h"
#include "delete.h"
#include "select_where.h"
#include "createindex.h"
#include "listaencadeada.h"

//função para remover e atualizar registros no arquivo binário
long int removerReg(NO *listaRem, NO *listaNovosRem, FILE *arquivobin, int registrosremovidos){
    NO *atual;
    //se a lista de removidos for nula, ela sera exatamente a lista de novos removidos
    if(listaRem == NULL){
        listaRem = listaNovosRem;
    } else {
    //caso contrario, adiciona a lista de novos removidos à lista de removidos
        atual = listaRem;
        while(atual->prox != NULL){
            atual = atual->prox;
        }
        atual->prox = listaNovosRem;
    }

    //salva na variavel "novo_topo" o novo topo do cabecalho, que sera o byteoffset do primeiro elemento da lista de removidos
    long int novo_topo = -1;
    if(listaRem != NULL){
        novo_topo = listaRem->byteOffset;
    }

    //atualiza o arquivo binario com os novos "removido" e os novos "prox"
    atualiza_rem(arquivobin, listaRem);

    //retorna o novo topo para ser escrito no cabecalho
    return novo_topo;
}

//função para buscar um registro pelo ID usando busca binária, ja que o arquivo eh ordenado por id
long int buscaBinariaPorId(registro_indice *vetorInd, int nroRegArq, int id){
    int inicio = 0;
    int fim = nroRegArq - 1;

    //busca binaria
    while(inicio <= fim){
        int meio = (inicio + fim) / 2;
        if(vetorInd[meio].id == id){
            return vetorInd[meio].byteOffset;
        } else if (vetorInd[meio].id < id){
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    return -1; //ID não encontrado
}

//função para buscar e remover registro por ID
void buscarPorId(FILE *arquivobin, int id, NO **listaNovosRem, int *registrosremovidos, cabecalho_ *cabecalho){
    //cria o vetor dos registros que irao para o arquivo de indices
    registro_indice *vetorInd = criarVetorIndices(arquivobin, cabecalho->nroRegArq);
    
    //pega o byteoffset do id buscado, caso exista, por meio da busca binaria
    long int byteoffset_reg = buscaBinariaPorId(vetorInd, cabecalho->nroRegArq, id);

    //se o byteoffset com o id buscado nao existir, sera -1, entao nao ira remover nada
    //se existir, ira remover
    if(byteoffset_reg > 0){
        //vai ate o byteoffset e le o registro a ser removido
        fseek(arquivobin, byteoffset_reg, SEEK_SET);
        registro_dados *reg = lerReg(arquivobin);

        //adiciona o no removido na lista de novos removidos
        adicionarNoRemocao(listaNovosRem, reg, byteoffset_reg);
        //incrementa a qtd de registros removidos para atualizar o cabecalho depois
        (*registrosremovidos)++;
        
        //libera o registro
        liberarRegistro(reg);
    }
    
    //libera o vetor de indices
    free(vetorInd);
}

//função para buscar e remover registros com critérios variados
void buscarComCriterios(FILE *arquivobin, int m, criterio_busca* criterio, NO **listaNovosRem, int *registrosremovidos){
    //define os byteoffsets que serao usados para auxiliar
    //inicia com 25 pois eh o byteoffset do primeiro registro, pulando o cabecalho
    long int byteoffset_pularegistro = 25;
    long int byteoffset_atual = 25;
    //bool utilizado para verificar se foi encontrado ou nao
    bool encontrado;

    while(1){
        //vai para o byteoffset do proximo registro
        fseek(arquivobin, byteoffset_pularegistro, SEEK_SET);
        //aloca o registro e le ele
        registro_dados* reg = lerReg(arquivobin);
        if (reg == NULL) break;

        //define o novo byteoffset para "pular" o registro atual e ir para o proximo
        byteoffset_pularegistro += reg->tamanhoRegistro;

        //a busca esta sendo realizada para remover um registro especifico
        //entao, eh possivel pular os registros ja removidos, pois nao tem como remover eles novamente
        if(strcmp(reg->removido, "1") == 0){
            //libera o registro, atualiza o byteoffset atual e vai para a proxima iteraçao
            liberarRegistro(reg);
            byteoffset_atual = byteoffset_pularegistro;
            continue;
        }

        //verifica se o registro esta dentro dos criterios buscados
        encontrado = verificarCriterio(reg, &criterio[0]);
        for(int i = 1; i < m && encontrado; i++){
            encontrado = verificarCriterio(reg, &criterio[i]);
        }

        //se for um dos registros buscados para ser removido
        if(encontrado){
            //ele sera adicionado na lista de removidos, a quantidade de registros removidos ira aumentar
            adicionarNoRemocao(listaNovosRem, reg, byteoffset_atual);
            (*registrosremovidos)++;
            //marca ele como logicamente removido
            fseek(arquivobin, byteoffset_atual, SEEK_SET);
            fwrite("1", sizeof(char), 1, arquivobin);
        }

        //atualiza o byteoffset atual e libera o registro
        byteoffset_atual = byteoffset_pularegistro;
        liberarRegistro(reg);
    }
}

//função principal para fazer a busca dos registros com os critérios necessários e imprimir com a formatação solicitada
void leitura_busca(int m, criterio_busca* criterio, char* nomeArquivoBin, char *nomeArquivoIndex){
    //abre o arquivo binario para leitura e escrita
    FILE* arquivobin = abrirArquivoBinario_escrita_e_leitura(nomeArquivoBin);
    if(arquivobin == NULL) return;

    //cria e le o cabecalho do arquivo binario
    cabecalho_* cabecalho = lerCabecalho(arquivobin);
    if(cabecalho == NULL){
        fclose(arquivobin);
        return;
    }

    //variavel para manter controle da quantidade de novos registros removidos, para atualizar o cabecalho posteriormente
    int registrosremovidos = 0;
    //cria a lista de removidos que ja estao removidos no arquivo binario
    NO *listaRem = listarRemovidos(arquivobin, cabecalho);
    //cria a lista dos novos removidos que serao removidos na execucao da funcionalidade
    NO *listaNovosRem = NULL;

    //se o criterio buscado for "id", a busca sera realizada por meio do arquivo de indices
    //entao, chama a funcao para realizar essa busca
    if(strcmp("id", criterio->tipo_buscado) == 0){
        int id = atoi(criterio[0].buscado);
        buscarPorId(arquivobin, id, &listaNovosRem, &registrosremovidos, cabecalho);
    //se for qualquer um dos outros criterios, faz a busca assim como na funcionalidade 3
    } else {
        buscarComCriterios(arquivobin, m, criterio, &listaNovosRem, &registrosremovidos);
    }

    //chama a funcao para juntar as duas listas e atualizar todos os campos necessarios para os registros removidos
    //ela retorna o novo topo, para que seja possivel atualizar o cabecalho com o novo primeiro NO da lista
    long int topo = removerReg(listaRem, listaNovosRem, arquivobin, registrosremovidos);

    //vai para o inicio do arquivo, atualiza os valores necessarios o cabecalho
    fseek(arquivobin, 0, SEEK_SET);
    cabecalho->nroRegArq -= registrosremovidos;
    cabecalho->nroRegRem += registrosremovidos;
    cabecalho->status = '1';
    cabecalho->topo = topo;

    //escreve os novos valores do cabecalho no arquivo binario
    atualizarCabecalho(arquivobin, cabecalho);

    //libera o cabecalho e fecha o arquivo
    free(cabecalho);
    fclose(arquivobin);
}
