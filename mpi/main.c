#include <stdio.h>
#include <stdlib.h>
#pragma pack(1)

typedef struct cabecalho {
	unsigned short tipo;
	unsigned int tamanho_arquivo;
	unsigned short reservado1;
	unsigned short reservado2;
	unsigned int offset;
	unsigned int tamanho_cabecalho;
	int largura;
	int altura;
	unsigned short planos;
	unsigned short bits;
	unsigned int compressao;
	unsigned int tamanho_imagem;
	int xresolucao;
	int yresolucao;
	unsigned int cores_usadas;
	unsigned int cores_significantes;
} CABECALHO;

typedef struct rgb{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} RGB;

/*
MAIN ---------------------------------------------------------------------
*/
int main(int argc, char **argv ){
    FILE *fileIn, *fileOut;

    argc = 4;
    argv[1] = "c:\\temp\\borboletanova.bmp";
    argv[2] = "c:\\temp\\lixo.bmp";
    argv[3] = "3";

    //Testa o numero de argumentos
    if (argc != 4) {
		printf("%s <img_entrada> <img_saida> <mascara> \n", argv[0]);
		exit(0);
	}

    //Abre o arquivo de input
	printf("ABRINDO ARQUIVO DE ENTRADA...\n");
	fileIn = fopen(argv[1], "rb");
	if (fileIn == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
		exit(0);
	}
    printf("ABRIU ARQUIVO DE ENTRADA\n");

    printf("ABRINDO ARQUIVO DE SAIDA...\n");
	//Abre o arquivo de output
	fileOut = fopen(argv[2], "wb");
	if (fileOut == NULL){
		printf("Erro ao abrir o arquivo %s\n", argv[2]);
		exit(0);
	}
	printf("ABRIU ARQUIVO DE SAIDA\n");









    //Fecha o arquivo de input
	printf("FECHANDO ARQUIVO DE ENTRADA...\n");
	fclose(fileIn);
    printf("FECHOU ARQUIVO DE ENTRADA\n");

    //Fecha o arquivo de output
    printf("FECHANDO ARQUIVO DE SAIDA...\n");
	fclose(fileOut);
	printf("FECHOU ARQUIVO DE SAIDA\n");
}
