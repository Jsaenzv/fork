#ifndef NARGS
#define NARGS 4
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	if (argc == 0) {
		printf("Debes incluir un comando a ejecutar en tus argumentos\n");
		return 2;
	}
	char *argumento = NULL;
    size_t largo = 0;
	ssize_t cantidad_argumentos = 0;

	char *argumentos[6];
	argumentos[0] = argv[1];
	for (int i = 1; i < 6; i++) {
		argumentos[i] = NULL;
	}

	size_t cantidad_caracteres = strlen(argumento);

	while (getline(&argumento, &largo, stdin) != -1) {
		cantidad_caracteres = strlen(argumento);
		if (cantidad_caracteres > 0 && argumento[cantidad_caracteres - 1] == '\n') {
			argumento[cantidad_caracteres - 1] = '\0';
		}
		cantidad_argumentos++;
		argumentos[cantidad_argumentos] = strdup(argumento);
		if (cantidad_argumentos == 4) {
			int pid = fork();
			if (pid == 0) {
				execvp(argv[1], argumentos);
				perror("execvp failed");
				return 0;
			} else {
				wait(NULL);
				for (int i = 1; i < 5; i++) {
					free(argumentos[i]);
					argumentos[i] = NULL;
				}
				cantidad_argumentos = 0;
			}
		}
	}

	if (cantidad_argumentos != 0) {
		execvp(argv[1], argumentos);
	}

	for (int i = 1; i <= cantidad_argumentos; i++) {
		free(argumentos[i]);
	}

	free(argumento);

	return 0;
}
