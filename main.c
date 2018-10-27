#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct _PGM {
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGM;

int **allocateMatrix(int row, int col)
{
    int **val,i;
    val = (int **)malloc(sizeof(int *) * row);
    if (val == NULL) {
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < row; ++i) {
        val[i] = (int *)malloc(sizeof(int) * col);
        if (val[i] == NULL) {
            exit(EXIT_FAILURE);
        }
    }
    return val;
}
void deallocateMatrix(int **matrix, int row)
{
    int i;
    for (i = 0; i < row; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(){
	int i,j,sayac;
	PGM data;
	
	// Pgm Dosyasýnýn Okunmasý
	FILE *pgmFile;
    char version[4];
	pgmFile = fopen("pentagone.1024.pgm", "rb");
	if (pgmFile == NULL) {
        perror("cannot open file to read");
        exit(EXIT_FAILURE);
    }
    fscanf(pgmFile, "%s", version);
    fscanf(pgmFile, "%d", &data.col);
    fscanf(pgmFile, "%d", &data.row);
    fscanf(pgmFile, "%d", &data.max_gray);
    fgetc(pgmFile);
    data.matrix = allocateMatrix(data.row, data.col);
        for (i = 0; i < data.row; ++i) {
            for (j = 0; j < data.col; ++j) {
                data.matrix[i][j] = fgetc(pgmFile);
            }
        }
    fclose(pgmFile);

	// Gauss Filtresinin Uygulanmasý
	int sum,p,q;
	int **gauss_image;
	gauss_image = allocateMatrix(data.row-4, data.col-4);
	int gaussian_kernel[5][5]=
	{
    {1, 4, 7, 4, 1},
    {4, 16, 26, 16, 4},
    {7, 26 , 41 ,26, 7},
    {4, 16, 26, 16, 4},
    {1, 4, 7, 4, 1}
	};
	
		// Konvolüsyon Ýþlemi	
	for (i = 2; i < data.row - 2; i++){
	    for (j = 2; j < data.col - 2; j++){
		sum = 0;
		for (p = -2; p < 3 ; p++){
		    for (q = -2; q < 3; q++){
			sum = sum + ((data.matrix[i+p][j+q])*(gaussian_kernel[p+2][q+2]));
	        }
		}
		sum = sum / 273;
		gauss_image[i-2][j-2]=sum;
	    }
	}
	
		// Max-Min Bulma
	int min = 9999,max = -9999;

	for(i=0;i<data.row-4;i++){
		for(j=0;j<data.col-4;j++){
				if(gauss_image[i][j]<min){
					min = gauss_image[i][j];
				}
				if(gauss_image[i][j]>max){
					max = gauss_image[i][j];
				}
		}
	}	
	
		// Normalizasyon
	for(i=0;i<data.row-4;i++){
		for(j=0;j<data.col-4;j++){
				gauss_image[i][j] = ((gauss_image[i][j]-min)*255) / (max-min);
		}
	}
	
	// Sobel Filtresinin Uygulandýktan Sonra Dosyaya Yazýlmasý 
	FILE *pgmWrite;
    pgmWrite = fopen("gauss.pgm", "wb");
    if (pgmWrite == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }
    fprintf(pgmWrite, "P5 ");
    fprintf(pgmWrite, "%d %d ", data.col-4, data.row-4);
    fprintf(pgmWrite, "%d ", data.max_gray);
        for (i = 0; i < data.row-4; ++i) {
            for (j = 0; j < data.col-4; ++j) {
                fputc(gauss_image[i][j], pgmWrite);
            }
        }
        fclose(pgmWrite);
	
	
	
	// Sobel Filtresinin Uygulanmasý
	int **sobelx_image;
	int **sobely_image;
	int **sobel_image;
	sobelx_image = allocateMatrix(data.row-6, data.col-6);
	sobely_image = allocateMatrix(data.row-6, data.col-6);
	sobel_image = allocateMatrix(data.row-6, data.col-6);
    int sobel[3][3]=
	{
    {-1,0,1},
    {-2,0,2},
    {-1,0,1}
	};
	int sobely[3][3]=
	{
    {-1,0,-1},
    {-2,0,2},
    {-1,0,1}
	};	
		// Konvolüsyon Ýþlemi
	for (i = 1; i < data.row - 5; i++){
	    for (j = 1; j < data.col - 5; j++){
		sum = 0;
		for (p = -1; p < 2; p++){
		    for (q = -1; q < 2; q++){
			sum = sum + gauss_image[i+p][j+q]*sobel[p+1][q+1];
	        }
		}
		if(sum<0){
			sum = 0;
		}
		sobelx_image[i-1][j-1]=sum;
	    }
	}
	
	for (i = 1; i < data.row - 5; i++){
	    for (j = 1; j < data.col - 5; j++){
		sum = 0;
		for (p = -1; p < 2; p++){
		    for (q = -1; q < 2; q++){
			sum = sum + gauss_image[i+p][j+q]*sobel[p+1][q+1];
	        }
		}
		if(sum<0){
			sum = 0;
		}
		sobely_image[i-1][j-1]=sum;
	    }
	}
	
	float fl;
	int intvalue;
	
	for(i=0;i<data.row-6;i++){
		for(j=0;j<data.col-6;j++){
			fl = sqrt(pow(sobelx_image[i][j],2) + pow(sobely_image[i][j],2));
			intvalue = round(fl);
			sobel_image[i][j]=intvalue;			
		}
	}	
	
	
		// Max-Min Bulma
	min = 9999;
	max = -9999;

	for(i=0;i<data.row-6;i++){
		for(j=0;j<data.col-6;j++){
				if(sobel_image[i][j]<min){
					min = sobel_image[i][j];
				}
				if(sobel_image[i][j]>max){
					max = sobel_image[i][j];
				}
		}
	}	
		// Normalizasyon
	for(i=0;i<data.row-6;i++){
		for(j=0;j<data.col-6;j++){	
				sobel_image[i][j] = ((sobel_image[i][j]-min)*255) / (max-min);	
				
		}
	}
	
	
	
	// Sobel Filtresinin Uygulandýktan Sonra Dosyaya Yazýlmasý 
	FILE *pgmSobel;
    pgmSobel = fopen("sobel.pgm", "wb");
    if (pgmSobel == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }
    fprintf(pgmSobel, "P5 ");
    fprintf(pgmSobel, "%d %d ", data.col-6, data.row-6);
    fprintf(pgmSobel, "%d ", data.max_gray);
        for (i = 0; i < data.row-6; ++i) {
            for (j = 0; j < data.col-6; ++j) {
                fputc(sobel_image[i][j], pgmSobel);
            }
        }
    fclose(pgmSobel);
    
    // Laplacian Filtresinin Uygulanmasý
    // Fazladan bir matrix daha oluþturmak yerine daha önceden oluþturulmuþ gauss_image matrixi kullanýldý.
    
    /*int laplacian[5][5]=
	{
	{0,0,-1,0,0},
   	{0,-1,-2,-1,0},
   	{-1,-2,16,-2,-1},
   	{0,-1,-2,-1,0},
   	{0,0,-1,0,0}
	};*/
	
	int laplacian[3][3]=
	{
	{0,1,0},
   	{1,-4,1},
   	{0,1,0}
	};
	
	int **laplacian_image;
	laplacian_image = allocateMatrix(data.row-2, data.col-2);
	
	 	// Konvolüsyon Ýþlemi
	for (i = 1; i < data.row - 1; i++){
	    for (j = 1; j < data.col - 1; j++){
		sum = 0;
		for (p = -1; p < 2; p++){
		    for (q = -1; q < 2; q++){
			sum = sum + data.matrix[i+p][j+q]*laplacian[p+1][q+1];
	        }
		}
		if(sum<0){
			sum = 0;
		}
		laplacian_image[i-1][j-1]=sum;
	    }
	}
	
		// Max Min Bulma
	min = 9999;
	max = -9999;
	for(i=0;i<data.row-2;i++){
		for(j=0;j<data.col-2;j++){
				if(laplacian_image[i][j]<min){
					min = laplacian_image[i][j];
				}
				if(laplacian_image[i][j]>max){
					max = laplacian_image[i][j];
				}
		}
	}	
	
		// Normalizasyon
	for(i=0;i<data.row-2;i++){
		for(j=0;j<data.col-2;j++){
				
				laplacian_image[i][j] = ((laplacian_image[i][j]-min)*255) / (max-min);
			
		}
	}
	
	// Laplacian Filtresinin Uygulandýktan Sonra Dosyaya Yazýlmasý
	FILE *pgmLaplacian;
    pgmLaplacian = fopen("laplacian.pgm", "wb");
    if (pgmLaplacian == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }
    fprintf(pgmLaplacian, "P5 ");
    fprintf(pgmLaplacian, "%d %d ", data.col-2, data.row-2);
    fprintf(pgmLaplacian, "%d ", data.max_gray);
        for (i = 0; i < data.row-2; ++i) {
            for (j = 0; j < data.col-2; ++j) {
                fputc(laplacian_image[i][j], pgmLaplacian);
            }
        }
        fclose(pgmLaplacian);
        deallocateMatrix(gauss_image,data.row-4);
        deallocateMatrix(sobel_image,data.row-6);
    }
    
