#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define TERMINAR_PROGRAMA -1
#define ULTIMO_PRIMO 0


int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Debes indicar un numero para calcular los primos menores a ese numero\n");
		exit(EXIT_FAILURE);
	}
	char* n_char = argv[1];
	int n = atoi(n_char);
	int pipe_izquierdo[2];
	int r = pipe(pipe_izquierdo);
	if (r < 0) {
		printf("Error en pipe");
		exit(EXIT_FAILURE);
	}
	ssize_t bytes_escritos = 0;
	ssize_t bytes_leidos = 0;
	
	int pid = fork();
	if (pid == 0) {
		int primo_recibido;
		close (pipe_izquierdo[1]);
		while (((bytes_leidos = read(pipe_izquierdo[0], &primo_recibido, sizeof(primo_recibido)))) > 0) {
			if (bytes_leidos == -1) {
				printf("Error al leer del pipe");
			}
			printf("primo %i\n", primo_recibido);
			int pipe_derecho[2];
			int r = pipe(pipe_derecho);
			if (r < 0) {
				printf("error en pipe");
				exit(EXIT_FAILURE);
			}
			pid = fork();
			if (pid < 0) {
				printf("Error en fork");
				exit(EXIT_FAILURE);
			}
			if (pid == 0) {
				int pipe_cerrar = pipe_izquierdo[0];
				pipe_izquierdo[0] = pipe_derecho[0];
				close(pipe_cerrar);
				close (pipe_derecho[1]);
			}
			else {
				int i = primo_recibido;
				close(pipe_derecho[0]);
				while (((bytes_leidos = read(pipe_izquierdo[0], &primo_recibido, sizeof(primo_recibido)))) > 0){
					if (bytes_leidos == 0) {
						printf("Error al leer del pipe");
					}
					if (primo_recibido % i != 0) {
						bytes_escritos = write(pipe_derecho[1], &primo_recibido, sizeof(primo_recibido));
						if (bytes_escritos == -1) {
							printf("Error al escribir en pipe");
							exit(EXIT_FAILURE);
						}
					}
				}
				close (pipe_izquierdo[0]);
				close (pipe_derecho[1]);
				wait(NULL);
			}
		}
		close (pipe_izquierdo[0]);
		return 0;
	}
	else {
		printf("primo %i\n", 2);
		close (pipe_izquierdo[0]);
		for (int i = 3; i <= n; i++) {
			if (i % 2 != 0) {
				bytes_escritos = write(pipe_izquierdo[1], &i, sizeof(i));
				if (bytes_escritos == -1) {
					printf("Error al escribir en pipe");
					exit(EXIT_FAILURE);
				}
			}
		}
		close (pipe_izquierdo[1]);
		wait(NULL);
	}
	return 0;
}
