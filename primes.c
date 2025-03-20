#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	printf("Hola mundo");
	if (argc < 2) {
		printf("Debes indicar un numero para calcular los primos menores a ese numero\n");
		return 2;
	}
	char* n_char = argv[1];
	int n = atoi(n_char);
	int numeros_primos[n - 1];
	int j = 2;
	int pipe_izquierdo[2];
	int r = pipe(pipe_izquierdo);
	for (int i = 0; i < n - 1; i++) {
		numeros_primos[i] = j;
		j++;
	}
	if (r < 0) {
		printf("Error en pipe");
		return 2;
	}
	int primo_recibido;
	int pid = fork();
	if (pid == 0) {
		// Soy el hijo
		close (pipe_izquierdo[1]);
		while (read(pipe_izquierdo[0], &primo_recibido, sizeof(primo_recibido)) > 0) {
			printf("Primo: %i\n", primo_recibido);
			int pipe_derecho[2];
			int r = pipe(pipe_derecho);
			if (r < 0) {
				printf("error en fork");
				return 2;
			}
			int pid = fork();
			if (pid < 0) {
				printf("Error en fork");
				return 2;
			}
			if (pid == 0) {
				// soy el hijo
				pipe_izquierdo[0] = pipe_derecho[0];
				close(pipe_derecho[1]);
				close (pipe_izquierdo[0]);
			}
			else {
				int i = primo_recibido;
				while ((read(pipe_izquierdo[0], &primo_recibido, sizeof(primo_recibido))) > 0){
					if (primo_recibido % i != 0) {
						write(pipe_derecho[1], &primo_recibido, sizeof(primo_recibido));
					}
				}
				close (pipe_izquierdo[0]);
				close (pipe_derecho[1]);
			}
		}
	}
	else {
		printf("Primo: %i\n", numeros_primos[0]);
		close (pipe_izquierdo[0]);
		for (int i = 0; i < n - 1; i++) {
			if (numeros_primos[i] % 2 != 0) {
				write(pipe_izquierdo[1], &numeros_primos[i], sizeof(numeros_primos[i]));
			}
		}
		close (pipe_izquierdo[1]);
		wait(NULL);
	}
	return 0;
}


	// Pseudocódigo:
// 	p := <leer valor de pipe izquierdo>

// imprimir p // asumiendo que es primo

// mientras <pipe izquierdo no cerrado>:
//     n = <leer siguiente valor de pipe izquierdo>
//     si n % p != 0:
//         escribir <n> en el pipe derecho

// (El único proceso que es distinto, es el primero, que tiene que simplemente generar la secuencia de números naturales de 2 a n. No tiene lado izquierdo.)

// $ ./primes <n>
