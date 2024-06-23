/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/

#include "funcoes_gerais.h"

//funçao para abrir o arquivo csv para realizar a leitura
FILE *abrirArquivoCSV_leitura(char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");

    return arquivo;
}

//funçao para abrir o arquivo binario para escrita e leitura
FILE *abrirArquivoBinario_escrita_e_leitura(char* nomeArquivo) {
    //se o arquivo já existir, abre o arquivo para ler e escrever em binario
    FILE* arquivo = fopen(nomeArquivo, "rb+");

    cabecalho_ *cabecalho = (cabecalho_ *)malloc(sizeof(cabecalho_));

    cabecalho->status = '0'; //aberto = 0, fechado = 1
    cabecalho->topo = -1;
    cabecalho->proxByteOffset = 25; //prox byte offset
    cabecalho->nroRegArq = 0; //armazena a quantidade de registros no arquivo
    cabecalho->nroRegRem = 0; //quantidade de registros removidos

    if(arquivo == NULL) {
        //se o arquivo não existir, cria o arquivo
        arquivo = fopen(nomeArquivo, "wb+");

        //escreve o cabeçalho no começo do arquivo
        fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
        fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo);
        fwrite(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivo);
        fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
        fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    } else {
        //para fechar o arquivo, o status do cabeçalho é alterado para fechado (1)
        fseek(arquivo, 0, SEEK_SET);
        fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    }

    free(cabecalho);

    return arquivo;
}

//funçao para abrir o arquivo binario para leitura
FILE *abrirArquivoBinario_leitura(char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "rb");

    return arquivo;
}

//funçao para abrir o arquivo index para escrita e leitura
FILE *abrirArquivoIndex_escrita_e_leitura(char *nomeArquivo) {
    //abre para leitura e escrita
    FILE *arquivo = fopen(nomeArquivo, "w+b");
    
    char status = '0';

    if(arquivo == NULL) {
        //se o arquivo não existir escreve o cabeçalho no começo do arquivo
        fwrite(&status, sizeof(char), 1, arquivo);

    } else {
        //para fechar o arquivo, o status do cabeçalho é alterado para fechado (1)
        fseek(arquivo, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, arquivo);
    }

    return arquivo;
}

//funçao para abrir o arquivo index para leitura
FILE *abrirArquivoIndex_leitura(char *nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "rb");

    return arquivo;
}

//verifica o status do arquivo
int verificarConfiabilidade(FILE *arquivoDados){
    char statusDados;
    fseek(arquivoDados, 0, SEEK_SET);
    fread(&statusDados, sizeof(char), 1, arquivoDados);
    
    //se for 0, significa que o arquivo esta inconsistente
    //se for 1, esta consistente
    if(statusDados == '0'){
        return 0;
    }

    return 1;
}

//função para ler o cabeçalho do arquivo binário
cabecalho_ *lerCabecalho(FILE* arquivobin) {
    //cria e aloca o cabecalho
    cabecalho_* cabecalho = (cabecalho_*)malloc(sizeof(cabecalho_));
    if (cabecalho == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    //vai para o inicio do arquivo, le e armazena todos os dados do cabecalho do arquivo
    fseek(arquivobin, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, arquivobin);
    fread(&cabecalho->topo, sizeof(long int), 1, arquivobin);
    fread(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivobin);
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivobin);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivobin);

    //retorna o cabecalho
    return cabecalho;
}


//função para ler um registro do arquivo binário
registro_dados* lerReg(FILE* arquivobin) {
    //cria e aloca o registro
    registro_dados* reg = (registro_dados*)malloc(sizeof(registro_dados));
    if (reg == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    //verifica se esse registro existe no arquivo
    //se nao existir, libera o registro e retorna NULL
    if (fread(&reg->removido, sizeof(char), 1, arquivobin) < 1) {
        free(reg);
        return NULL;
    }
    reg->removido[1] = '\0';

    //le os outros dados do registro
    fread(&reg->tamanhoRegistro, sizeof(int), 1, arquivobin);
    fread(&reg->prox, sizeof(long int), 1, arquivobin);
    fread(&reg->id, sizeof(int), 1, arquivobin);
    fread(&reg->idade, sizeof(int), 1, arquivobin);

    //inicializa as strings como nulas
    reg->nomeJogador.string = NULL;
    reg->nacionalidade.string = NULL;
    reg->nomeClube.string = NULL;

    //le, aloca e armazena o nome do jogador
    fread(&reg->nomeJogador.tamanho, sizeof(int), 1, arquivobin);
    if (reg->nomeJogador.tamanho > 0) {
        reg->nomeJogador.string = (char*)malloc(reg->nomeJogador.tamanho + 1);
        fread(reg->nomeJogador.string, reg->nomeJogador.tamanho, 1, arquivobin);
        reg->nomeJogador.string[reg->nomeJogador.tamanho] = '\0';
    }

    //le, aloca e armazena a nacionalidade
    fread(&reg->nacionalidade.tamanho, sizeof(int), 1, arquivobin);
    if (reg->nacionalidade.tamanho > 0) {
        reg->nacionalidade.string = (char*)malloc(reg->nacionalidade.tamanho + 1);
        fread(reg->nacionalidade.string, reg->nacionalidade.tamanho, 1, arquivobin);
        reg->nacionalidade.string[reg->nacionalidade.tamanho] = '\0';
    }

    //le, aloca e armazena o nome do clube
    fread(&reg->nomeClube.tamanho, sizeof(int), 1, arquivobin);
    if (reg->nomeClube.tamanho > 0) {
        reg->nomeClube.string = (char*)malloc(reg->nomeClube.tamanho + 1);
        fread(reg->nomeClube.string, reg->nomeClube.tamanho, 1, arquivobin);
        reg->nomeClube.string[reg->nomeClube.tamanho] = '\0';
    }

    //retorna o registro
    return reg;
}

//função para liberar a memória de um registro
void liberarRegistro(registro_dados* reg) {
    if (reg->nomeJogador.string != NULL) free(reg->nomeJogador.string);
    if (reg->nacionalidade.string != NULL) free(reg->nacionalidade.string);
    if (reg->nomeClube.string != NULL) free(reg->nomeClube.string);
    free(reg);
}

//função para verificar se o registro atende ao critério de busca
bool verificarCriterio(registro_dados* reg, criterio_busca* criterio) {
    //verifica qual o tipo do criterio buscado e retorna se o criterio é igual ao registro ou nao (true or false) 
    if (strcmp("id", criterio->tipo_buscado) == 0) {
        return reg->id == atoi(criterio->buscado);
    } else if (strcmp("idade", criterio->tipo_buscado) == 0) {
        return reg->idade == atoi(criterio->buscado);
    } else if (strcmp("nomeJogador", criterio->tipo_buscado) == 0) {
        return reg->nomeJogador.tamanho > 0 && strcmp(criterio->buscado, reg->nomeJogador.string) == 0;
    } else if (strcmp("nacionalidade", criterio->tipo_buscado) == 0) {
        return reg->nacionalidade.tamanho > 0 && strcmp(criterio->buscado, reg->nacionalidade.string) == 0;
    } else if (strcmp("nomeClube", criterio->tipo_buscado) == 0) {
        return reg->nomeClube.tamanho > 0 && strcmp(criterio->buscado, reg->nomeClube.string) == 0;
    }
    return false;
}

//função para atualizar o cabeçalho com os valores atualizados
void atualizarCabecalho(FILE *arquivoDados, cabecalho_ *cabecalho){
    //vai para o inicio do cabecalho (pulando o status pois sera atualizado so no final, antes do fechamento do arquivo)
    fseek(arquivoDados, sizeof(char), SEEK_SET);

    //escreve todos os campos do cabecalho atualizados no arquivo binario
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivoDados);
    fwrite(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivoDados);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivoDados);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivoDados);
    
    //por fim, vai para o campo "status" e escreve ele como '1', pois o arquivo binario sera fechado em seguida
    cabecalho->status = '1';
    fseek(arquivoDados, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, arquivoDados);
}