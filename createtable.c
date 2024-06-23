/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/


#include "funcoes_gerais.h"
#include "createtable.h"

void escreverBinario(FILE *arquivobin, registro_dados *reg) {
    //escrever o registro no arquivo binario
    fwrite(&reg->removido, sizeof(char), 1, arquivobin);
    fwrite(&reg->tamanhoRegistro, sizeof(int), 1, arquivobin);
    fwrite(&reg->prox, sizeof(long int), 1, arquivobin);   
    fwrite(&reg->id, sizeof(int), 1, arquivobin);
    fwrite(&reg->idade, sizeof(int), 1, arquivobin);
    fwrite(&(reg->nomeJogador).tamanho, sizeof(int), 1, arquivobin);
    fwrite(((reg->nomeJogador).string), sizeof(char), (reg->nomeJogador).tamanho , arquivobin);
    fwrite(&(reg->nacionalidade).tamanho, sizeof(int), 1, arquivobin);
    fwrite(((reg->nacionalidade).string), sizeof(char), (reg->nacionalidade).tamanho, arquivobin);
    fwrite(&(reg->nomeClube).tamanho, sizeof(int), 1, arquivobin);
    fwrite(((reg->nomeClube).string), sizeof(char), (reg->nomeClube).tamanho, arquivobin);

    //desalocar o registro
    free((reg->nomeJogador).string);
    free((reg->nacionalidade).string);
    free((reg->nomeClube).string);
    free(reg);    
}

cabecalho_ *cabecalho_inicial() {
    cabecalho_ *cabecalho = (cabecalho_ *)malloc(sizeof(cabecalho_));

    cabecalho->status = '0'; //aberto = 0, fechado = 1
    cabecalho->topo = -1; //aponta para o primeiro removido da lista de removidos
    cabecalho->proxByteOffset = 25; //prox byte offset
    cabecalho->nroRegArq = 0; //armazena a quantidade de registros no arquivo
    cabecalho->nroRegRem = 0; //armazena a quantidade de registros armazenados

    return cabecalho;
}

void inserirEspaco(char *str) {
    int tam = strlen(str);
    int espacos = 0;

    //conta quantos espacos são necessários
    for(int i = 0; i < tam - 1; i++) {
        if(str[i] == ',' && str[i+1] == ',') {
            espacos++;
        }
    }

    //se nao e necessario adicionar espacos, retorna
    if(espacos == 0) {
        return;
    }

    //cria uma nova string com a quantidade de caracteres necessaria para adicionar os novos espacos
    char *novastring = (char *) malloc(tam + espacos + 1);
    if(novastring == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //copia a str para a novastring, adicionando os espacos
    int novo_index = 0;
    for (int i = 0; i < tam; i++) {
        novastring[novo_index++] = str[i];
        if((str[i] == ',') && (i < tam - 1) && (str[i+1] == ',')) {
            novastring[novo_index++] = ' ';
        }
    }
    //colocar o '\0' no fim da string
    novastring[novo_index] = '\0';

    //copia a nova string para str e da free nela
    strcpy(str, novastring);
   
    free(novastring);
}

void transferirDados_CSV_BINARIO(char *nomeArquivo, char *nomeArquivoBin) {
    //abre o arquivo csv
    FILE *arquivoler = abrirArquivoCSV_leitura(nomeArquivo);
    if(arquivoler == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //abre/cria o arquivo binario
    FILE *arquivobin = abrirArquivoBinario_escrita_e_leitura(nomeArquivoBin);   
    if(arquivobin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    char linha[1024];
    //comando para ignorar a primeira linha do csv
    fgets(linha, 1024, arquivoler);

    cabecalho_ *cabecalho = cabecalho_inicial();

    while(fgets(linha, 1024, arquivoler) != NULL) {
        inserirEspaco(linha);

        //criar e alocar o registro do jogador
        registro_dados *reg = (registro_dados *) malloc(sizeof(registro_dados));
        if(reg == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return;
        }

        strcpy(reg->removido, "0");

        //ler o campo id do registro
        reg->id = atoi(strtok(linha, ","));

        //ler a idade do registro
        //se a idade nao existir, idade será "-1"
        char *idade_tmp = strtok(NULL, ",");
        if(strcmp(idade_tmp, " ") == 0) {
            reg->idade = -1;
        } else {
            reg->idade = atoi(idade_tmp);
        }

        //alocar o nomeJogador
        char *nomeJogador_tmp = strtok(NULL, ",");
        //se o nomeJogador nao existir, ira alocar uma string de tamanho 1 e colocar '\0' nesse local, para preencher o binario depois
        //o tamanho da string será 0
        if(strcmp(nomeJogador_tmp, " ") == 0) {
            reg->nomeJogador.tamanho = 0;
            (reg->nomeJogador).string = (char *) malloc(strlen(nomeJogador_tmp));
            reg->nomeJogador.string[0] = '\0';
            //se existir, aloca e coloca no registro, alem de definir o tamanho dele
        } else {
            (reg->nomeJogador).string = (char *) malloc(strlen(nomeJogador_tmp) + 1);
            strcpy((reg->nomeJogador).string, nomeJogador_tmp);
            (reg->nomeJogador).tamanho = strlen(nomeJogador_tmp);
        }


        //ler a nacionalidade
        char *nacionalidade_tmp = strtok(NULL, ",");
        //se a nacionalidade nao existir, ira alocar uma string de tamanho 1 e colocar '\0' nesse local, para preencher o binario depois
        //o tamanho da string será 0
        if(strcmp(nacionalidade_tmp, " ") == 0) {
            reg->nacionalidade.tamanho = 0;
            (reg->nacionalidade).string = (char *) malloc(strlen(nacionalidade_tmp));
            reg->nacionalidade.string[0] = '\0';
        //se existir, aloca e coloca no registro, alem de definir o tamanho dela
        } else {
            (reg->nacionalidade).string = (char *) malloc(strlen(nacionalidade_tmp) + 1);
            strcpy((reg->nacionalidade).string, nacionalidade_tmp);
            (reg->nacionalidade).tamanho = strlen((reg->nacionalidade).string);
        }

        //le o nomeClube
        char *nomeClube_tmp = strtok(NULL, ",");
        //se nomeClube for vazio, ele irá ler apenas um \n
        //assim, trocamos o \n por \0 para preencher a string depois
        if(nomeClube_tmp[strlen(nomeClube_tmp) - 1] == '\n' || nomeClube_tmp[strlen(nomeClube_tmp) - 1] == '\r') {
            nomeClube_tmp[0] = '\0';
        }
        
        //aloca e coloca o nomeClube no registro
        (reg->nomeClube).string = (char *) malloc(strlen(nomeClube_tmp) + 1);
        strcpy((reg->nomeClube).string, nomeClube_tmp);
        (reg->nomeClube).tamanho = strlen(nomeClube_tmp); //tamanho sera 0

        //determina o tamanho do registro (baseado no que foi lido) e define o proximo como -1
        reg->tamanhoRegistro = sizeof(reg->removido) + sizeof(int) + sizeof(long int) +  sizeof(int) + sizeof(int) + sizeof(int) + strlen(reg->nomeJogador.string) + sizeof(int) + strlen(reg->nacionalidade.string) + sizeof(int) + strlen(reg->nomeClube.string);
        reg->prox = -1;
        
        //atualizar o proxByteOffset e o nroRegArq do cabecalho
        cabecalho->proxByteOffset += reg->tamanhoRegistro;
        cabecalho->nroRegArq++; 

        //escreve o registro no arquivo binario e desaloca o registro
        escreverBinario(arquivobin, reg);
    }

    //atualiza o cabecalho do arquivo binario
    cabecalho->status = '1';
    fseek(arquivobin, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, arquivobin);
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivobin);
    fwrite(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivobin);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivobin);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivobin);
    
    fclose(arquivoler);
    fclose(arquivobin);
}