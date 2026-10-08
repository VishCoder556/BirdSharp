#ifndef STB_LANG_ERROR_H
#define STB_LANG_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void stb_lang_error_major_global_underlying(char *type, char *fmt, ...);
void stb_lang_error_minor_underlying(char *file, char *contents, int offset, char *type, char *fmt, ...);



#define stb_lang_error_minor(...) \
stb_lang_error_minor_underlying(__VA_ARGS__); exit(-1);

#define stb_lang_error_major_global(...) \
stb_lang_error_major_global_underlying(__VA_ARGS__); exit(-1);


#ifdef STB_LANG_ERROR_IMPLEMENTATION

void stb_lang_error_major_global_underlying(char *type, char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    int addcap = 256;
    char *add = arena_alloc(&g_arena, addcap);
    if (add == NULL){
        printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to generate errors");
        exit(-1);
    }

    int n = vsnprintf(add, addcap, fmt, args);

    if (n > addcap){
        addcap = n;
        add = realloc(add, addcap);
        if (add == NULL){
            printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to generate errors");
            exit(-1);
        }
    }
    va_end(args);


    printf("\x1b[1;31m%s\x1b[0m: %s\n", type, add);
}

void stb_lang_error_hint(char *type, char *elaboration){
    printf("\x1b[38;5;208mhint\x1b[0m: %s\n", type);
    int linen = 0;
    printf(" %d | ", linen++);
    for (int i=0; i<(int)strlen(elaboration); i++){
        if (elaboration[i] == '\n'){
            printf("\n %d | ", linen++);
        }else {
            putchar(elaboration[i]);
        }
    }
    printf("\n");
}
void stb_lang_note_minor(char *file, char *contents, int offset, char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    int addcap = 256;
    char *add = arena_alloc(&g_arena, addcap);
    if (add == NULL){
        printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to generate errors");
        exit(-1);
    }

    int n = vsnprintf(add, addcap, fmt, args);

    if (n > addcap){
        addcap = n;
        add = realloc(add, addcap);
        if (add == NULL){
            printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to generate errors");
            exit(-1);
        }
    }
    va_end(args);

    int row = 0;
    int col=0;
    int last = 0;
    int i=0;
    for (; i<offset; i++){
        if (contents[i] == '\n') {row++;col=i;last=i;};
        if (contents[i] == '\0') {break;};
    };
    char *line = arena_strdup(&g_arena, contents + col);
    char *oldline = line;
    if (line[0] == '\n'){line++;}

    i=last + 1;
    while(1){
        if (contents[i] == '\n') {break;};
        if (contents[i] == '\0') {break;};
        i++;
    };
    line[i-(last)] = '\0';

    col = (offset - col - 1);

    int newrow = row;
    int count = 0;
    do {
        count++;
        newrow /= 10;
    } while (newrow != 0);

    row++;
    // printf("\x1b[1;37m%s:%d:%d: \x1b[1;31m%s\x1b[0m: %s\n%s\n", file, row, col, type, add, line);
    printf("\x1b[1;37m%s:%d:%d: \x1b[38;5;208mnote\x1b[0m: %s\n%d |  %s\n", file, row, col, add, row, line);
    for (int i=0; i<count+1; i++){printf(" ");}
    printf("|  ");
    for (int i=0; i<col; i++){printf(" ");}
    printf("^");
    printf("\n");
    free(oldline);

}

/*
code/main.c:2134:32: note: to match this '{'
 2134 | int main(int argc, char **argv){
      |                                ^
*/

void stb_lang_error_minor_underlying(char *file, char *contents, int offset, char *type, char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    int addcap = 256;
    char *add = arena_alloc(&g_arena, addcap);
    if (add == NULL){
        printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to generate errors");
        exit(-1);
    }

    int n = vsnprintf(add, addcap, fmt, args);

    if (n > addcap){
        addcap = n;
        add = realloc(add, addcap);
        if (add == NULL){
            printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to generate errors");
            exit(-1);
        }
    }
    va_end(args);

    int row = 0;
    int col=0;
    int last = 0;
    int i=0;
    for (; i<offset; i++){
        if (contents[i] == '\n') {row++;col=i;last=i;};
        if (contents[i] == '\0') {break;};
    };
    char *line = arena_strdup(&g_arena, contents + col);
    char *oldline = line;
    if (line[0] == '\n'){line++;}

    i=last + 1;
    while(1){
        if (contents[i] == '\n') {break;};
        if (contents[i] == '\0') {break;};
        i++;
    };
    line[i-(last + 1)] = '\0';

    col = (offset - col - 1);

    int newrow = row;
    int count = 0;
    do {
        count++;
        newrow /= 10;
    } while (newrow != 0);

    row++;
    // printf("\x1b[1;37m%s:%d:%d: \x1b[1;31m%s\x1b[0m: %s\n%s\n", file, row, col, type, add, line);
    printf("\x1b[1;37m%s:%d:%d: \x1b[1;31m%s\x1b[0m: %s\n%d |  %s\n", file, row, col, type, add, row, line);
    for (int i=0; i<count+1; i++){printf(" ");}
    printf("|  ");
    for (int i=0; i<col; i++){printf(" ");}
    printf("^");
    printf("\n");
    free(oldline);
}



#endif

#endif
