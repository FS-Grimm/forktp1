#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void filtro (int arriba_in){
	int x;
	ssize_t leido = read(arriba_in,&x, sizeof(x));

	if (leido == -1){
		perror("Rompio la lectura del pipe\n");
	}
	if (leido == 0){
		perror("Se llamo a filtro con un EOF\n");
	}

	printf("primo %d",x);

	int abajo[2];
	pipe(abajo);
	if ((*abajo) < 0) {
		perror("Rompio el pipe\n");
	}
	pid_t prox_filtro= fork();

	if (prox_filtro < 0){
		perror("Rompio el fork\n");
	}

	if (prox_filtro == 0) {
		filtro(abajo[0]);
		exit(0);
	} else{
		int numero;
		leido=read(arriba_in,&numero, sizeof(numero));
		if (leido == -1){
			perror("Rompio la lectura del pipe\n");
		}
		while (numero!= EOF){
			if (numero%x!=0) {
				write(abajo[1], &numero, sizeof(numero));
			}
			leido=read(arriba_in,&numero, sizeof(numero));
			if (leido == -1){
				perror("Rompio la lectura del pipe\n");
			}
		}
		close(abajo[1]);
		close(arriba_in);
		wait(0);
	}
}

int main(int argc, char *argv[]){
	int primer_pipe[2];
	ssize_t res_pipe=pipe(primer_pipe);

	pid_t primer_filtro= fork();

	if (res_pipe < 0) {
		perror("Rompio la creacion del pipe\n");
	}

	if (primer_filtro < 0){
		perror("Rompio el fork\n");
	}

	if (primer_filtro == 0) {
		filtro(primer_pipe[0]);
		exit(0);
	} else{
		ssize_t escrito;
		for (int i=2;i<=argc;i++){
			escrito=write(primer_pipe[1], &argv[i], sizeof(argv[i]));
			if (escrito == -1){
				perror("Rompio la lectura del pipe\n");
			}

		}
		close(primer_pipe[1]);
		wait(0);
	}


	return 0;
}