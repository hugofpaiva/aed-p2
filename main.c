#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// wc SherlockHolmes.txt  shell command count words by space

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
    bool valid; // para verificar se é nula ou não
    char word[64];
    int count;          //contador de palavras
    int tdist;          //total da soma das distâncias (em relação ao contador de palavras geral)
    int tdistp;         //total da soma das distâncias (em relação à posição dos indices)
    int dmin;           //distância mínima (em relação ao contador de palavras geral)
    int dmax;           //distância máxima (em relação ao contador de palavras geral)
    int dminp;          //distância mínima (em relação à posição dos indices)
    int dmaxp;          //distância máxima (em relação à posição dos indices)
    int last;           //última posição (em relação ao contador de palavras geral)
    int first;          //primeira posição (em relação ao contador de palavras geral)
    int lastp;          //última posição (em relação à posição dos indices)
    int firstp;         //primeira posição (em relação à posição dos indices)
    struct w_ele *next; // Pointer para a próxima palavra
} w_ele;

unsigned int hash_function(const char *str, unsigned int s)
{ // for 32-bit unsigned integers, s should be smaller that 16777216u
    unsigned int h;
    for (h = 0u; *str != '\0'; str++)
        h = (256u * h + (0xFFu & (unsigned int)*str)) % s;
    return h;
}

void add_ele(w_ele **words, file_data_t *f)
{
    int index = hash_function(f->word, 500);
    w_ele *actual = words[index];
    if (actual->valid == true) //se já existir um elemento na linked list daquele index
    {
        if (strcmp(actual->word, f->word) == 0)
        { // se for igual

            actual->count++;
        }
        else
        { //se não for igual vou percorrer os next elements para ver se há algum igual
            bool found = false;
            while (actual->next != NULL)
            {
                actual = actual->next;
                if (strcmp(actual->word, f->word) == 0)
                { // se for igual

                    actual->count++;
                    found = true;
                    break;
                }
            }
            if (found) //verificar que nenhum foi encontrado
            {
                w_ele *temp = malloc(sizeof(w_ele));
                strcpy(temp->word, f->word);
                temp->valid = true;
                temp->first = f->word_num;
                temp->count = 1;
                temp->last = f->word_num;
                temp->lastp = f->current_pos;
                temp->firstp = f->word_pos;
                temp->next = NULL;
                temp->dmin = -1;  //indicativo que a distância ainda não foi alterada
                temp->dmax = -1;  //indicativo que a distância ainda não foi alterada
                temp->dminp = -1; //indicativo que a distância ainda não foi alterada
                temp->dmaxp = -1; //indicativo que a distância ainda não foi alterada
                actual->next = temp;
            }
        }
    }
    else
    {
        strcpy(actual->word, f->word);
        actual->valid = true;
        actual->first = f->word_num;
        actual->count++;
        actual->last = f->word_num;
        actual->lastp = f->current_pos;
        actual->firstp = f->word_pos;
    }
}

void get_info(w_ele **words, char name[])
{
    printf("%s", name);
    //get info about a word
    int index = hash_function(name, 500);
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
    int s_hash = 500;
    w_ele *words[s_hash];
    for (int s = 0; s < s_hash; s++)
    {
        w_ele *elem = malloc(sizeof(w_ele));
        elem->valid = false;
        elem->next = NULL;
        elem->count = 0;
        elem->dmin = -1;  //indicativo que a distância ainda não foi alterada
        elem->dmax = -1;  //indicativo que a distância ainda não foi alterada
        elem->dminp = -1; //indicativo que a distância ainda não foi alterada
        elem->dmaxp = -1; //indicativo que a distância ainda não foi alterada
        words[s] = elem;
    }
    file_data_t *f = malloc(sizeof(file_data_t));
    if (!open_text_file("test.txt", f))
    {

        while (!read_word(f))
        {
            add_ele(words, f);
        }
        printf("File read successfully!\n");
    }
    else
    {
        printf("------------------\n");
        printf("Error opening file!\n");
        printf("------------------\n");
    }
    get_info(words, "ok");
    close_text_file(f);
    /*   w_ele *w = words[473];
    printf("ok\n");
    printf("%s", w->word);*/
    //printf("Contador de palavras: %ld     Indice do inicio da palavra atual(em todo o texto): %ld    Indice do final da palavra atual(em todo o texto): %ld\n", f->word_num, f->word_pos, f->current_pos);
    //Falta método para verificar se o elemento da linkedlist está vazio!
}
