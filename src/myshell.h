#ifndef MYSHELL_H
#define MYSHELL_H
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 512
#define MAX_COMMAND_SIZE 1024
#define MAX_HOSTNAME_SIZE 64
char startdir[MAX_INPUT_SIZE];
#define START_DIR getcwd(startdir, sizeof(startdir))
char cwd[MAX_INPUT_SIZE]; 

void filter_input(char *input) {
    if (input == NULL) {
        return;
    }

    int i = 0;
    while (input[i] != '\0' && input[i] != '\n') {
        i++;
    }
    input[i] = '\0'; // Terminate the string
}

void cd_command(char *arg) {
    if (arg[0] == '\0') {
        // No argument provided, change to home directory
        chdir(getenv("HOME"));
    } else {
        // Change to the specified directory
        if (chdir(arg) != 0) {
            printf("No such file or directory\n");
            return;
        }
    }
    // Update the current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
    }
}

void dir_command(char *arg) {
    char command[MAX_COMMAND_SIZE];
    if (arg[0] != '\0') {
        // List contents of the specified directory
        snprintf(command, sizeof(command), "ls -al %s", arg);
    } else {
        // No argument provided, list contents of current directory
        snprintf(command, sizeof(command), "ls -al");
    }

    system(command);
}

void environ_command() {
    extern char **environ;
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

void help_command() {
    char command[MAX_COMMAND_SIZE];
    snprintf(command, sizeof(command), "more %smanual/readme", startdir);
    system(command);
}

void echo_command(char *arg) {
    printf("%s\n", arg);
}

void pause_command() {
    printf("Press Enter to continue...");
    fflush(stdout);
    getchar(); // wait for Enter key press
    printf("\n");
}

void pwd_command() {
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
    } else {
        printf("%s\n", cwd);
    }
}

void execute_command(char *command, char *arg, char *input_file, char *output_file, int append) {
    char full_command[MAX_COMMAND_SIZE];
    if (arg[0] != '\0') {
        snprintf(full_command, sizeof(full_command), "%s %s", command, arg);
    } else {
        snprintf(full_command, sizeof(full_command), "%s", command);
    }

    if (input_file != NULL) {
        freopen(input_file, "r", stdin);
    }

    if (output_file != NULL) {
        if (append) {
            freopen(output_file, "a", stdout);
        } else {
            freopen(output_file, "w", stdout);
        }
    }

    system(full_command);

    if (input_file != NULL) {
        freopen("/dev/tty", "r", stdin);
    }

    if (output_file != NULL) {
        freopen("/dev/tty", "w", stdout);
    }
}

void invoke_program(char *program, char *arg) {
    char *args[] = { program, arg, NULL };

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork() error");
        return;
    } else if (pid == 0) {
        // Child process
        if (execvp(program, args) == -1) {
            perror("execvp() error");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

#endif 
 /* MYSHELL_H */