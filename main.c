#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define minus_inf -1000000000 // a very small integer
#define plus_inf +1000000000  // a very large integer
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

typedef struct link_ele
{
    bool valid; // para verificar se é nula ou não
    char word[64];
    long count;            //contador de palavras
    long tdist;            //total da soma das distâncias (em relação ao contador de palavras geral)
    long tdistp;           //total da soma das distâncias (em relação à posição dos indices)
    long dmin;             //distância mínima (em relação ao contador de palavras geral)
    long dmax;             //distância máxima (em relação ao contador de palavras geral)
    long dminp;            //distância mínima (em relação à posição dos indices)
    long dmaxp;            //distância máxima (em relação à posição dos indices)
    long last;             //última posição (em relação ao contador de palavras geral)
    long first;            //primeira posição (em relação ao contador de palavras geral)
    long lastp;            //última posição (em relação à posição dos indices)
    long firstp;           //primeira posição (em relação à posição dos indices)
    struct link_ele *next; // Pointer para a próxima palavra
} link_ele;

typedef struct tree_node
{
    struct tree_node *left;   // pointer to the left branch (a sub-tree)
    struct tree_node *right;  // pointer to the right branch (a sub-tree)
    struct tree_node *parent; // optional
    char word[64];
    bool valid;
    long count;  //contador de palavras
    long tdist;  //total da soma das distâncias (em relação ao contador de palavras geral)
    long tdistp; //total da soma das distâncias (em relação à posição dos indices)
    long dmin;   //distância mínima (em relação ao contador de palavras geral)
    long dmax;   //distância máxima (em relação ao contador de palavras geral)
    long dminp;  //distância mínima (em relação à posição dos indices)
    long dmaxp;  //distância máxima (em relação à posição dos indices)
    long last;   //última posição (em relação ao contador de palavras geral)
    long first;  //primeira posição (em relação ao contador de palavras geral)
    long lastp;  //última posição (em relação à posição dos indices)
    long firstp; //primeira posição (em relação à posição dos indices)
    long data;   // the data item (we use an int here, but it can be anything)
} tree_node;

void add_node(tree_node *actual, file_data_t *f)
{
    bool found = false;
    tree_node *parent = NULL;

    while (actual->valid == true)
    {
        if (strcmp(actual->word, f->word) == 0) //se este for o correspondente à palavra lida
        {
            long tempdist = f->word_num - actual->last;
            long tempdistp = f->current_pos - actual->lastp;
            actual->tdist = actual->tdist + tempdist;
            actual->tdistp = actual->tdistp + tempdistp;
            if (tempdist < actual->dmin)
                actual->dmin = tempdist;
            if (tempdist > actual->dmax)
                actual->dmax = tempdist;
            if (tempdistp < actual->dminp)
                actual->dminp = tempdistp;
            if (tempdistp > actual->dmaxp)
                actual->dmaxp = tempdistp;
            actual->count++;
            actual->last = f->word_num;
            actual->lastp = f->current_pos;
            found = true;
            break;
        }
        else
        {
            parent = actual;
            actual = (strcmp(f->word, actual->word ) < 0) ? ((actual)->left) : ((actual)->right); // select branch }
        }
    }
    //Se não existir aquela palavra
    if (!found)
    {
        if(actual->parent!=NULL){
            tree_node *actual = malloc(sizeof(tree_node));
        }
        strcpy(actual->word, f->word);
        actual->valid = true;
        actual->first = f->word_num;
        actual->count++;
        actual->last = f->word_num;
        actual->lastp = f->current_pos;
        actual->firstp = f->word_pos;
        actual->parent=parent;
    }
}

unsigned int hash_function(const char *str, unsigned int s)
{ // for 32-bit unsigned integers, s should be smaller that 16777216u
    unsigned int h;
    for (h = 0u; *str != '\0'; str++)
        h = (256u * h + (0xFFu & (unsigned int)*str)) % s;
    return h;
}

void add_ele(link_ele **words, file_data_t *f, int size)
{
    int index = hash_function(f->word, size);
    link_ele *actual = words[index];
    if (actual->valid == true) //se já existir um elemento na linked list daquele index
    {
        if (strcmp(actual->word, f->word) == 0)
        { // se for igual
            long tempdist = f->word_num - actual->last;
            long tempdistp = f->current_pos - actual->lastp;
            actual->tdist = actual->tdist + tempdist;
            actual->tdistp = actual->tdistp + tempdistp;
            if (tempdist < actual->dmin)
                actual->dmin = tempdist;
            if (tempdist > actual->dmax)
                actual->dmax = tempdist;
            if (tempdistp < actual->dminp)
                actual->dminp = tempdistp;
            if (tempdistp > actual->dmaxp)
                actual->dmaxp = tempdistp;
            actual->count++;
            actual->last = f->word_num;
            actual->lastp = f->current_pos;
        }
        else
        { //se não for igual vou percorrer os next elements para ver se há algum igual
            bool found = false;
            while (actual->next != NULL)
            {
                actual = actual->next;
                if (strcmp(actual->word, f->word) == 0)
                { // se for igual
                    long tempdist = f->word_num - actual->last;
                    long tempdistp = f->current_pos - actual->lastp;
                    actual->tdist = actual->tdist + tempdist;
                    actual->tdistp = actual->tdistp + tempdistp;
                    if (tempdist < actual->dmin)
                        actual->dmin = tempdist;
                    if (tempdist > actual->dmax)
                        actual->dmax = tempdist;
                    if (tempdistp < actual->dminp)
                        actual->dminp = tempdistp;
                    if (tempdistp > actual->dmaxp)
                        actual->dmaxp = tempdistp;
                    actual->count++;
                    actual->last = f->word_num;
                    actual->lastp = f->current_pos;
                    found = true;
                    break;
                }
            }
            if (!found) //verificar que nenhum foi encontrado
            {
                link_ele *temp = malloc(sizeof(link_ele));
                strcpy(temp->word, f->word);
                temp->valid = true;
                temp->first = f->word_num;
                temp->count = 1;
                temp->last = f->word_num;
                temp->lastp = f->current_pos;
                temp->firstp = f->word_pos;
                temp->next = NULL;
                temp->dmin = plus_inf;   //indicativo que a distância ainda não foi alterada
                temp->dmax = minus_inf;  //indicativo que a distância ainda não foi alterada
                temp->dminp = plus_inf;  //indicativo que a distância ainda não foi alterada
                temp->dmaxp = minus_inf; //indicativo que a distância ainda não foi alterada
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

void get_info(link_ele **words, int size)
{
    char name[64];
    printf("Insert word for info: ");
    scanf("%[^\n]", name);
    //get info about a word
    int index = hash_function(name, size);
    link_ele *actual = words[index];
    while (strcmp(actual->word, name) != 0 && actual->next != NULL)
    {
        actual = actual->next;
    }
    if (strcmp(actual->word, name) == 0) //Double check, pode vir a ser retirado
    {
        printf("\nInformation about word '%s'\n", name);
        printf("\nCount: %ld\n", actual->count);
        printf("\nPosition (related to the index position of all the text):\n");
        printf("First: %ld\n", actual->first);
        printf("Last: %ld\n", actual->last);
        printf("\nPosition (related to the distinct word counter):\n");
        printf("First: %ld\n", actual->firstp);
        printf("Last: %ld\n", actual->lastp);
        printf("\nDistances beetween consecutive occurrences (related to the index position):\n");
        printf("Smallest: %ld\n", actual->dminp);
        printf("Average: %ld\n", (actual->tdistp) / (actual->count - 1)); //-1 porque quero o numero de distancias e não de palavras
        printf("Largest: %ld\n", actual->dmaxp);
        printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
        printf("Smallest: %ld\n", actual->dmin);
        printf("Average: %ld\n", (actual->tdist) / (actual->count - 1));
        printf("Largest: %ld\n\n", actual->dmax);
    }
    else
    {
        printf("Word %s not found!\n", name);
        exit(0);
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

void usage(char *argv[])
{
    printf("Unknown option\n");
    printf("\nUsage: %s -h -t\n\n", argv[0]);
    printf("-h Initialize program using HashTable\n");
    printf("-t Initialize program using Ordered Binary Tree\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h')
    {
        printf("Initializing HashTable\n");
        int s_hash = 500;
        link_ele *words[s_hash];
        for (int s = 0; s < s_hash; s++)
        {
            link_ele *elem = malloc(sizeof(link_ele));
            elem->valid = false;
            elem->next = NULL;
            elem->count = 0;
            elem->dmin = plus_inf;   //indicativo que a distância ainda não foi alterada
            elem->dmax = minus_inf;  //indicativo que a distância ainda não foi alterada
            elem->dminp = plus_inf;  //indicativo que a distância ainda não foi alterada
            elem->dmaxp = minus_inf; //indicativo que a distância ainda não foi alterada
            words[s] = elem;
        }
        file_data_t *f = malloc(sizeof(file_data_t));
        if (!open_text_file("test.txt", f))
        {
            while (!read_word(f))
            {
                add_ele(words, f, s_hash);
            }
            printf("File read successfully!\n");
        }
        else
        {
            printf("------------------\n");
            printf("Error opening file!\n");
            printf("------------------\n");
        }
        get_info(words, s_hash);
        close_text_file(f);
    }
    else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 't')
    {
        tree_node *root = malloc(sizeof(tree_node));
        //define root
        root->left = NULL;
        root->right = NULL;
        root->parent = NULL;
        root->valid = false;
        root->count = 0;
        root->dmin = plus_inf;   //indicativo que a distância ainda não foi alterada
        root->dmax = minus_inf;  //indicativo que a distância ainda não foi alterada
        root->dminp = plus_inf;  //indicativo que a distância ainda não foi alterada
        root->dmaxp = minus_inf; //indicativo que a distância ainda não foi alterada
        printf("Initializing Ordered Binary Tree\n");
        file_data_t *f = malloc(sizeof(file_data_t));
        if (!open_text_file("test.txt", f))
        {
            while (!read_word(f))
            {
                add_node(root, f);
            }
            printf("File read successfully!\n");
        }
        else
        {
            printf("------------------\n");
            printf("Error opening file!\n");
            printf("------------------\n");
        }
    }
    else
    {
        usage(argv);
    }

    /*   w_ele *w = words[473];
    printf("ok\n");
    printf("%s", w->word);*/
    //printf("Contador de palavras: %ld     Indice do inicio da palavra atual(em todo o texto): %ld    Indice do final da palavra atual(em todo o texto): %ld\n", f->word_num, f->word_pos, f->current_pos);
    //Falta método para verificar se o elemento da linkedlist está vazio!
}
