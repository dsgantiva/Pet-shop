#include <stdio.h>
#include <stdlib.h>



//Define las constantes

#define DAT_SIZE 10 //# de datos a generar


//Crea la estructura dogType
typedef struct DogType
{
    char nombre[33];
	char tipo[33];
	int edad;
	char raza[17];
	int estatura;
	float peso;
	char sexo;
	int file;
}dogType;

//Genera un aleatorio de tipo entero entre a y b
int aleatorioEntero(int a, int b)
{
    return(a + (rand() % (int)(b - a + 1)));
}

//Genera un aleatorio de tipo real entre a y b
float aleatorioReal(int a, int b)
{
    return((float)aleatorioEntero(a, b) + ((float) rand() / (RAND_MAX)) + 1);
}




int main()
{

    //Carga los archivos de texto con los nombres
	dogType data;
	FILE *nombresMascotas = fopen("nombresMascotas.txt","r");
	int r=0;
	FILE *razasMascotas = fopen("razasMascotas.txt","r");
	int r2=0;
	FILE *tiposMascotas = fopen("tiposMascotas.txt","r");
	int r3=0;

    //Crea el archivo donde se van a guardar los datos
	FILE *ap=fopen("dataDogs.dat","wb");


	for(int i=0; i<DAT_SIZE; i++)
    {
        //Lee el siguiente nombre
		r =	fscanf(nombresMascotas," %[^\n]",data.nombre);
		r2 = fscanf(razasMascotas," %[^\n]",data.raza);
		r3 = fscanf(tiposMascotas," %[^\n]",data.tipo);

        //Genera datos numericos aleatoriamente dentro de un rango determinado en los argumentos
		data.edad = aleatorioEntero(1,20);
		data.estatura = aleatorioEntero(5,100);
		data.peso = aleatorioReal(3,100);

        //Si el iterador es par, sera hombre, sino, sera mujer
		if(i%2 == 0)
        {
            data.sexo = 'H';
        }
        if (i%2==1)
        {
            data.sexo = 'M';
        }
        //Crea el ID de la historia clinica
        data.file = i;
        fwrite(&data, sizeof(struct DogType), 1, ap);

        //Si se llega al final del archivo con los nombres, manda el apuntador al principio del mismo
        if(r==EOF)
        {
        	rewind(nombresMascotas);
        }
        if(r2==EOF)
        {
        	rewind(razasMascotas);
        }
        if(r3==EOF)
        {
        	rewind(tiposMascotas);
        }

	}

    FILE *file_ind;
    file_ind = fopen("file_dat.txt", "w+");
    int dat_size = DAT_SIZE;
    fprintf(file_ind, "%d",dat_size);

    fclose(file_ind);
    fclose(ap);
	fclose(nombresMascotas);
	fclose(razasMascotas);
	fclose(tiposMascotas);

	printf("\n----Fin de la ejecucion----\n");
}
