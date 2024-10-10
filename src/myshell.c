#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include "myshell.h"

int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE]; // Declare input variable
    if (argc == 2) {
        // Batch mode: read commands from a file
        FILE *batch_file = fopen(argv[1], "r");
        if (batch_file == NULL) {
            perror("Error opening batch file");
            return EXIT_FAILURE;
        }

        while (fgets(input, sizeof(input), batch_file) != NULL) {
            filter_input(input); // remove newline character from input for easier comparison

            char command[MAX_INPUT_SIZE];
            char arg[MAX_INPUT_SIZE];
            char input_file[MAX_INPUT_SIZE];
            char output_file[MAX_INPUT_SIZE];
            int append = 0;

            if (sscanf(input, "%s %[^\n] %s %s %d", command, arg, input_file, output_file, &append) < 2) {
                strcpy(arg, ""); // No argument provided
            }

            if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) { // exit command
                break;
            } else if (strcmp(command, "cd") == 0) { // change directory command
                cd_command(arg);
            } else if (strcmp(command, "clr") == 0) { // clear the screen command
                system("clear");
            } else if (strcmp(command, "dir") == 0) { // list contents of directory command
                dir_command(arg);
            } else if (strcmp(command, "environ") == 0) { // list environment strings command
                environ_command();
            } else if (strcmp(command, "echo") == 0) { // echo command
                echo_command(arg);
            } else if (strcmp(command, "help") == 0) { // display user manual command
                help_command();
            } else if (strcmp(command, "pause") == 0) { // pause operation command
                pause_command();
            } else if (strcmp(command, "pwd") == 0) { // print working directory command
                pwd_command();
            } else { // execute other commands
                execute_command(command, arg, input_file, output_file, append);
            }
        }

        fclose(batch_file);
    } else if (argc == 1) {
        // Interactive mode
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            return EXIT_FAILURE;
        }

        struct passwd *pw = getpwuid(getuid());
        if (pw == NULL) {
            perror("getpwuid() error");
            return EXIT_FAILURE;
        }
        char *username = pw->pw_name;

        char hostname[MAX_HOSTNAME_SIZE];
        if (gethostname(hostname, sizeof(hostname)) != 0) {
            perror("gethostname() error");
            return EXIT_FAILURE;
        }

        while (1) { // user input loop
            printf("%s@%s:%s$ ", username, hostname, cwd);
            if (fgets(input, sizeof(input), stdin) == NULL) {
                perror("fgets() error");
                return EXIT_FAILURE;
            }
            filter_input(input); // remove newline character from input for easier comparison

            char command[MAX_INPUT_SIZE];
            char arg[MAX_INPUT_SIZE];
            char input_file[MAX_INPUT_SIZE];
            char output_file[MAX_INPUT_SIZE];
            int append = 0;

            if (sscanf(input, "%s %[^\n] %s %s %d", command, arg, input_file, output_file, &append) < 2) {
                strcpy(arg, ""); // No argument provided
            }

            if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) { // exit command
                break;
            } else if (strcmp(command, "cd") == 0) { // change directory command
                cd_command(arg);
            } else if (strcmp(command, "clr") == 0) { // clear the screen command
                system("clear");
            } else if (strcmp(command, "dir") == 0) { // list contents of directory command
                dir_command(arg);
            } else if (strcmp(command, "environ") == 0) { // list environment strings command
                environ_command();
            } else if (strcmp(command, "echo") == 0) { // echo command
                echo_command(arg);
            } else if (strcmp(command, "help") == 0) { // display user manual command
                help_command();
            } else if (strcmp(command, "pause") == 0) { // pause operation command
                pause_command();
            } else if (strcmp(command, "pwd") == 0) { // print working directory command
                pwd_command();
            } else { // execute other commands
                invoke_program(command, arg); // function to invoke programs
            }
        }
    }
}