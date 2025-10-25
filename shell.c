#include "shell.h"

int main(void) {
    char buf[BUFSIZE];
    int argc = 0;
    char *argv[MAXTOKENS];

    printf("Welcome to my shell! \"help\" for a list of commands, or \"quit\" to quit.\n");
    printf("%s$ ", getcwd(NULL, 0));

    while (fgets(buf, BUFSIZE, stdin) != NULL) {
        buf[strlen(buf) - 1] = '\0';
        argc = tokenize(buf, argv);

        if (strcmp(argv[0], "help") == 0) {
            printhelp();
        } else if (strcmp(argv[0], "quit") == 0) {
            return 0;
        }

        for (int i = 0; i < (sizeof(builtins) / sizeof(struct func_entry)); i++) {
            if (strcmp(builtins[i].name, argv[0]) == 0) {
                builtins[i].func(argc, argv);
            }
        }
        printf("%s$ ", getcwd(NULL, 0));
    }
}

int ls(int argc, char *argv[]) {
    DIR *dir_ptr;
    struct dirent *curr_file;

    switch (argc) {
        case LS_MIN:
            dir_ptr = opendir(".");
            break;
        case LS_MAX:
            dir_ptr = opendir(argv[LS_MAX - 1]);
            break;
        default:
            printf("Usage: ls [directory] (no directory implies the current directory)\n");
            return -1;
    }

    if (dir_ptr == NULL) {
        printf("Error opening directory...\n");
        return -1;
    }

    while ((curr_file = readdir(dir_ptr)) != NULL) {
        if (strcmp(curr_file->d_name, ".") == 0 || strcmp(curr_file->d_name, "..") == 0) { // Skip self and parent referencing directories
            continue;
        }
        printf("%s\n", curr_file->d_name);
    }
    closedir(dir_ptr);
    return 0;
}

int echo(int argc, char *argv[]) {
    for (int i = ECHO_MIN; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

int pwdir(int argc, char *argv[]) {
    printf("%s\n", getcwd(NULL, 0));
    return 0;
}

int cd(int argc, char *argv[]) {
    if (argc < CD_MIN) {
        printf("Usage:\n\tcd [path]\n");
        return -1;
    } else {
        return chdir(argv[1]);
    }
}

int find(int argc, char *argv[]) {
    char *search_type;
    char *name;
    char *path;
    
    if (argc < FIND_MIN) {
        printf("Usage:\n\tfind [-s|-d] [name] [path] (optional)\n");
        return -1;
    } else if (argc == FIND_MIN) {
        search_type = argv[1];
        name = argv[2];
        path = getcwd(NULL, 0);
    } else {
        search_type = argv[1];
        name = argv[2];
        path = argv[3];
    }

    if (strcmp(search_type, "-s") == 0) {
        DIR *dir_ptr;
        struct dirent *curr_file;

        if ((dir_ptr = opendir(path)) == NULL) {
            printf("Error opening directory...\n");
            return -1;
        }

        while ((curr_file = readdir(dir_ptr)) != NULL) {
            if (strcmp(curr_file->d_name, name) == 0) {
                printf("File found at %s/%s\n", path, name);
                return 0;
            }
        }
        printf("File not found...\n");
        return -1;

    } else if (strcmp(search_type, "-d") == 0) {
        if (find_recursive(name, path) == -1) {
            printf("File not found...\n");
            return -1;
        }
        return 0;
    } else {
        printf("Usage:\n\tfind [-s|-d] [name] [path] (optional)\n");
        return -1;
    }
}

int find_recursive(char *name, char *path) {
    DIR *dir_ptr;
    struct dirent *curr_file;

    if ((dir_ptr = opendir(path)) == NULL) {
        printf("Error opening directory...\n");
        return -1;
    }

    while ((curr_file = readdir(dir_ptr)) != NULL) {
        if (strcmp(curr_file->d_name, ".") == 0 || strcmp(curr_file->d_name, "..") == 0) { // Skip self and parent referencing directories
            continue;
        } else if (strcmp(curr_file->d_name, name) == 0) { // File has been found
            printf("File found at %s/%s\n", path, name);
            return 0;
        } else { // Need to check if the current file is a directory
            struct stat fstat;
            char *new_path = malloc(strlen(path) + strlen(curr_file->d_name) + 1);
            strncpy(new_path, path, strlen(path));
            strcat(strcat(new_path, "/"), curr_file->d_name);
            if (stat(new_path, &fstat) == -1) {
                continue;
            }
            if (S_ISDIR(fstat.st_mode)) {
                if (find_recursive(name, new_path) == 0) {
                    return 0;
                }
                continue;
            }
        }
    }
    return -1;
}

/**
 * Tokenizes buf by spaces, puts a pointer to each token in the array argv (argv is null terminated)
 */
int tokenize(char buf[], char *argv[]) {
    int curr_token = 0;
    argv[curr_token] = strtok(buf, " ");

    while (argv[curr_token] != NULL) {
        argv[++curr_token] = strtok(NULL, " ");
    }
    return curr_token;
}

/**
 * Prints the help string for each builtin shell command
 */
void printhelp(void) {
    for (int i = 0; i < (sizeof (builtins) / sizeof (struct func_entry)); i++) {
        printf("%s : %s\n", builtins[i].name, builtins[i].help);
    }
}