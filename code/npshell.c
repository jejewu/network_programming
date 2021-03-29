# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <stdlib.h>
# include <stdbool.h>
#include <sys/wait.h>

int process_line(char *input, char **args);
char * remove_first_space(char *input);
int select_fun(char **args, int head, int tail, int parent_pipe, int child_pipe, int pipe_number);
int main_pipe_process(int parent_pipe, int child_pipe, pid_t pid, int pipe_number);
int parent_pipe_in(int pipe_number);
int child_pipe_out(int pipe_number);
int check_redirect(char **args, int head, int tail);
int do_redirect(char **args, int head, int tail, int location, int parent_pipe, int child_pipe, int pipe_number);
int fun_printenv(char **args, int head, int tail);
int fun_setenv(char **args, int head, int tail, int number);
int init();
int deal_pipen(int N);
int routine_pipen();
// no use
int count_executable();
int fill_content(char **commands);
void remove_spaces(char* s);

// use strcat to concate two string
// strcat(str1, str2); the resultant string is stored in str1.
// char path[]="";
// char path[] = "./work_dir/bin/";
// total program

typedef struct{
    int pipe_write;
    int pipe_read;
    int count;
    int pipth; //order of pipeN_pool
}pipeN;
pipeN pipeN_record[1000];
int pipeN_record_len = 0;

int pipeN_pool[1000];
pipeN pipeN_next; //expire pipeN
bool expire_pipeN = 0;
// int next_can_pipe = 0; //next pipeN can used

int count;
int pipe_pool[1000];
pid_t pid_queue[400];
int pidno = 0;
bool pipe_condition[4]; //{0,0,0,0} no-pipe, pipe, number-pip, !-pipe

int main(){
    char **args;
    int divide_alloc;
    init(); //set env PATH to bin:.
    int pipe_number = 0;
    while(1){
        // initialize the status
        pipe_condition[0] = 0;
        // pipe_condition[1] = 0;
        pipe_condition[2] = 0;
        pipe_condition[3] = 0;
        printf("%c ",'%');
        fflush(stdout);
        // input commands
        char input[15001];
        fgets(input, 15001, stdin);
        
        // only enter
        if(input[0] == 10) continue;

        //process line
        char *std;
        
        args = malloc(sizeof(std) * 7000);
        
        divide_alloc = process_line(input, args);
        // to debug process_line()
        // printf("%d\n",divide_alloc);
        // int a;
        // for(a = 0; a < divide_alloc; a++ ){
        //     printf("%d",a);
        //     printf("%s",args[a]);
        //     printf("%ld\n",strlen(args[a]));
            
        // }
        // int pipe_number = 0;
        int parent_pipe = -1, child_pipe = -1;
        int pipe_in, pipe_out;
        int head, tail;
        int last_pipe = 0; //for last command
        bool is_pipN = 0;
        
        if(expire_pipeN){
            parent_pipe = pipeN_next.pipe_read;
        } 
        // implement pipe
        for(head = 0, tail = 0; tail < divide_alloc; tail++){
            // normal pipe
            if(args[tail][0] == '|' && strlen(args[tail]) == 1){
                // printf("pipe\n");
                pipe_condition[1] = 1;
                last_pipe = tail + 1;
                // printf("\nin\n");
                
                if( pipe( pipe_pool + 2 * pipe_number) < 0) printf("normal error");
                
                child_pipe = 2 * pipe_number + 1;
                select_fun(args, head, tail - 1, parent_pipe, child_pipe, pipe_number);
                //next input
                parent_pipe = 2 * pipe_number;
                // printf("%d %d\n", child_pipe, parent_pipe);
                pipe_number = (pipe_number+1)%1000;
                head = tail + 1;
                continue;
            }

            // pipeN
            // check length of |N
            if(args[tail][0] == '|' && strlen(args[tail]) > 1){
                // printf("in pipeN\n");
                pipe_condition[2] = 1;
                last_pipe = tail;
                // string to int
                int N = atoi( args[tail] + 1 );
                // printf("%s %d\n",args[tail], N);
                // compute whether same expire date pipeN
                child_pipe = deal_pipen(N);

                // if( pipe( pipe_pool + 2 * pipe_number) < 0) printf("error");
                // child_pipe = 2 * pipe_number + 1;

                select_fun(args, head, tail - 1, parent_pipe, child_pipe, pipe_number);
                head = tail + 1;
            }
        }
        // check !N but not do
        for(int i = last_pipe; i < divide_alloc; i++){
            // deal !N
            if(args[i][0] == '!'){
                pipe_condition[3] = 1;
                // string to int
                int N = atoi( args[i] + 1 );
                // printf("%s %d\n",args[i], N);
                // compute whether same expire date pipeN
                child_pipe = deal_pipen(N);

                // if( pipe( pipe_pool + 2 * pipe_number) < 0) printf("error");
                // child_pipe = 2 * pipe_number + 1;

                select_fun(args, last_pipe, i - 1, parent_pipe, child_pipe, pipe_number);
                divide_alloc = i;
                break;
            }
        }

        // (last one or only one command) and no pipeN
        if(pipe_condition[2] == 0 && pipe_condition[3] == 0){
            int location = check_redirect(args, last_pipe, divide_alloc);
            // printf("%d \n",location);
            if( location != -1){ //redirect and no pipe after
                // because redirect no need child pipe
                do_redirect ( args, last_pipe, divide_alloc, location, parent_pipe, 1, pipe_number);
            }
            else{ // no redirect
                if( !strcmp(args[last_pipe], "printenv") ){
                    // printf("printenv\n");
                    fun_printenv(args, last_pipe, divide_alloc);
                }
                else if( !strcmp(args[last_pipe], "setenv") ){
                    // printf("setenv\n");
                    fun_setenv(args, last_pipe, divide_alloc, 1);
                    }
                
                else if( !strcmp(args[last_pipe], "exit") ){
                    printf("exit\n");
                    return 0;
                    }
                
                else{
                    int tail = divide_alloc - 1; //before |
                    child_pipe = -1;
                    pipe_out = -1; //no pipN so no pip out for last command
                    // printf("parent %d\n", parent_pipe);
                    // printf("last_pipe %d tail %d\n",last_pipe, tail);
                    // printf("%s\n",args[last_pipe]);
                    select_fun(args, last_pipe, tail, parent_pipe, child_pipe, pipe_number);
                }
            }
        }
        routine_pipen();
    } 
    
    // free dynamic allocate memory
    // only for main process
    // divide command
    for(int i = 0; i < divide_alloc; i++){
        free(args[i]);
    }
    return 0; 
}

int deal_pipen(int N){
    int same_pipeN = -1;
    // find same expire pipeN
    for(int i = 0; i < pipeN_record_len; i++){
        if(pipeN_record[i].count == N){
            same_pipeN = i;
            break;
        }
    }
    //no same create new one
    if(same_pipeN == -1){
        //find can use pipeN in pipeN_pool
        for(int i = 0, j; i < 1000; i+=2){

            for(j = 0; j < pipeN_record_len; j++){
                if(i == pipeN_record[j].pipth){
                    // printf("same\n");
                    break;
                }
            }
            //find empty pipN can use
            if(j == pipeN_record_len){
                // printf("%d\n",i);
                if( pipe(pipeN_pool + i) < 0)printf("pipN error\n");
                pipeN_record[pipeN_record_len].pipe_read = i;
                pipeN_record[pipeN_record_len].pipe_write = i+1;
                pipeN_record[pipeN_record_len].count = N;
                pipeN_record[pipeN_record_len].pipth = i;
                same_pipeN = pipeN_record_len;
                pipeN_record_len ++;
                // printf("new pipeN %d\n",same_pipeN);
                break;
            }
        }
    }
    // return child pipe
    return pipeN_record[same_pipeN].pipe_write;
}

int routine_pipen(){
    for(int i = 0; i < pipeN_record_len; i++){
        pipeN_record[i].count -= 1;
        printf("%d %d\n",i,pipeN_record_len);
        if(pipeN_record[i].count == -1){
            //moving pipN_record
            for(int j = i; j < pipeN_record_len - 1; j++){
                pipeN_record[j] = pipeN_record[j+1];
            }
            pipeN_record_len --;
            continue;
        }
        if(pipeN_record[i].count == 0){
            expire_pipeN = 1;
            pipeN_next = pipeN_record[i]; //remain expire pipeN to next
            // //moving pipN_record
            // for(int j = i; j < pipeN_record_len - 1; j++){
            //     pipeN_record[j] = pipeN_record[j+1];
            // }
            // pipeN_record_len --;
            // // printf("expire\n");
            break;
        }
        expire_pipeN = 0;
    }
    return 0;
}

int init(){
    char *args[] = {"setenv", "PATH", "bin:."};
    fun_setenv(args, 0, 3, 1);
    return 0;
}

int fun_printenv(char **args, int head, int tail){

    printf("%s\n",getenv(args[head + 1]));
    return 0;
}

int fun_setenv(char **args, int head, int tail, int number){
    // for(int i = head; i < tail; i++){
    //     printf("%s\n", args[i]);
    // }
    setenv(args[head+1], args[head+2], number);
    return 0;
}

// fix head is first and tail is last (no include | )
int select_fun(char **args, int head, int tail, int parent_pipe, int child_pipe, int pipe_number){
    //head -> program name tail -> |
    int len = tail - head + 2;
    char *arg[len];
    for(int i = 0; i < len; i++){
        if(i == len - 1){   //last one
            arg[i] = NULL;
            break;
        }
        if(i == 0){
            // ./bin/(6) \0(1)
            arg[i] = malloc( sizeof(char) * ( strlen(args[ head + i ]) + 1 + 15) );
            // strcpy(arg[i], "./work_dir/bin/");
            // printf("\n%ld\n",strlen(args[head + i]));
            strcat(arg[i], args[head + i]);
            continue;
        }
        arg[i] = malloc( sizeof(char) * ( strlen(args[ head + i ]) + 1 ) );
        strcpy( arg[i], args[head + i] );
    }
    //check parse correct
    // for(int i = 0; i <= tail - head; i++){
    //     printf("%d %s ", i, arg[i]);
    // }
    // printf("\n");
    // fork until success
    int a;
    pid_t pid = fork();
    while(pid < 0){
        wait(&a);
        pid = fork();
    }

    if(pid == 0){
        //stdin 0, stdout 1, stderr 2
        // printf("%s\n", arg[0]);
        if(parent_pipe != -1){

            parent_pipe_in(parent_pipe);
        }
        if(child_pipe != -1){
            child_pipe_out(child_pipe);
        }
        
        int err = execvp(arg[0], arg);
        if(err != 0)
            printf("Unknown command: [ %s ].\n",args[head]);
        exit(0);
    }
    else{
        // printf("%s ",args[head]);
        main_pipe_process(parent_pipe, child_pipe, pid, pipe_number);
    }
    
}

int check_redirect(char **args, int head, int tail){
    for(int i = head; i < tail; i++){
        if(args[i][0] == '>'){
            return i;
        }
    }
    return -1;
}

int do_redirect(char **args, int head, int tail, int location, int parent_pipe, int child_pipe, int pipe_number){

    int len = location - head + 1; //exclude > include \0
    char *arg[len];
    for(int i = 0; i < len; i++){
        if(i == len - 1){   //last one
            arg[i] = NULL;
            break;
        }
        if(i == 0){
            // ./bin/(6) \0(1)
            arg[i] = malloc( sizeof(char) * ( strlen(args[ head + i ]) + 1 + 15) );
            // strcpy(arg[i], "./work_dir/bin/");
            // printf("\n%ld\n",strlen(args[head + i]));
            strcat(arg[i], args[head + i]);
            continue;
        }
        arg[i] = malloc( sizeof(char) * ( strlen(args[ head + i ]) + 1 ) );
        strcpy( arg[i], args[head + i] );
    }
    // open file
    // FILE* pfile;
    // pfile = fopen(args[location + 1], "w");

    int a;
    pid_t pid = fork();
    while(pid < 0){
        wait(&a);
        pid = fork();
    }
    //pipeN in
    if(expire_pipeN){
        parent_pipe = pipeN_next.pipe_read;
    }

    if(pid == 0){
        FILE* pfile;
        pfile = fopen(args[location + 1], "w");
        //stdin 0, stdout 1, stderr 2
        // printf("%s\n", arg[0]);
        if(parent_pipe != -1){
            parent_pipe_in(parent_pipe);
        }
        if(child_pipe != -1){
            dup2( fileno(pfile), 1);
        }
        
        int err = execvp(arg[0], arg);
        if(err != 0)
            printf("Unknown command: [ %s ].\n",args[head]);
        exit(0);
    }
    else{
        // printf("%s ",args[head]);
        main_pipe_process(parent_pipe, child_pipe, pid, pipe_number);
    }

}

int child_pipe_out(int pipe_number){
    // printf("in child_pipe %d\n",pipe_number);
    int pipe_out = pipe_number;
    int pipe_in = pipe_number - 1;
    //pipeN
    if(pipe_condition[2] || pipe_condition[3]){
        close(pipeN_pool[pipe_in]);
        if(dup2(pipeN_pool[pipe_out], 1) == -1)printf("|N error\n");
        
        // printf("child pipeN\n");
        if(pipe_condition[3]){
            if(dup2(pipeN_pool[pipe_out], 2) == -1)printf("!N error\n");
        }
        close(pipeN_pool[pipe_out]);
        return 0;
    }

    // printf("%d\n", pipe_out);
    // close input
    close(pipe_pool[pipe_in]);
    dup2(pipe_pool[pipe_out], 1);
    close(pipe_pool[pipe_out]);
}

int parent_pipe_in(int pipe_number){
    // printf("in parent_pipe %d\n",pipe_number);
    int pipe_in = pipe_number;
    int last_pipe_out = pipe_number + 1;
    
    //pipeN
    if(expire_pipeN){
        close(pipeN_pool[last_pipe_out]);
        dup2(pipeN_pool[pipe_in], 0);
        close(pipeN_pool[pipe_in]);
        return 0;
    }

    close(pipe_pool[last_pipe_out]);
    // printf("%s %d \n", arg[0], pipe_pool[(pipe_number-1) * 2 + 1]);
    dup2(pipe_pool[pipe_in], 0);
    close(pipe_pool[pipe_in]);
    // printf("%d\n", pipe_in);
    return 0;
}

int main_pipe_process(int parent_pipe, int child_pipe, pid_t pid, int pipe_number){
    // printf("%d %d\n",parent_pipe, child_pipe);
    // fflush(stdout);
    //record pid
    pid_queue[pidno] = pid;
    pidno = (pidno + 1) % 400;
    // last command or not 1->last 0->not last
    bool last = child_pipe == -1;
    
    if(expire_pipeN != 1 && pipe_condition[3] == 0 && parent_pipe != -1){
        // close parent pipe
        close(pipe_pool[2 * (pipe_number - 1)]);
        close(pipe_pool[2 * (pipe_number-1) +1]);
    }
    if(expire_pipeN == 1){
        // printf("close pipeN %d\n",parent_pipe);
        close(pipeN_pool[parent_pipe]);
        close(pipeN_pool[parent_pipe + 1]);
        expire_pipeN = 0;
    }

    if(last){
        for(int i = 0; i < pidno; i++){
            int status;
            // printf("%d",pid_queue[i]);
            // fflush(stdout);
            waitpid(pid_queue[i], &status, 0);
                
        }
    }


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