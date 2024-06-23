/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "funcoes_gerais.h"
#include "insertinto.h"
#include "listaencadeada.h"

//função para remover um nó utilizado da lista
void removerNO(NO **lista, long int bestFit, FILE *arquivoDados){

    //variáveis atual e anterior
    NO *atual = *lista;
    NO *anterior = NULL;

    //enquanto existir um nó a ser checado, entra no loop
    while(atual != NULL){

        //se o nó checado é o bestFit,
        //se esse nó for o topo da lista, muda o topo da lista
        //se não, a lista irá pular o nó atual
        if(atual->byteOffset == bestFit){
            if(anterior == NULL){
                *lista = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }

            free(atual);
            return;
        }

        //atualiza o nó a ser checado e o nó anterior
        anterior = atual;
        atual = atual->prox;
    }
}

//função para encontrar o bestFit do registro a ser inserido
NO *bestFit(int tamReg, NO *lista){
    int menorDif, atualDif; //diferença atual e menor diferença entre o registro removido e o inserido
    NO *bestFit = NULL;
    int flag = 0; //checa se há pelo menos um registro com uma diferença positiva

    //enquanto existir um nó a ser checado na lista, entra no loop
    while(lista != NULL){

        //se não houver nenhuma diferença válida, menorDif recebe atualDif
        atualDif = lista->tamanhoRegistro - tamReg;
        if(flag == 0){
            menorDif = atualDif;
            bestFit = lista;
            //se a diferença for positiva, esse bestfit poderá ser utilizado
            if(menorDif >= 0){
                flag++;
            }
        } else {

            //se a diferença atual for menor que a menor diferença, bestfit será o nó atual
            if(atualDif < menorDif && atualDif >= 0){
                menorDif = atualDif;
                bestFit = lista;
            }
        }
        //se for encontrado um registro que não há diferença, sai do loop
        if(menorDif == 0){
            break;
        }
        //checa o próximo da lista
        lista = lista->prox;
    }

    return bestFit;
}

//função auxiliar para adicionar lixo à bestfits imperfeitos
void adicionarLixo(FILE *arquivoDados, long int posicao, int quantidade){
    char lixo = '$';
    fseek(arquivoDados, posicao, SEEK_SET);
    for(int i = 0; i < quantidade; i++){
        fwrite(&lixo, sizeof(char), 1, arquivoDados);
    }
}

//escreve o registro num espaço determinado e adiciona lixo aos espaços não utilizados
void escreverRegistro(FILE *arquivoDados, long int insertByteOffset, int tamRegInsert, int id, int idade, char *nomeJogador, char *nacionalidade, char *nomeClube, int tamRemovido) {
    int tamNomeJogador = strlen(nomeJogador);
    int tamNacionalidade = strlen(nacionalidade);
    int tamNomeClube = strlen(nomeClube);

    char removido = '0';
    long int prox = -1;

    fseek(arquivoDados, insertByteOffset, SEEK_SET);
    fwrite(&removido, sizeof(char), 1, arquivoDados);
    fwrite(&tamRemovido, sizeof(int), 1, arquivoDados);
    fwrite(&prox, sizeof(long int), 1, arquivoDados);
    fwrite(&id, sizeof(int), 1, arquivoDados);
    fwrite(&idade, sizeof(int), 1, arquivoDados);
    fwrite(&tamNomeJogador, sizeof(int), 1, arquivoDados);
    fwrite(nomeJogador, sizeof(char), tamNomeJogador, arquivoDados);
    fwrite(&tamNacionalidade, sizeof(int), 1, arquivoDados);
    fwrite(nacionalidade, sizeof(char), tamNacionalidade, arquivoDados);
    fwrite(&tamNomeClube, sizeof(int), 1, arquivoDados);
    fwrite(nomeClube, sizeof(char), tamNomeClube, arquivoDados);

    //encontra quantos caracteres de lixo serão necessários
    int tamlixo = tamRemovido - tamRegInsert;
    //se há lixo necessário, adiciona esse lixo no registro
    if(tamlixo > 0){
        adicionarLixo(arquivoDados, insertByteOffset + tamRegInsert, tamlixo);
    }
}

//função principal para inserir um registro no arquivo binário
int inserirDados(char *nomeArquivoDados, int id, int idade, char *nomeJogador, char *nacionalidade, char *nomeClube){

    //abre o arquivo de dados
    FILE *arquivoDados = abrirArquivoBinario_escrita_e_leitura(nomeArquivoDados);
    if(arquivoDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    //aloca um novo cabeçalho e o completa com os valores atuais do arquivo
    cabecalho_ *cabecalho = lerCabecalho(arquivoDados);
    if(cabecalho == NULL){
        fclose(arquivoDados);
        return -1;
    }

    //obtem os tamanhos das strings
    int tamNomeJogador = strlen(nomeJogador);
    int tamNacionalidade = strlen(nacionalidade);
    int tamNomeClube = strlen(nomeClube);

    //tamRegInsert: tamanho do registro completo a ser inserido
    int tamRegInsert = sizeof(char) + sizeof(int) + sizeof(long int) + sizeof(int) + sizeof(int) + sizeof(int) + tamNomeJogador + sizeof(int) + tamNacionalidade + sizeof(int) + tamNomeClube;

    //insertByteOffset: byteOffset que será inserido o registro
    long int insertByteOffset = -1;
    //tamRemovido: tamanho do registro removido que será substituído
    int tamRemovido = -1;

    //se houver removidos no arquivo:
    if(cabecalho->nroRegRem > 0){

        //cria uma lista de removidos do arquivo
        NO *listaRemovidos = listarRemovidos(arquivoDados, cabecalho);
        //encontra o bestFit para o registro a ser inserido no arquivo de dados
        NO *best = bestFit(tamRegInsert, listaRemovidos);
        //se o bestFit foi encontrado, o byteOffset do registro a ser inserido recebe o byteOffset do bestFit
        if(best != NULL) insertByteOffset = best->byteOffset;

        //se o byteOffset for -1, o registro será escrito no fim do arquivo
        //se não, acha onde o registro deve ser escrito e lê o tamanho do registro removido, depois atualiza o numero de registros removidos
        if(insertByteOffset == -1){
            cabecalho->proxByteOffset += tamRegInsert;
            fseek(arquivoDados, sizeof(char) + sizeof(long int), SEEK_SET);
            fread(&insertByteOffset, sizeof(long int), 1, arquivoDados);
        } else {
            if(insertByteOffset == cabecalho->topo) cabecalho->topo = (best->prox)->byteOffset;
            fseek(arquivoDados, insertByteOffset + sizeof(char), SEEK_SET);
            fread(&tamRemovido, sizeof(int), 1, arquivoDados);
            cabecalho->nroRegRem--;
        }

        //remove o registro da lista de removidos
        removerNO(&listaRemovidos, insertByteOffset, arquivoDados);
        //atualiza o "prox" dos removidos
        atualiza_rem(arquivoDados, listaRemovidos);
        //depois, libera a memória da lista
        liberarLista(listaRemovidos);

    //se não houver registros removidos, o registro será inserido no fim do arquivo e atualiza o cabeçalho
    } else {
        insertByteOffset = cabecalho->proxByteOffset;
        cabecalho->proxByteOffset += tamRegInsert;
        tamRemovido = tamRegInsert;
    }

    //escreve o registro no byteOffset especificado
    escreverRegistro(arquivoDados, insertByteOffset, tamRegInsert, id, idade, nomeJogador, nacionalidade, nomeClube, tamRemovido);

    //atualiza a quantidade de registros no arquivo
    cabecalho->nroRegArq++;

    //atualiza o cabeçalho do arquivo
    atualizarCabecalho(arquivoDados, cabecalho);

    //fecha o arquivo e os elementos alocados
    free(cabecalho);
    fclose(arquivoDados);

    return 0;
}
