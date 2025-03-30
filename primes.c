#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void filtro (int arriba_in){
	int x;
	ssize_t leido = read(arriba_in,&x, sizeof(x));

	if (leido == -1){
		perror("Rompio la lectura del pipe\n");
	}
	if (leido == 0){
		return;
	}

	printf("primo %d\n",x);
    fflush(stdout);
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
		while (numero!= EOF){
			if (leido == -1){
				perror("Rompio la lectura del pipe\n");
			}
			if (numero % x !=0) {
				write(abajo[1], &numero, sizeof(numero));
			}
			leido=read(arriba_in,&numero, sizeof(numero));
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
	    int numero_max=atoi(argv[1]);
		ssize_t escrito;
		for (int i=2;i<=numero_max;i++){
			escrito=write(primer_pipe[1], &i, sizeof(i));
			if (escrito == -1){
				perror("Rompio la escrtirura del pipe\n");
			}
		}
		close(primer_pipe[1]);
		wait(0);
	}


	return 0;
}