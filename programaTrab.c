/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "createtable.h"
#include "select.h"
#include "select_where.h"
#include "createindex.h"
#include "insertinto.h"
#include "delete.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    //funcionalidade escolhida e arquivos
    int funcionalidade;
    scanf("%d", &funcionalidade);

    char arquivobin[50];
    char arquivoindex[50];
    int n, m;

    switch (funcionalidade) {

        //funcionalidade 1:
        case 1:
            //obtem-se os nomes dos arquivos a serem abertos
            char arquivocsv[50];
            scanf("%s", arquivocsv);
            scanf("%s", arquivobin);
            //realiza a transferência de dados do CSV para o binário
            transferirDados_CSV_BINARIO(arquivocsv, arquivobin);

            //utiliza a função binarioNaTela para correção
            binarioNaTela(arquivobin);
            
            break;

        //funcionalidade 2
        case 2:
            //obtem-se os nomes dos arquivos a serem abertos
            scanf("%s", arquivobin);

            //lê o arquivo binário especificado e imprime os registros necessarios
            leituraBinario(arquivobin);

            break;

        //funcionalidade 3
        case 3:
            //obtem-se os nomes dos arquivos a serem abertos
            scanf("%s", arquivobin);
            
            //obtem-se a quantidade de buscas serão realizadas
            
            scanf("%d", &n);

            //m sera a quantidade de criterios utilizados em cada uma das n buscas
            

            //loop para realizar as n buscas
            for(int i = 0; i < n; i++) {
                //le a quantidade de criterios para a busca i
                scanf("%d", &m);
                //cria um vetor da struct "criterio_busca" para colocar os criterios e as strings/inteiros buscados
                criterio_busca criterio[m];

                printf("Busca %d\n\n", (i+1));

                //le todos os criterios e armazena no vetor da struct "criterio_busca"
                for(int j = 0; j < m; j++) {
                    scanf("%s", criterio[j].tipo_buscado);
                    scan_quote_string(criterio[j].buscado);
                }

                //funçao para ler o binario especificado e imprimir os registros conforme os criterios
                leitura_buscaBinario(m, criterio, arquivobin);
            }

            break;

        //funcionalidade 4
        case 4:
            //obtem-se os nomes dos arquivos a serem abertos
            
            scanf("%s", arquivobin);
            scanf("%s", arquivoindex);

            //se a indexação foi bem-sucedida, printa a função binarioNaTela
            if(indexarDados(arquivobin, arquivoindex) == 0){
                binarioNaTela(arquivoindex);

            //se não, printa uma mensagem de erro
            } else {
                printf("Falha no processamento do arquivo.\n");
            }

            break;
        case 5:
            //obtem-se os nomes dos arquivos a serem abertos
            scanf("%s", arquivobin);
            scanf("%s", arquivoindex);
            
            //obtem-se a quantidade de buscas serão realizadas
            scanf("%d", &n);

            //m sera a quantidade de criterios utilizados em cada uma das n buscas
        
            //loop para realizar as n buscas
            for(int i = 0; i < n; i++) {
                //le a quantidade de criterios para a busca i
                scanf("%d", &m);
                //cria um vetor da struct "criterio_busca" para colocar os criterios e as strings/inteiros buscados
                criterio_busca criterio[m];

                //le todos os criterios e armazena no vetor da struct "criterio_busca"
                for(int j = 0; j < m; j++) {
                    scanf("%s", criterio[j].tipo_buscado);
                    scan_quote_string(criterio[j].buscado);
                }

                //funçao para ler o binario especificado e imprimir os registros conforme os criterios
                leitura_busca(m, criterio, arquivobin, arquivoindex);
            }

            //atualiza_prox(arquivobin);
            binarioNaTela(arquivobin);
            
            //cria o arquivo de indice e printa o binario na tela
            if(indexarDados(arquivobin, arquivoindex) == 0){
                binarioNaTela(arquivoindex);

            //se não, printa uma mensagem de erro
            } else {
                printf("Falha no processamento do arquivo.\n");
            }

            break;

        case 6:
            int id, idade;
            char nomeJogador[50];
            char nacionalidade[50];
            char nomeClube[50];
                //leitura dos nomes dos arquivos e número de entradas
            scanf("%s", arquivobin);
            scanf("%s", arquivoindex);
            scanf("%d", &n);

            //loop para ler e inserir os dados
            for (int i = 0; i < n; i++) {
                scanf("%d", &id);
                char temp[50];

                //verificação e leitura da idade
                scanf("%s", temp);
                if (strcmp(temp, "NULO") == 0) {
                    idade = -1; // Indicador de idade nula
                } else {
                    idade = atoi(temp);
                }

                //verificação e leitura do nome do jogador
                scan_quote_string(temp);
                if (strcmp(temp, "NULO") == 0) {
                    strcpy(nomeJogador, "");
                } else {
                    strcpy(nomeJogador, temp);
                }

                //verificação e leitura da nacionalidade
                scan_quote_string(temp);
                if (strcmp(temp, "NULO") == 0) {
                    strcpy(nacionalidade, "");
                } else {
                    strcpy(nacionalidade, temp);
                }

                //verificação e leitura do nome do clube
                scan_quote_string(temp);
                if (strcmp(temp, "NULO") == 0) {
                    strcpy(nomeClube, "");
                } else {
                    strcpy(nomeClube, temp);
                }

                //insere os dados no arquivo de dados
                inserirDados(arquivobin, id, idade, nomeJogador, nacionalidade, nomeClube);
            }
            
            binarioNaTela(arquivobin);
            
            //atualiza o arquivo index com os registros atualizados
            indexarDados(arquivobin, arquivoindex);
            binarioNaTela(arquivoindex);

            break;
        default:
            //caso nenhuma das 6 funcionalidades não forem chamadas, dá uma mensagem de erro
            printf("Falha no processamento do arquivo.\n");
    }

    return 0;
}