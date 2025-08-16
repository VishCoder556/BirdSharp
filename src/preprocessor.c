#include <libgen.h>


void preprocess(char *main_file, Tokenizer *tokenizer){
    for (int i=0; i<tokenizer->tokenlen; i++){
        if (tokenizer->tokens[i].type == TOKEN_HASH && i + 3 < tokenizer->tokenlen){
            if (tokenizer->tokens[i + 1].type == TOKEN_EXC && tokenizer->tokens[i + 2].type == TOKEN_ID && strcmp(tokenizer->tokens[i + 2].value, "include") == 0 && tokenizer->tokens[i + 3].type == TOKEN_STRING){
                char path[512];
                strcpy(path, main_file);
                char *dir = dirname(path);
                char input_file[512];
                snprintf(input_file, 512, "%s/%s", dir,  tokenizer->tokens[i + 3].value);

                Tokenizer *tokenizer2 = malloc(sizeof(Tokenizer));
                FILE *f = fopen(input_file, "r");
                tokenizer2->line = 1;
                tokenizer2->name = input_file;
                tokenizer2->col = 0;
                tokenizer2->code = malloc(90000);
                size_t size = fread(tokenizer2->code, 1, 90000, f);
                tokenizer2->code[size] = '\0';
                fclose(f);
                tokenizer2->cur = 0;
                tokenizer2->tokens = malloc(sizeof(Token) * 9000);
                tokenizer2->tokenlen = 0;

                while(tokenizer_token(tokenizer2) != -1){
                };

                tokenizer2->tokenlen--;

                preprocess(input_file, tokenizer2);

                memmove(
                    tokenizer->tokens + i + tokenizer2->tokenlen,
                    tokenizer->tokens + i + 4,
                    (tokenizer->tokenlen - (i + 4)) * sizeof(Token)
                );

                for (int v=0; v<tokenizer2->tokenlen; v++){
                    tokenizer->tokens[i + v] = tokenizer2->tokens[v];
                }

                tokenizer->tokenlen += tokenizer2->tokenlen - 4;
            };
        }
    };
}
