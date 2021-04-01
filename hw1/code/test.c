# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <stdlib.h>
# include <stdbool.h>



int main(){
    // char s[1000]="ioarhvp;o oisehrdgv;po   oarihv;   oirshvgo;";
    // // scanf("%s", s);
    // char *a = s;
    // const char* d = s;
    // do {
    //     while (*d == ' ') {
    //         ++d;
    //     }
    // } while (*a++ = *d++);
    // printf("%s", a);
    // printf("%s", s);
    // char c[10];
    // fgets(c, 10,stdin);
    // printf("%ld\n",strlen(c));
    // char b[] = "5656\n";
    // printf("%ld\n", strlen(b+1));
    // char *a[] = {"4545","5656"};
    // printf("%ld\n", sizeof("45df45"));
    char a[5] = "454\0";
    char b[4] = "454\0";
    printf("%d\n",strcmp(a, b));
}