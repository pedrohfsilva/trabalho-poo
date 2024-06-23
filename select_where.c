/*
        NOME              NUSP
Pedro Louro Fernandes - 13672446
Letícia Barbosa Neves - 14588659
*/


#include "funcoes_gerais.h"
#include "select_where.h"

//função para imprimir um registro
void imprimirRegistro(registro_dados* reg) {
    if (reg->nomeJogador.tamanho == 0) {
        printf("Nome do Jogador: SEM DADO\n"); 
    } else {
        printf("Nome do Jogador: %s\n", reg->nomeJogador.string);
    }

    if (reg->nacionalidade.tamanho == 0) {
        printf("Nacionalidade do Jogador: SEM DADO\n");
    } else {
        printf("Nacionalidade do Jogador: %s\n", reg->nacionalidade.string);
    }

    if (reg->nomeClube.tamanho == 0) {
        printf("Clube do Jogador: SEM DADO\n\n");
    } else {
        printf("Clube do Jogador: %s\n\n", reg->nomeClube.string);
    }
}

//função principal para fazer a busca dos registros com os critérios necessários e imprimir com a formatação solicitada
void leitura_buscaBinario(int m, criterio_busca* criterio, char* nomeArquivoBin) {
    FILE* arquivobin = abrirArquivoBinario_leitura(nomeArquivoBin);
    if (arquivobin == NULL) return;

    cabecalho_* cabecalho = lerCabecalho(arquivobin);
    if (cabecalho == NULL) {
        fclose(arquivobin);
        return;
    }

    long int byteoffset_pularegistro = 25; //inicia em 25 pois é o tamanho do cabeçalho
    int registrosimpressos = 0;
    bool encontrado = false;

    while (1) {
        registro_dados* reg = lerReg(arquivobin);
        if (reg == NULL) break;

        byteoffset_pularegistro += reg->tamanhoRegistro;

        if (strcmp(reg->removido, "1") == 0) {
            liberarRegistro(reg);
            fseek(arquivobin, byteoffset_pularegistro, SEEK_SET);
            continue;
        }

        encontrado = verificarCriterio(reg, &criterio[0]);
        for (int i = 1; i < m && encontrado; i++) {
            encontrado = verificarCriterio(reg, &criterio[i]);
        }

        if (encontrado) {
            registrosimpressos++;
            imprimirRegistro(reg);
        }

        liberarRegistro(reg);
    }

    if (registrosimpressos == 0) {
        printf("Registro inexistente.\n\n");
    }

    free(cabecalho);
    fclose(arquivobin);
}
