# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <stdlib.h>
# include <stdbool.h>

typedef struct command {
    char *program;
    char *object;
    bool last;
} command;

int count_executable();
int fill_content(char **commands);
void remove_spaces(char* s);

// use strcat to concate two string
// strcat(str1, str2); the resultant string is stored in str1.
char path[] = "./bin/";

int main(){
    //check what is in bin
    int count;
    //substract . and ..
    count = count_executable() - 2;
    // printf("%d\n",count);
    // checking what is in bin/
    char *commands[count];
    fill_content(commands);
    // //debug commands correct or not
    // int i;
    // printf("%d",count);
    // for(i = 0; i < count; i++){
    //     printf("%d",i);
    //     printf("%s\n", commands[i]);
    // }

    
    //
    while(1){
        printf("$ ");
        // input commands
        char input[15001];
        fgets(input, 15001, stdin);

        //divide commands
        char *pointer = input;
        command *next_command;
        
        while(1){
            next_command = extract_next(pointer);
            
        }
        

        // remove space in commands
        // remove_spaces(input);
        // printf("%s",input);
    } 
    // pointer array
    //char *input_command[]={"ls","./test.html"};
    //printf("$ ");
    //scanf("%s", *input_command);
    // printf("number %ld\n", strlen(input_command));
    //printf
    // char *a[]={"cat","test.html",0};
    // execv("bin/cat",a);
    
    
    
    // free dynamic allocate memory
    // only for main process
    // commands
    int i;
    for(i = 0; i < count; i++){
        free(commands[i]);
    }
    return 0; 
}

command* extract_next(char *input){
    command *a;
    return a; 
}

int count_executable(){
    int count = 0;

    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir("./bin");
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    while ((de = readdir(dr)) != NULL) 
            count++;
    closedir(dr);
    return count;
}

int fill_content(char **commands){
    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir("./bin");
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    }
    int i = 0;
    while ((de = readdir(dr)) != NULL){
        // printf("%d",i);
        if(i == 0 || i == 1){
            i++;
            // printf("%s\n",de->d_name);
            continue;
        }
        // i substract 2 for move backward 2 position to ignore . and ..
        commands[i-2] = malloc( sizeof(char) * ( strlen(de->d_name) + 1 ) );
        strcpy(commands[i-2], de->d_name);
        // printf("%s\n",de->d_name);
        i++;
    } 
    closedir(dr);
    return 0;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
    // printf("%s",s);
}