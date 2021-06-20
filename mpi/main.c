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
    CABECALHO header;
    RGB **imgIn, **imgOut;
    int mascara;

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
	printf("\nABRINDO ARQUIVO DE ENTRADA...\n");
	fileIn = fopen(argv[1], "rb");
	if (fileIn == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
		exit(0);
	}
    printf("ABRIU ARQUIVO DE ENTRADA\n");

	//Abre o arquivo de output
    printf("\nABRINDO ARQUIVO DE SAIDA...\n");
	fileOut = fopen(argv[2], "wb");
	if (fileOut == NULL){
		printf("Erro ao abrir o arquivo %s\n", argv[2]);
		exit(0);
	}
	printf("ABRIU ARQUIVO DE SAIDA\n");

    //Verifica tamanho da mascara
	mascara = atoi(argv[3]);
    if (mascara != 3 && mascara != 5 && mascara != 7) {
        printf("Mascara de %d não suportada!\n", mascara);
		exit(0);
    }

    //Lê o cabeçalho do arquivo de entrada
    printf("\nLENDO ARQUIVO DE ENTRADA...\n");
	fread(&header, sizeof(CABECALHO), 1, fileIn);
	printf("LEU ARQUIVO DE ENTRADA\n");

	//escreve o cabeçalho do arquivo de saída
	printf("\nESCREVENDO ARQUIVO DE SAIDA...\n");
	fwrite(&header, sizeof(CABECALHO), 1, fileOut);
    printf("ESCREVEU ARQUIVO DE SAIDA\n");

    printf("\n");
    printf("Tamanho do arquivo: %u\n", header.tamanho_arquivo);
    printf("Offset: %d\n", header.offset);
    printf("Largura: %d\n", header.largura);
    printf("Altura: %d\n", header.altura);
    printf("Bits: %d\n", header.bits);
    printf("Tamanho da imagem: %u\n", header.tamanho_imagem);
    printf("\n");

    printf("\nALOCANDO MEMORIA PARA ESTRUTURAS...\n");
    //Aloca o espaço para os pixels da imagem
    imgIn = (RGB **)malloc(header.altura * sizeof(RGB *));
    imgOut   = (RGB **)malloc(header.altura * sizeof(RGB *));
    for(int i = 0 ; i < header.altura ; i++) {
		imgIn[i]  = (RGB *)malloc(header.largura * sizeof(RGB));
		imgOut[i] = (RGB *)malloc(header.largura * sizeof(RGB));
	}
	printf("ALOCOU MEMORIA PARA ESTRUTURAS\n");

    //Fecha o arquivo de input
	printf("\nFECHANDO ARQUIVO DE ENTRADA...\n");
	fclose(fileIn);
    printf("FECHOU ARQUIVO DE ENTRADA\n");

    //Fecha o arquivo de output
    printf("\nFECHANDO ARQUIVO DE SAIDA...\n");
	fclose(fileOut);
	printf("FECHOU ARQUIVO DE SAIDA\n");
}
