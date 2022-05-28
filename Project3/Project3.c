#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>


//비트맵 헤더를 한묶음으로
typedef struct tagBITMAPHEADER {
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	RGBQUAD hRGB[256]; //이 코드에서는 필요없음 (8bit에만 필요)
}BITMAPHEADER;

//비트맵을 읽어와서 화소정보의 포인터를 리턴
BYTE* loadBitmapFile(int bytesPerPixel, BITMAPHEADER* bitmapHeader, int* imgWidth, int* imgHeight, char* filename);

//비트맵 파일 쓰기
void writeBitmapFile(int bytesPerPixel, BITMAPHEADER outputHeader, BYTE* output, int imgSize, char* filename);

int** kronecker(int** A, int B[][1], int aROW, int aCOL) {
	/*
	int aROW = sizeof(A) / sizeof(int*);
	int aCOL = sizeof(A[0]) / sizeof(int);
	int bROW = sizeof(B) / sizeof(int*);
	int bCOL = sizeof(B[0]) / sizeof(int);
	*/
	int bROW = 2;
	int bCOL = 1;

	int cROW = aROW * bROW;
	int cCOL = aCOL * bCOL;

	int i, j, l, k, stROW, stCOL;
	int** C = (int**)malloc(sizeof(int*) * cROW);
	for (int i = 0; i < cROW; i++) {
		C[i] = (int*)malloc(sizeof(int) * cCOL);
	}

	for (i = 0; i < aROW; i++) {
		for (j = 0; j < aCOL; j++) {
			stROW = i * bROW;
			stCOL = j * bCOL;
			for (k = 0; k < bROW; k++) {
				for (l = 0; l < bCOL; l++) {
					C[stROW + k][stCOL + l] = A[i][j] * B[k][l];
				}
			}
		}
	}
	return C;
}

int** identical(int n) {
	int** I = (int**)malloc(sizeof(int*) * n);
	for (int i = 0; i < n; i++) {
		I[i] = (int*)malloc(sizeof(int) * n);
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i == j)
				I[i][j] = 1;
			else
				I[i][j] = 0;
		}
	}
	return I;

}

int** haar(int n) {
	int** H = (int**)malloc(sizeof(int*) * n);
	for (int i = 0; i < n; i++) {
		H[i] = (int*)malloc(sizeof(int) * n);
	}


	if (n == 1) {
		H[0][0] = 1;
	}
	else {
		int m = n / 2;

		int** F = (int**)malloc(sizeof(int*) * n);
		for (int i = 0; i < n; i++) {
			F[i] = (int*)malloc(sizeof(int) * m);
		}
		int** S = (int**)malloc(sizeof(int*) * n);
		for (int i = 0; i < n; i++) {
			S[i] = (int*)malloc(sizeof(int) * m);
		}

		int fir[2][1], sec[2][1];
		fir[0][0] = 1;
		fir[1][0] = 1;
		sec[0][0] = 1;
		sec[1][0] = -1;
		F = kronecker(haar(m), fir, m, m);
		S = kronecker(identical(m), sec, m, m);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++)
			{
				H[i][j] = F[i][j];
			}
			for (int j = m; j < n; j++)
			{
				H[i][j] = S[i][j - m];
			}
		}
	}
	return H;
}
float** transposeMat(float** A, int m, int n) {
	float** T = (float**)malloc(sizeof(float*) * m);
	for (int i = 0; i < m; i++)
	{
		T[i] = (float*)malloc(sizeof(float) * n);
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			T[j][i] = A[i][j];
		}
	}
	return T;
}

float* noramlizeVec(float V[], int n) {
	float sum = 0;
	float* N = (float*)malloc(sizeof(float) * n);
	for (int i = 0; i < n; i++) {
		sum += V[i] * V[i];
	}
	float size = sqrt((double)sum);
	for (int i = 0; i < n; i++) {
		N[i] = V[i] / size;
	}
	return (float*)N;
}





int main() {
	/*******************************************************************/
	/*************************** Read image  ***************************/
	/*******************************************************************/
	BITMAPHEADER originalHeader;	//비트맵의 헤더부분을 파일에서 읽어 저장할 구조체
	BITMAPHEADER outputHeader;		//변형을 가한 헤더부분을 저장할 구조체
	int imgSize, imgWidth, imgHeight;					//이미지의 크기를 저장할 변수
	int bytesPerPixel = 3;			//number of bytes per pixel (1 byte for R,G,B respectively)

	BYTE* image = loadBitmapFile(bytesPerPixel, &originalHeader, &imgWidth, &imgHeight, "image_lena_24bit.bmp"); //비트맵파일을 읽어 화소정보를 저장 (불러들이는 이미지는 .c와 같은 폴더에 저장)
	if (image == NULL) return 0;

	imgSize = imgWidth * imgHeight; // total number of pixels
	BYTE* output = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);				//결과값을 저장할 포인터 선언 및 메모리 할당
	outputHeader = originalHeader;										//헤더정보를 출력헤더정보에 할당





	/*******************************************************************/
	/************************ Perform HWT/IHWT *************************/
	/*******************************************************************/
	//이미지 행렬 A 구성 (RGB값이 있으므로 픽셀당 값 하나씩만 읽어서 imgWidth x imgHeight 행렬 구성)

	int** A; //original image matrix
	A = (int**)malloc(sizeof(int*) * imgHeight);
	for (int i = 0; i < imgHeight; i++) {
		A[i] = (int*)malloc(sizeof(int) * imgWidth);
	}

	for (int i = 0; i < imgHeight; i++)
		for (int j = 0; j < imgWidth; j++)
			A[i][j] = image[(i * imgWidth + j) * bytesPerPixel];




	//Haar matrix H 구성 (orthonormal column을 갖도록 구성)
	int n = imgHeight; //이미지가 정사각형(Height==Width)이라고 가정; n = 2^t,t=0,1,2,...

	n = 8;

	int** _H = (int**)malloc(sizeof(int*) * n);
	for (int i = 0; i < n; i++) {
		_H[i] = (int*)malloc(sizeof(int) * n);
	}
	_H = haar(n);



	int** H = (float**)malloc(sizeof(float*) * n);
	for (int i = 0; i < n; i++) {
		H[i] = (float*)malloc(sizeof(float) * n);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			H[i][j] = (float)_H[j][i];
		}
	}


	for (int i = 0; i < n; i++) {
		H[i] = noramlizeVec(H[i], n);
	}
	H = transposeMat(H, n, n);
	printf("\nH 출력\n");
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%f ", H[i][j]);
		}
		printf("\n");
	}






	//...

	//HWT 수행: 행렬곱 B = H'*A*H
	//...

	//행렬 B 자르기: B의 upper left corner(subsquare matrix)를 잘라 Bhat에 저장
	//Bhat은 B와 사이즈가 같으며 B에서 잘라서 저장한 부분 외에는 모두 0으로 채워짐
	//...

	//IHWT 수행: Ahat = H*Bhat*H'
	//...





	/*******************************************************************/
	/******************* Write reconstructed image  ********************/
	/*******************************************************************/
	//Ahat을 이용해서 위의 image와 같은 형식이 되도록 구성 (즉, Ahat = [a b;c d]면 [a a a b b b c c c d d d]를 만들어야 함)
	BYTE* Are = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	//...

	writeBitmapFile(bytesPerPixel, outputHeader, Are, imgSize, "output1.bmp");


	free(image);
	free(output);
	for (int i = 0; i < imgHeight; i++)
		free(A[i]);
	free(A);
	free(Are);

	return 0;
}

BYTE* loadBitmapFile(int bytesPerPixel, BITMAPHEADER* bitmapHeader, int* imgWidth, int* imgHeight, char* filename)
{
	FILE* fp = fopen(filename, "rb");	//파일을 이진읽기모드로 열기
	if (fp == NULL)
	{
		printf("파일로딩에 실패했습니다.\n");	//fopen에 실패하면 NULL값을 리턴
		return NULL;
	}
	else
	{
		fread(&bitmapHeader->bf, sizeof(BITMAPFILEHEADER), 1, fp);	//비트맵파일헤더 읽기
		fread(&bitmapHeader->bi, sizeof(BITMAPINFOHEADER), 1, fp);	//비트맵인포헤더 읽기
		//fread(&bitmapHeader->hRGB, sizeof(RGBQUAD), 256, fp);	//색상팔렛트 읽기 (24bitmap 에서는 존재하지 않음)

		*imgWidth = bitmapHeader->bi.biWidth;
		*imgHeight = bitmapHeader->bi.biHeight;
		int imgSizeTemp = (*imgWidth) * (*imgHeight);	// 이미지 사이즈를 상위 변수에 할당

		printf("Size of image: Width %d   Height %d\n", bitmapHeader->bi.biWidth, bitmapHeader->bi.biHeight);
		BYTE* image = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSizeTemp);	//이미지크기만큼 메모리할당

		fread(image, bytesPerPixel * sizeof(BYTE), imgSizeTemp, fp);//이미지 크기만큼 파일에서 읽어오기

		fclose(fp);
		return image;
	}
}



void writeBitmapFile(int bytesPerPixel, BITMAPHEADER outputHeader, BYTE* output, int imgSize, char* filename)
{
	FILE* fp = fopen(filename, "wb");

	fwrite(&outputHeader.bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&outputHeader.bi, sizeof(BITMAPINFOHEADER), 1, fp);
	//fwrite(&outputHeader.hRGB, sizeof(RGBQUAD), 256, fp); //not needed for 24bitmap
	fwrite(output, bytesPerPixel * sizeof(BYTE), imgSize, fp);
	fclose(fp);
}






