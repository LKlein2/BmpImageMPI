#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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

void lerArquivo(RGB* img, CABECALHO header, FILE *file)
{
    int alinhamento;
    unsigned char byte;

	for(int i = 0; i < header.altura; i++){
		alinhamento = (header.largura * 3) % 4;

		if (alinhamento != 0){
			alinhamento = 4 - alinhamento;
		}

		for(int j = 0; j < header.largura; j++){
			fread(&img[i * header.largura + j], sizeof(RGB), 1, file);
		}

		for(int j = 0; j < alinhamento; j++){
			fread(&byte, sizeof(unsigned char), 1, file);
		}
	}
}

void escreverArquivo(RGB* img, CABECALHO header, FILE *file)
{
    int alinhamento;
    unsigned char byte;

	for(int i = 0; i < header.altura; i++){
		alinhamento = (header.largura * 3) % 4;

		if (alinhamento != 0){
			alinhamento = 4 - alinhamento;
		}

		for(int j = 0; j < header.largura; j++){
			fwrite(&img[i * header.largura + j], sizeof(RGB), 1, file);
		}

		for(int j = 0; j < alinhamento; j++){
			fwrite(&byte, sizeof(unsigned char), 1, file);
		}
	}
}

int * ordenarVetor(int* array, int size) {
	for(int i = 0 ; i < size ; i++) {
		for(int j = 0 ; j < size - 1 ; j++) {
			if(array[j] > array[j + 1])
			{
				int temp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
		}
	}
	return array;
}

/*
MAIN ---------------------------------------------------------------------
*/
int main(int argc, char **argv ){
    FILE *fileIn, *fileOut;
    CABECALHO header;
    RGB *imgIn, *imgOut, *imgInAux, *imgOutAux;
    int mascara;

	int numProcessos, idProcesso;
	double tempoInicial, tempoFinal;

	/*
    argc = 4;
    argv[1] = "c:\\temp\\borboleta.bmp";
    argv[2] = "c:\\temp\\lixo.bmp";
    argv[3] = "7";
	*/

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &idProcesso);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessos);

    if (argc != 4) {
		printf("%s <img_entrada> <img_saida> <mascara> \n", argv[0]);
		exit(0);
	}

	fileIn = fopen(argv[1], "rb");
	if (fileIn == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
		exit(0);
	}

	fileOut = fopen(argv[2], "wb");
	if (fileOut == NULL){
		printf("Erro ao abrir o arquivo %s\n", argv[2]);
		exit(0);
	}

	mascara = atoi(argv[3]);
    if (mascara != 3 && mascara != 5 && mascara != 7) {
        printf("Mascara de %d não suportada!\n", mascara);
		exit(0);
    }

	fread(&header, sizeof(CABECALHO), 1, fileIn);
	fwrite(&header, sizeof(CABECALHO), 1, fileOut);

    imgIn  	  = (RGB *)malloc(header.altura * header.largura * sizeof(RGB));
	imgInAux  = (RGB *)malloc((header.altura / numProcessos) * header.largura * sizeof(RGB));
    imgOut 	  = (RGB *)malloc(header.altura * header.largura * sizeof(RGB));
	imgOutAux = (RGB *)malloc(header.altura * header.largura * sizeof(RGB));

	if (idProcesso == 0) {
		lerArquivo(imgIn, header, fileIn);
	}

	tempoInicial = MPI_Wtime();

	MPI_Bcast(	imgOut, 
			  	(header.altura * header.largura)*sizeof(RGB),
			  	MPI_BYTE, 
			  	0, 
			  	MPI_COMM_WORLD);

	MPI_Scatter(imgIn,
				(header.altura / numProcessos) * header.largura * sizeof(RGB), 
				MPI_BYTE,
				imgInAux, 
				(header.altura / numProcessos) * header.largura * sizeof(RGB), 
				MPI_BYTE, 
				0, 
				MPI_COMM_WORLD);
	
	int sizeOfArray = (mascara * mascara);
	int* arrayR = (int*)malloc(sizeOfArray * sizeof(int));
	int* arrayG = (int*)malloc(sizeOfArray * sizeof(int));
	int* arrayB = (int*)malloc(sizeOfArray * sizeof(int));

	//Calcula o range
	//Para mascara de 3: -1 até 1 então range = 1
	//Para mascara de 5: -2 até 2 então range = 2
	//Para mascara de 7: -3 até 3 então range = 3
	int range = (mascara - 1) / 2;
	int mediana = sizeOfArray / 2;	

	for (int i = 0; i < header.altura / numProcessos; i++)
	{
		for (int j = 0; j < header.largura; j++)
		{
			int posicaoArray = 0;

			//Range eixo X
			for (int k = -range; k <= range; k++)
			{
				int posAltura = i + k;
				
				//Range eixo Y
				for (int l = -range; l <= range; l++)
				{
					int posLargura = j + l;

					if (posAltura < 0 || posLargura < 0 || posAltura >= header.altura || posLargura >= header.largura) {
						arrayR[posicaoArray] = 0;
						arrayG[posicaoArray] = 0;
						arrayB[posicaoArray] = 0;
						posicaoArray++;
					} else {
						arrayR[posicaoArray] = imgInAux[posAltura * header.largura + posLargura].red;
						arrayG[posicaoArray] = imgInAux[posAltura * header.largura + posLargura].green;
						arrayB[posicaoArray] = imgInAux[posAltura * header.largura + posLargura].blue;
						posicaoArray++;
					}
				}				
			}		

			imgOut[i * header.largura + j].red   = ordenarVetor(arrayR, sizeOfArray)[mediana];
			imgOut[i * header.largura + j].green = ordenarVetor(arrayG, sizeOfArray)[mediana];
			imgOut[i * header.largura + j].blue  = ordenarVetor(arrayB, sizeOfArray)[mediana];
		}
	}

	free(arrayR);
	free(arrayG);
	free(arrayB);

	MPI_Gather( imgOut,
				header.altura / numProcessos * header.largura * sizeof(RGB), 
				MPI_BYTE,
				imgOutAux, 
				header.altura / numProcessos * header.largura * sizeof(RGB), 
				MPI_BYTE, 
				0, 
				MPI_COMM_WORLD);

	if (idProcesso == 0) {
		escreverArquivo(imgOutAux, header, fileOut);
	}

	fclose(fileIn);
	fclose(fileOut);

	tempoFinal = MPI_Wtime();
	if (idProcesso == 0) {
		printf("\nTEMPO DE EXECUÇÃO: %f\n", tempoFinal - tempoInicial);
	}
	MPI_Finalize();
}
