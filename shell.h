/**
 * KNOWN BUGS:
 * ls : None
 * echo : doesn't print anything when the number of tokens passed exceeds MAXTOKENS - 1
 * pwdir : None
 * cd : None
 * find : Aborts when searching through 2 deep nested folders
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>

#define BUFSIZE (4096)
#define MAXTOKENS (16)
#define TRUE (1)
#define FALSE (0)

/* Definitions of minimum & maximum arguements for functions */
#define LS_MIN (1)
#define LS_MAX (2)
#define ECHO_MIN (1)
#define ECHO_MAX (MAXTOKENS)
#define PWDIR_MIN (1)
#define PWDIR_MAX (PWDIR_MIN)
#define CD_MIN (2)
#define CD_MAX (CD_MIN)
#define FIND_MIN (3)
#define FIND_MAX (4)

char ls_help[] = "Lists the contents of the current working directory, or a specfied one.\n"
                "Usage:\n\tls [directory]";
char echo_help[] = "Repeats back whatever follows \"echo\"\n"
                "Usage: \n\techo [message]";
char pwdir_help[] = "Prints the current working directory\n";
char cd_help[] = "Change the current working directory\n"
                "Usage: \n\tcd [path]";
char find_help[] = "Get the path to a file or directory by name, searching in a given path or the current working directory."
                "Shallow search (-s) to not check sub-directories or deep search (-d) to check sub-directories\n"
                "Usage: \n\tfind [-s|-d] [name] [path] (optional)";

/* Helper functions */

int tokenize(char buf[], char *argv[]);
void printhelp(void);
int find_recursive(char *, char *);

/* Builtin shell functions */

/**
 * Lists the contents of the current working directory, or a specified one.
 */
int ls(int, char **);

/**
 * Repeats back what follows "echo"
 */
int echo(int, char **);

/**
 * Prints the current working directory
 */
int pwdir(int, char **);

/**
 * Changes the current working directory
 */
int cd(int, char **);

/**
 * Searches for a specific file or directory
 */
int find(int, char**);

// Define function pointer
typedef int (*func_ptr)(int, char **);

// Define function list entry
struct func_entry {
    char *name;
    char *help;
    func_ptr func;
};

// A list of builtin shell commands. This is checked first when a command is given to the shell
struct func_entry builtins[] = {
    { "ls", ls_help, ls },
    { "echo", echo_help, echo },
    { "pwdir", pwdir_help, pwdir },
    { "cd", cd_help, cd },
    { "find", find_help, find }
};