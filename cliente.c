
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>



#define PORT 3536
#define BACKLOG 32
//#define IP "192.168.0.12"
#define IP "127.0.0.1"






typedef struct Sockaddr_in{
	short sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];

}sockaddr_in;

typedef struct In_addr{
	unsigned long sin_addr;
}in_addr;

//Estructura de los registros
typedef struct DogType{
	//El tamaño de los arreglos es 1 mayor que el tamaño máximo
	//para que no cuente el salto de línea
	char nombre[33];
	char tipo[33];
	int edad;
	char raza[17];
	int estatura;
	float peso;
	char sexo;
	int file;
}dogType;

//Guarda los datos que ingresa el usuario en una estructura dogType
dogType crearDatos(){
	dogType registro;

	//NOMBRE
	//[^\n] toma todo hasta que se encuentre con un \n(salto de linea$
	int count=0;

	do{
		count=0;
		printf("Ingrese el nombre: ");
		scanf("\n%[^\n]s",registro.nombre);
		while(registro.nombre[count]!='\0'){
			count++;
		}
		if(count>=33){
			printf("ERROR: dato demasiado extenso\n");
		}

	}while(count>=33);
	registro.nombre[0]= toupper(registro.nombre[0]);
	//TIPO
	do{
		count=0;
		printf("Ingrese el tipo de animal: ");
		scanf("\n%[^\n]s",registro.tipo);
		while(registro.tipo[count]!='\0'){
			count++;
		}
		if(count>=33){
			printf("ERROR: dato demasiado extenso\n");
		}

	}while(count>=33);



	//EDAD


	printf("Ingrese la edad: ");
	scanf("%d",&registro.edad);


	//RAZA

	do{
		count=0;
		printf("Ingrese el raza: ");
		scanf("\n%[^\n]s",registro.raza);
		while(registro.raza[count]!='\0'){
			count++;
		}
		if(count>=17){
			printf("ERROR: dato demasiado extenso\n");
		}

	}while(count>=17);

	//ESTATURA
	printf("Ingrese la estatura (en cm): ");
	scanf("%d",&registro.estatura);

	//PESO
	printf("Ingrese el peso (en Kg): ");
	scanf("%f",&registro.peso);

	//SEXO
	char tmp;
	do{
		printf("Ingrese el sexo [H/M]: ");
		scanf("\n%c",&tmp);
		if(tmp=='h' || tmp=='m' || tmp=='H' || tmp=='M'){
			break;
		}else{
			printf("ERROR: dato incorrecto. Ingrese 'H' para hombre o 'M' para mujer\n");
		}
	}while(tmp!='h' || tmp!='m');
	registro.sexo = tmp;

	return registro;
}


int confirmacion(){
	char a;
	int r=0;
	printf("Desea continuar [Y/N] ");
	scanf(" %c", &a);
	// Lo vuelve todo mayúsculas
	a=toupper(a);
	if(a=='Y'|| a =='N'){
		r=1;
	}


	//Si la opción es invalida entra en un loop y la pide hasta que sea valida
	while(r==0){
		printf("ERROR: Opcion invalida, ingrese 'Y' para si, 'N' para no: ");
		scanf(" %c", &a);
		a=toupper(a);
		if(a=='Y'|| a =='N'){
			r=1;
		}

	}
	if(a=='Y'){
		return 1;
	}else{
		return 0;
	}
}


void esperar(){
	printf("Presione ENTER para continuar ...");

	char prev = 0;
	while(1)
	{
		//Toma de entrada cualquier cosa para continuar
		char c = getchar();
		if(c == '\n' && prev == c)
		{
			break;
		}
		prev = c;
	}
}
void menu(){
	system("clear");
	printf("MENU PRINCIPAL\n");
	printf("\n1. Ingresar registro\n2. Ver registro\n3. Borrar registro\n4. Buscar registro\n5. Salir\n" );
	printf("? ");
}


void imprimirDatos(dogType registro){
	printf("Nombre = %s\nTipo = %s\nEdad = %d\nRaza = %s\nEstatura = %d\nPeso = %f\nSexo = %c\n",registro.nombre,registro.tipo,registro.edad,registro.raza,registro.estatura,registro.peso,registro.sexo);
}



int main (int srgc, char*argv[]){
	int clientfd,r;
	struct Sockaddr_in client;
	socklen_t size=sizeof(struct sockaddr_in);
	char buffer[32],buffer2[32];
	int yes =1;
	int select;
	int exit = 0;
	int count =0;
	int pos,n,flag,id;
	char nombre[32];
	int id_f =10000000;
	dogType registro;
	FILE *ap;

	clientfd = socket(AF_INET,SOCK_STREAM,0);
	if(clientfd==-1){
		perror("error en el socket");
	}
	client.sin_family = AF_INET;
	client.sin_port =htons(PORT);
	client.sin_addr.s_addr = inet_addr(IP);  //direccion ip del servidor
	bzero(client.sin_zero,8);

	r= connect(clientfd,(sockaddr_in *)&client,size);
	if(r==-1){
		perror("error en la conexion");
	}
	// se termian la conexion de del socket inicia el menu
	//envio de la ip
	system("hostname -I > ip.txt");
	ap = fopen("ip.txt", "r");
	char ip[32];
	fscanf(ap,"%s",ip);
	send(clientfd,&ip,sizeof(ip),0);
	fclose(ap);
	system("rm ip.txt");


	while(exit!=1)
	{

		menu();
		//elige la opcion y la envia al servidor para realizar el mismo proceso
		scanf("%d",&select);
		r=send(clientfd,&select,sizeof(int),0);		//tener cuidado
		if(r!=sizeof(int)){
			perror("no se enviaron todos los paquetes");
		}

		switch(select){
			case 0:
			break;

			case 1:
			system("clear");
			printf("INGRESAR REGISTRO\n");
			registro = crearDatos();
			registro.file = id_f;
			id_f++;
			if(confirmacion()==1){
				//se envia al servidor el registro que se acaba de crear para que lo almacene
				int uno=1;
				send(clientfd,&uno,sizeof(int),0);
				send(clientfd,&registro,sizeof(dogType),0);
			}else{
				int cero=0;
				send(clientfd,&cero,sizeof(dogType),0);
			}
			esperar();
			break;


			case 2:
			system("clear");
			printf("VER REGISTRO\n ID: ");
			scanf("%d",&pos);
			if(confirmacion()==1){
				//se recibe el datasize() la cantidad de datos que existen en el sistema
				//se guarda en la variable n
				recv(clientfd,&n,sizeof(int),0);
				printf("Existen %d registros actualmente\n", n);
				//se envia la posicion que se desea eliminar
				send(clientfd,&pos,sizeof(int),0);
				if(pos>=n){
					printf("ERROR: El registro no existe\n");
				}else{
					//se recibe la esructura dogtype y se guarda en registro
					recv(clientfd,&registro,sizeof(dogType),0);
					imprimirDatos(registro);
					printf("Desea abrir la historia clinica de la mascota?\n");
					if(confirmacion() ==1 ){
						system("clear");
						printf("Por favor espere ...\n");
						int cero = 0;
						int uno = 1;
						send(clientfd, &uno,sizeof(int),0);
						system("clear");
						printf("Por favor espere ...\n");




						recv(clientfd, &n, sizeof(int), 0);
						system("clear");
						char text[128];


						sprintf(buffer,"tmp%d.txt", registro.file);
						ap = fopen(buffer, "w+");
						do{
							recv(clientfd, &r, sizeof(int), 0);
							if(r == 1){
								break;
							}
							recv(clientfd, &text, sizeof(text), 0);
							fprintf(ap, "%s\n", text);
						}while(r!=1);
						fclose(ap);
						sprintf(buffer2, "gedit -s tmp%d.txt", registro.file);
						system(buffer2);


						ap = fopen(buffer, "r");
						do{
							r = fscanf(ap, " %[^\n]", text);
							if(r==EOF){
								send(clientfd, &uno, sizeof(int), 0);
								break;
							}else{
								send(clientfd, &cero, sizeof(int), 0);
							}
							send(clientfd, &text, sizeof(text), 0);
							if(r<0){
								printf("ERROR send 2\n");
							}
						}while(r!=EOF);
						fclose(ap);
						printf("se ejecuta el remover\n");
						sprintf(buffer2, "rm -f tmp%d.txt", registro.file);
						printf("se ejecuta el remover\n");
						system(buffer2);

					}else{
						int cero = 0;
						send(clientfd, &cero,sizeof(int),0);
					}
				}

			}
			esperar();
			break;


			case 3:
			system("clear");
			printf("BORRAR REGISTRO\n ID: ");
			//se recibe el datasize() la cantidad de datos que existen en el sistema
			//se guarda en la variable n
			recv(clientfd,&n,sizeof(int),0);
			scanf("%d", &pos);
			if(confirmacion()==1){

				if(n<=pos || pos< 0){
					int cero = 0;
					send(clientfd, &cero,sizeof(int),0);
					printf("ERROR: El registro no existe\n");
				}else{
					//se envia la pos que se desea eliminar
					int uno = 1;
					send(clientfd,&uno,sizeof(int),0);
					send(clientfd, &pos, sizeof(int),0);
				}
			}else{
				int cero = 0;
					send(clientfd, &cero,sizeof(int),0);
			}
			esperar();
			break;

			case 4:
			system("clear");
			printf("BUSCAR REGISTRO\n Nombre:\n");
			scanf("\n%[^\n]",nombre);
			if(confirmacion()==1){
				//se envia el nombre quee se desea buscar
				send(clientfd,nombre,sizeof(char)*32,0);
				flag=0;
				printf("-----------------------------------------\n");
				//el proceso se repitera flag se volvera 1 una vez
				//el servidor termine de realizar la busqueda de los datos
				//
				while(flag!=1){
					//revisa si la bandera es 2 o 1
					recv(clientfd,&flag,sizeof(int),0);
					if(flag==2){

						recv(clientfd,&registro,sizeof(registro),0);
						recv(clientfd,&id,sizeof(int),0);
						printf("ID: %d\n", id);
						imprimirDatos(registro);
						flag=0;
						printf("-----------------------------------------\n");

					}
				}
			}
			esperar();
			break;

			case 5:
			system("clear");
			printf("SALIR DE LA APLICACION\n");
			exit=confirmacion();
			//se envia el valor al servidor para saber si se debe
			//cerrar el servidor
			send(clientfd,&exit,sizeof(int),0);
			break;
		}
		system("clear");
	}
	close(clientfd);
	return 0;
}
