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
#include <time.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>


#define PORT 3536
#define BACKLOG 32
#define NUMTHREADS 32
#define MAX_PROCESOS 1


#define TUBERIA 0
#define MUTEX 1
#define SEMAFOROS 0


//VARIABLE SEMAFOROS

sem_t *semaforo;

//VARIABLE MUTEX

pthread_mutex_t lock;


//VARIABLE TUBERIAS

int fd[2];

int historias[NUMTHREADS];

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

void registrarLog(int o, int n, char s[32], char ip[12]){
	FILE *log_f;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	log_f = fopen("serverDogs.log", "a");
	fprintf(log_f, "[%d %02d %02d %02d %02d %02d]", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	fprintf(log_f, "[%s]", ip);
	switch(o){
		case 1:
		fprintf(log_f, "[Insercion][%d]\n", n);
		break;
		case 2:
		fprintf(log_f, "[Lectura][%d]\n", n);
		break;
		case 3:
		fprintf(log_f, "[Borrado][%d]\n", n);
		break;
		case 4:
		fprintf(log_f, "[Busqueda][%s]\n", s);
		break;
	}
	fclose(log_f);
}

void ingresarRegistro(dogType in){
	FILE *ap;
	ap = fopen("dataDogs.dat", "ab");
	fwrite(&in, sizeof(struct DogType), 1 , ap);
	fclose(ap);
	int dat_file;
	ap = fopen("file_dat.txt","r");
	fscanf(ap,"%d",&dat_file);
	in.file = dat_file;
	fclose(ap);
	dat_file++;
	ap = fopen("file_dat.txt","w");
	fprintf(ap, "%d",dat_file);
	fclose(ap);
}
int sizeData(){

	FILE *ap;
	ap = fopen("dataDogs.dat","rb");
	int count=0;

	fseek(ap, 0, SEEK_END);
	//Numero de posiciones sobre el tamano de la estructura
	count = ftell(ap)/sizeof(struct DogType);
	fclose(ap);
	return count;
}


void borrarRegistro(int pos){
	dogType reg;
	FILE *ap, *ap2;
	//Crea un archivo dat nuevo
	ap = fopen("dataDogs.dat", "rb");
	ap2 = fopen("temp.dat", "wb");


	//Copia los datos hasta una posición antes del que se va a eliminar
	for(int i=0; i<pos; i++){
		fread(&reg, sizeof(struct DogType), 1, ap);
		fwrite(&reg, sizeof(struct DogType), 1, ap2);
	}
	//Omite en el copiado la posición que se quiere eliminar
	fseek(ap, sizeof(struct DogType), SEEK_CUR);
	//Continua copiando los datos desde la posición siguiente al dato que se va a eliminar
	for(int i=pos+1; i<sizeData(); i++){
		fread(&reg, sizeof(struct DogType), 1, ap);
		fwrite(&reg, sizeof(struct DogType), 1, ap2);
	}
	//Se elimina el archivo viejo y el nuevo adopta su nombre
	fclose(ap);
	fclose(ap2);
	system("rm -f dataDogs.dat");
	system("mv temp.dat dataDogs.dat");
}

void * funcion(void *arg){
	int clientfd = *(int *)arg;;
	int r;
	char buffer[32];
	int yes =1;
	int select;
	int exit = 0;
	int count =0;
	int pos,flag;
	char nombre[32];
	int id_f =10000000;
	dogType registro;
	dogType reg;
	int n;
	int historiaOpcion;
	char ch;
	FILE *ap;
	int resp;

	char ip[32];
	r= recv(clientfd,&ip,sizeof(ip),0);
	printf("Cliente conectado: %s\n", ip);
	while(exit!=1)
	{
		//recibe la opcion tomada por el cliente
		r=recv(clientfd,&select,sizeof(int),0);
		if(r<=0){
			//errores
			perror("error en la creacion del recv");
		}

		switch(select){
			case 0:
			break;

			//INGRESAR REGISTRO CASO 1
			case 1:
			//recibe el registro del cliente para lamacenarlo en el sistema



			//espera la respuesta del cliente para validar la informacion
			recv(clientfd,&resp,sizeof(int),0);

			if(resp==1){

			/////////////SECCION CRITICA//////////////

			//tuberias

				if(TUBERIA==1){

				do{

					//printf("se ejecuta\n");
			    	read(fd[0],&ch,1);
					//printf("%c\n",ch );
			    }while(ch!='a');
			    //printf("sale del while\n");
			}

			//MUTEX
			if(MUTEX==1){

				//printf("se ejecuta\n");
				pthread_mutex_lock(&lock);
				//printf("se ehecuta 2\n");
			}

			if(SEMAFOROS==1){

				//printf("se ejecuta uno\n");
				sem_wait(semaforo);
				//printf("se ejecuta dos\n");
			}


			//se recibe y escribe el registro  y se guarda el log
			r=recv(clientfd,&registro,sizeof(dogType),0);

			ingresarRegistro(registro);
			n = sizeData();
			registrarLog(1, n, "0", ip);


			if(TUBERIA==1){
		    	ch='a';
		    	write(fd[1],&ch,1);

		    }
			//MUTEX
			if(MUTEX==1){

				pthread_mutex_unlock(&lock);
			}

			if(SEMAFOROS==1){

				sem_post(semaforo);
			}







			/////////////SECCION CRITICA//////////////
			}
			//tuberias


			break;


			//VER REGISTRO CASO 2
			case 2:
			n = sizeData();
			//se envia la cantidad archivos que existen
			send(clientfd,&n,sizeof(int),0);
			//recibe la pos que se desea
			recv(clientfd,&pos,sizeof(int),0);
			ap = fopen("dataDogs.dat", "rb");

			if(pos>=n){
				fclose(ap);
			}else{
				int cero = 0, uno = 1;
				fseek(ap, sizeof(struct DogType)*pos, SEEK_SET);
				fread(&reg, sizeof(struct DogType), 1, ap);
				//se envia el registro que se desea imprimir
				send(clientfd,&reg,sizeof(dogType),0);
				registrarLog(2, pos, "0", ip);
				recv(clientfd,&historiaOpcion,sizeof(int),0);
				if(historiaOpcion==1){

				int i, abierto=0, hActual=0;
					for(i=0; i<NUMTHREADS; i++){
						if(historias[i]==pos){
							//printf("for1 %d\n", historias[i]);
							abierto = 1;
							hActual = i;
							break;
						}
					}

					if(abierto==1){
						do{
							for(i=0; i<NUMTHREADS; i++){
						
								if(historias[i]==pos){
								
									hActual = i;
								
								}
								
							}
						}while(historias[hActual]!=-1);
					}
					

					for(i=0; i<NUMTHREADS; i++){
						//printf("for2 %d\n", historias[i]);
						if(historias[i]==-1){
							historias[i] = pos;
							hActual = i;
							break;
						}
					}


					//printf("se ejecuta 1");
					sprintf(buffer,"%d.txt", reg.file);
					//printf("%s\n", buffer);
					//printf("se ejecuta 1");
					fclose(ap);
					//printf("se ejecuta 2");
					ap = fopen(buffer, "r");
					/********************************/
					if(ap==NULL){
						sprintf(buffer,"touch %d.txt", reg.file);
						system(buffer);
						sprintf(buffer,"%d.txt", reg.file);
					}
					/********************************/
					ap = fopen(buffer, "r");
					fseek(ap, 0, SEEK_END);
					n = ftell(ap);
					fclose(ap);
					ap = fopen(buffer, "r");
					//printf("%d\n", n);
					r = send(clientfd, &n, sizeof(int), 0);
					if(r<0){
						printf("ERROR send 1\n");
					}
					char text[128];
					
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

					


					

					/////////////SECCION CRITICA//////////////

					//TUBERIA
					
					/*if(TUBERIA==1){
						do{
					    	read(fd[0],&ch,1);

					    }while(ch!='a');
					}
					//MUTEX
					if(MUTEX==1){

						pthread_mutex_lock(&lock);
					}
					if(SEMAFOROS==1){

						sem_wait(semaforo);
					}*/

					ap = fopen(buffer, "w+");

					do{
						recv(clientfd, &r, sizeof(int), 0);
						if(r == 1){
							break;
						}
						recv(clientfd, &text, sizeof(text), 0);
						fprintf(ap, "%s\n", text);
					}while(r!=1);

					historias[hActual] = -1;

					//TUBERIA

					
					/*if(TUBERIA==1){
						ch='a';
				    	write(fd[1],&ch,1);
				    }
				    //MUTEX
				    if(MUTEX==1){

						pthread_mutex_unlock(&lock);
					}

					if(SEMAFOROS==1){

						sem_post(semaforo);
					}*/

					/////////////SECCION CRITICA//////////////
				}
				fclose(ap);


			}




			break;

			//BORRAR REGISTRO CASO 3

			case 3:

			n=sizeData();
			//se envia la cantidad archivos que existen
			send(clientfd,&n,sizeof(int),0);


			//recibe la desiciom del usuario

			recv(clientfd,&resp,sizeof(int),0);

			if(resp==1){
				//se recibe la posicion que se desea eliminar
				recv(clientfd,&pos,sizeof(int),0);

				/////////////SECCION CRITICA//////////////

					//TUBERIA
				if(TUBERIA==1){
					do{
				    	read(fd[0],&ch,1);

				    }while(ch!='a');
				}
				//MUTEX
				if(MUTEX==1){

					pthread_mutex_lock(&lock);
				}
				if(SEMAFOROS==1){

					sem_wait(semaforo);
				}



				borrarRegistro(pos);
				registrarLog(3, pos, "0", ip);

				//TUBERIA
				if(TUBERIA==1){
					ch='a';
			    	write(fd[1],&ch,1);
			    }
			    //MUTEX
			    if(MUTEX==1){

					pthread_mutex_unlock(&lock);
				}

				if(SEMAFOROS==1){

					sem_post(semaforo);
				}

				/////////////SECCION CRITICA//////////////



			}




			break;


			//BUSCAR REGISTRO CASO 4

			case 4:
			recv(clientfd,nombre,sizeof(char)*32,0);
			FILE *ap;
			ap = fopen("dataDogs.dat", "rb");
			nombre[0] = toupper(nombre[0]);
			flag =0;
			//Busca con un FOR (Poco eficiente)
			for(int i=0; i<sizeData(); i++){
				//Lee el siguiente registro del archivo

				fread(&reg, sizeof(struct DogType), 1, ap);

				//Si el nombre concuerda se imprime y continua la busqueda
				if(strncmp(reg.nombre,nombre,32)==0){
					flag=2;
					//envia un flag =2
					send(clientfd,&flag,sizeof(int),0);
					//envia el registro que posee un nombre que conincide
					send(clientfd,&reg,sizeof(dogType),0);
					//envia el id del cliente que es dado por su poscicion
					send(clientfd,&i,sizeof(int),0);
				}
			}
			//una vez se termina la busqueda se retorna flag =1
			//para que termine el ciclo en el cliente
			flag=1;
			send(clientfd,&flag,sizeof(int),0);
			registrarLog(4, 0, nombre, ip);
			fclose(ap);
			break;

			//salir del programa
			case 5:
			//se recibe el exit
			recv(clientfd,&exit,sizeof(int),0);
			if(exit == 1){
				close(clientfd);
			}
			break;
		}
	}
	printf("Cliente desconectado: %s\n", ip);
}


int main(){
	int serverfd,r,clientfd;
	sockaddr_in server,client;
	socklen_t size=sizeof(struct sockaddr_in);
	socklen_t size_cli;
	int thread_num=0;
	pthread_t hilo[NUMTHREADS];

	for(int i=0; i<NUMTHREADS; i++){
		historias[i] = -1;
	}


	//tuberias

	pipe(fd);

	if(TUBERIA==1){
		//printf("se escribio ch\n");
		char ch ='a';
		write(fd[1],&ch,1);
	}

	//printf("hola mundo\n");

	semaforo=sem_open("semaforo_name", O_CREAT, 0700, MAX_PROCESOS);

	//AF_INET = Ipv4
	// SOCKE_STREAM = tcp
	serverfd = socket(AF_INET,SOCK_STREAM,0);
	if(serverfd ==-1){
		perror("error al crear el socket");
	}

	server.sin_family=AF_INET;
	server.sin_port= htons(PORT); //htons revisa el endianismo que posee la maquina
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8);

	int enable = 1;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");

	r=bind(serverfd,(struct sockaddr*)&server,size);
	if(r<0){

		perror("error en el bind");
		//errores
	}

	r=listen(serverfd,BACKLOG);
	if(r<0){
		//errores
		perror("error en el listen");
	}


	int inicio=1;
	while(1==1){
		//printf("se ejecuta 1\n");
		clientfd=accept(serverfd,(struct sockaddr*)&server,(socklen_t*)&size_cli);
		if(clientfd<0){
			//errores0
			perror("ERROR: Accept");
		}
		//printf("se ejecuta 1\n");
		r=pthread_create(&hilo[thread_num],NULL,(void *)funcion,(void *)&clientfd);
		if(r<0){
			perror("ERROR: Hilo");
			exit(-1);
		}
		//printf("se crea el hilo\n");
		fflush(stdout);
	}

	close(clientfd);
	close(serverfd);

	return 0;
}
