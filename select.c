/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "funcoes_gerais.h"
#include "select.h"

void leituraBinario(char *nomeArquivoBin) {
 //abre/cria o arquivo binario
    FILE *arquivobin = abrirArquivoBinario_leitura(nomeArquivoBin);   
    if(arquivobin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }   

    //aloca o cabeçalho
    cabecalho_ *cabecalho = (cabecalho_ *)malloc(sizeof(cabecalho_));
    if(cabecalho == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //faz a leitura do cabeçalho do binario
    fread(&cabecalho->status, sizeof(char), 1, arquivobin);
    fread(&cabecalho->topo, sizeof(long int), 1, arquivobin);
    fread(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivobin);
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivobin);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivobin);
    printf("Status: %c\nTopo: %li\nProxByte: %li\nRegs: %d\nRem: %d\n\n", cabecalho->status, cabecalho->topo, cabecalho->proxByteOffset, cabecalho->nroRegArq, cabecalho->nroRegRem);
    
    if(cabecalho->status == '0') {
        printf("Falha no processamento do arquivo\n");
        return;
    }

    //inicia em 25 pois é o tamanho do cabeçalho
    long int byteoffset_pularegistro = 25;
    
    //mantem a contagem de registros impressos 
    int registrosimpressos = 0;

    while(1) {
        fseek(arquivobin, byteoffset_pularegistro, SEEK_SET);
        //aloca o registro a ser utilizado
        registro_dados *reg = (registro_dados *) malloc(sizeof(registro_dados));
        if(reg == NULL) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        //le o campo removido do registro, e verifica se o registro existe
        if (fread(&reg->removido, sizeof(char), 1, arquivobin) < 1) {
            break;
        }
        reg->removido[1] = '\0';

        //le o tamanho do registro e atualiza o byteoffset_pularegistro
        fread(&reg->tamanhoRegistro, sizeof(int), 1, arquivobin);
        byteoffset_pularegistro += reg->tamanhoRegistro;

        //verifica se o registro esta marcado como removido
        //se estiver, pula para o proximo registro
        //se nao estiver, segue lendo e imprimindo o registro
        // if(strcmp(reg->removido, "a") == 0) {
        //     free(reg);
        //     fseek(arquivobin, byteoffset_pularegistro, SEEK_SET);
        //     continue;
        // }

        //a quantidade de registros impressos aumenta
        registrosimpressos++;

        //leitura dos campos de tamanho fixo do registro
        fread(&reg->prox, sizeof(long int), 1, arquivobin);
        fread(&reg->id, sizeof(int), 1, arquivobin);
        fread(&reg->idade, sizeof(int), 1, arquivobin);
        printf("idade %d\ntamanhoReg %d\n", reg->idade, reg->tamanhoRegistro);
        printf("prox: %ld\nid: %d\n", reg->prox, reg->id);
        printf("removido: %s\n", reg->removido);
        
        reg->nomeJogador.string = NULL;
        reg->nacionalidade.string = NULL;
        reg->nomeClube.string = NULL;

        //le o tamanho do nomeJogador
        fread(&reg->nomeJogador.tamanho, sizeof(int), 1, arquivobin);
        if(reg->nomeJogador.tamanho == 0) {
            printf("Nome do Jogador: SEM DADO\n");
        } else {
            //aloca o nome
            reg->nomeJogador.string = (char *)malloc(reg->nomeJogador.tamanho + 1);
        
            //le o nomeJogador, coloca um '\0' no fim da string e imprime
            fread(reg->nomeJogador.string, reg->nomeJogador.tamanho, 1, arquivobin);
            reg->nomeJogador.string[reg->nomeJogador.tamanho] = '\0';
            printf("Nome do Jogador: %s\n", reg->nomeJogador.string);
        }
        printf("Tamanho NomeJog: %d\n", reg->nomeJogador.tamanho);
        

        //le o tamanho da nacionalidade
        fread(&reg->nacionalidade.tamanho, sizeof(int), 1, arquivobin);
        //se for 0, imprime sem dado
        if(reg->nacionalidade.tamanho == 0) {
            printf("Nacionalidade do Jogador: SEM DADO\n");
        } else {
            //se tamanho for maior que 0, aloca a nacionalidade, faz a leitura, coloca o '\0' no final e imprime
            reg->nacionalidade.string = (char *)malloc(reg->nacionalidade.tamanho + 1);
            fread(reg->nacionalidade.string, reg->nacionalidade.tamanho, 1, arquivobin);
            reg->nacionalidade.string[reg->nacionalidade.tamanho] = '\0';
            printf("Nacionalidade do Jogador: %s\n", reg->nacionalidade.string);
        }
        printf("Tamanho Nacionalidade: %d\n", reg->nacionalidade.tamanho);
        
        //le o tamanho do nomeClube
        fread(&reg->nomeClube.tamanho, sizeof(int), 1, arquivobin);
        //se for 0, imprime "sem dado"
        if(reg->nomeClube.tamanho == 0) {
            printf("Clube do Jogador: SEM DADO\n\n");
        } else {
            //se tamanho for maior que 0, aloca, faz a leitura, coloca o '\0' no final e imprime o nomeClube
            reg->nomeClube.string = (char *)malloc(reg->nomeClube.tamanho + 1);
            fread(reg->nomeClube.string, reg->nomeClube.tamanho, 1, arquivobin);
            reg->nomeClube.string[reg->nomeClube.tamanho] = '\0';
            printf("Clube do Jogador: %s\n\n", reg->nomeClube.string);
        }
        printf("Tamanho Clube: %d\n", reg->nomeClube.tamanho);

        //da free em todos os campos alocados e da free no registrador
        if(reg->nomeJogador.string != NULL) free((reg->nomeJogador).string);
        if(reg->nacionalidade.string != NULL) free((reg->nacionalidade).string);
        if(reg->nomeClube.string != NULL) free((reg->nomeClube).string);
        if(reg != NULL) free(reg);
    }

    //se nenhum registro foi impresso, imprime "Registro inexistente"
    if(registrosimpressos == 0) {
        printf("Registro inexistente.\n\n");
    }

    //fecha o arquivo e da free no cabecalho
    free(cabecalho);
    fclose(arquivobin);    
}