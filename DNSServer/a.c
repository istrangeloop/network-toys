#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* doit(char* bb){
    char* cc = malloc(32);
    cc = "aaaaa";
    return cc;
}

int main(){
    char *bb;
    bb=doit(bb);
    printf("%s", bb);
}
