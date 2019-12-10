#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct file_data //com typedef não preciso de estar sempre a escrever struck file_data_t NOT SURE
{                        // public data
    long word_pos;       // zero-based
    long word_num;       // zero-based
    char word[64];
    // private data
    FILE *fp;
    long current_pos; // zero-based
} file_data_t;

typedef struct w_ele
{
    char word[64];
    int count;          //contador de palavras
    int tdist;          //total da soma das distâncias
    int dmin;           //distância mínima
    int dmax;           //distância máxima
    int last;           //última posição
    int first;          //primeira posição
    struct w_ele *next; // Pointer para a próxima palavra
} w_ele;

unsigned int hash_function(const char *str, unsigned int s)
{ // for 32-bit unsigned integers, s should be smaller that 16777216u
    unsigned int h;
    for (h = 0u; *str != '\0'; str++)
        h = (256u * h + (0xFFu & (unsigned int)*str)) % s;
    return h;
}

void add_ele(w_ele * *words, file_data_t *f){
int index = hash_function(f->word,500);
w_ele * w = malloc(sizeof(w_ele));
strcpy(w->word, f->word);
w->count++;
words[index]=w;
if(index==473){
    w_ele * x = words[473];
    printf("%s\n %d", x->word, x->count);
}

}

int open_text_file(char *file_name, file_data_t *fd)
{
    fd->fp = fopen(file_name, "rb");

    if (fd->fp == NULL)
        return -1;
    fd->word_pos = -1;
    fd->word_num = -1;
    fd->word[0] = '\0';
    fd->current_pos = -1;
    return 0;
}

void close_text_file(file_data_t *fd)
{
    fclose(fd->fp);
    fd->fp = NULL;
}

int read_word(file_data_t *fd)
{
    int i, c;
    // skip white spaces
    do
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            return -1;
        fd->current_pos++;
    } while (c <= 32);
    // record word
    fd->word_pos = fd->current_pos;
    fd->word_num++;
    fd->word[0] = (char)c;
    for (i = 1; i < (int)sizeof(fd->word) - 1; i++)
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            break; // end of file
        fd->current_pos++;
        if (c <= 32)
            break; // terminate word
        fd->word[i] = (char)c;
    }
    fd->word[i] = '\0';
    return 0;
}

int main(int argc, char *argv[])
{
    printf("Manda Nudes Laranjo\n");
    w_ele * words[500];
    if(words[473]==NULL){
    printf("ok");
}
    file_data_t *f = malloc(sizeof(file_data_t));
    if (!open_text_file("test.txt", f))
    {

        while (!read_word(f))
        {
        add_ele(words,f);
        }
    }
    w_ele * w = words[473];
    printf("ok\n");
    printf("%s", w->word);
    //printf("Contador de palavras: %ld     Indice do inicio da palavra atual(em todo o texto): %ld    Indice do final da palavra atual(em todo o texto): %ld\n", f->word_num, f->word_pos, f->current_pos);
    //Falta método para verificar se o elemento da linkedlist está vazio!
}
