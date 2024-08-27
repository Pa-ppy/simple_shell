#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* External environment variable */
extern char **environ;

/* Function prototypes */
void display_prompt(void);
char *read_command(void);
void execute_command(char *buffer);
char *find_command(char *command);
int builtin_handler(char **argv);

#endif
