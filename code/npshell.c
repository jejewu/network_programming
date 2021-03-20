# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <stdlib.h>
# include <stdbool.h>


int count_executable();
int fill_content(char **commands);
void remove_spaces(char* s);
int process_line(char *input, char **args);
char * remove_first_space(char *input);

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

    while(1){
        printf("$ ");
        // input commands
        char input[15001];
        fgets(input, 15001, stdin);
        
        // only enter
        if(input[0] == 10) continue;

        //process line
        char *std;
        char **args;
        args = malloc(sizeof(std) * 7000);
        int divide_alloc;
        divide_alloc = process_line(input, args);
        //to debug process_line()
        // int a;
        // for(a = 0; a < b; a++ ){
        // printf("%s",args[a]);
        // printf("%ld\n",strlen(args[a]));
        // }
    } 
    // pointer array
    //char *input_command[]={"ls","./test.html"};
    //scanf("%s", *input_command);
    // printf("number %ld\n", strlen(input_command));
    // char *a[]={"cat","test.html",0};
    // execv("bin/cat",a);
    
    // free dynamic allocate memory
    // only for main process
    // commands
    int i;
    for(i = 0; i < count; i++){
        free(commands[i]);
    }
    // divide command
    for(i = 0; i < divide_alloc; i++){
        free(args[i]);
    }
    return 0; 
}

int process_line(char *input, char **args){
    
    
    // input = remove_first_space(input);
    // printf("%s", input);
    char divide = ' ';
    char *re_hand, *re_last=NULL;
    // first command
    re_hand = remove_first_space(input);
    re_last = strchr(re_hand, divide);
    if(re_last == NULL){
        // printf("NULL\n");
        re_last = strchr(re_hand, '\n');
    }
    //copy
    int i = 0;
    args[i] = malloc( sizeof(char) * (re_last - re_hand + 1) );
    args[i][re_last - re_hand] = '\0';
    strncpy(args[i], re_hand, re_last - re_hand);
    i++;
    // printf("haha\n");

    for(;;i++){
        re_hand = remove_first_space(re_last);
        if(re_hand[0] == '\n') return i;
        // find next space
        re_last = strchr(re_hand, divide);
        // lasr noe remain
        if(re_last == NULL){
            break;
        }
        args[i] = malloc( sizeof(char) * (re_last - re_hand + 1) );
        args[i][re_last - re_hand] = '\0';
        strncpy(args[i], re_hand, re_last - re_hand);
    }
    args[i] = malloc( sizeof(char) * ( strlen(re_hand)) );
    args[i][strlen(re_hand) - 1] = '\0';
    strncpy(args[i], re_hand, strlen(re_hand) -1);
    // printf("%s ", args[i]);
    // printf("%d",i);
    // printf("fuck");
    return i+1;

}

char * remove_first_space(char *input){
    while(1){
        if(input[0] == ' ') input++;
        else{
            break;
        }
    }
    return input;
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