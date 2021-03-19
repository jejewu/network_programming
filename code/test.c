# include <string.h>
# include <stdio.h>
# include <stdlib.h>


int main(){
    char s[1000]="ioarhvp;o oisehrdgv;po   oarihv;   oirshvgo;";
    // scanf("%s", s);
    char *a = s;
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*a++ = *d++);
    printf("%s", a);
    printf("%s", s);
}