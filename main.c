#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define minus_inf -1000000000 // a very small integer
#define plus_inf +1000000000  // a very large integer
int count_array;              // array size being used

int count_diff; // different word counter

static double cpu_time; // time counter

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// code to measure the elapsed time used by a program fragment (an almost copy of elapsed_time.h)
//
// use as follows:
//
//   (void)elapsed_time();
//   // put your code to be time measured here
//   dt = elapsed_time();
//   // put morecode to be time measured here
//   dt = elapsed_time();
//
// elapsed_time() measures the CPU time between consecutive calls
//

#if defined(__linux__) || defined(__APPLE__)

//
// GNU/Linux and MacOS code to measure elapsed time
//

#include <time.h>

static double elapsed_time(void)
{
    static struct timespec last_time, current_time;

    last_time = current_time;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time) != 0)
        return -1.0; // clock_gettime() failed!!!
    return ((double)current_time.tv_sec - (double)last_time.tv_sec) + 1.0e-9 * ((double)current_time.tv_nsec - (double)last_time.tv_nsec);
}

#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)

//
// Microsoft Windows code to measure elapsed time
//

#include <windows.h>

static double elapsed_time(void)
{
    static LARGE_INTEGER frequency, last_time, current_time;
    static int first_time = 1;

    if (first_time != 0)
    {
        QueryPerformanceFrequency(&frequency);
        first_time = 0;
    }
    last_time = current_time;
    QueryPerformanceCounter(&current_time);
    return (double)(current_time.QuadPart - last_time.QuadPart) / (double)frequency.QuadPart;
}

#endif

static void reset_time(void)
{
    printf("%s\n", "Time reseted");
    cpu_time = 0.0;
}

typedef struct file_data
{                  // public data
    long word_pos; // zero-based
    long word_num; // zero-based
    char word[64];
    // private data
    FILE *fp;
    long current_pos; // zero-based
} file_data_t;

typedef struct link_ele
{
    char word[64];
    long count;            // word counter
    long tdist;            // total sum of distances (in relation to the general word counter)
    long tdistp;           // total sum of distances (in relation to the index position)
    long dmin;             // min distance (in relation to the general word counter)
    long dmax;             // max distance (in relation to the general word counter)
    long dminp;            // min distance (in relation to the index position)
    long dmaxp;            // max distance (in relation to the index position)
    long last;             // last position (in relation to the general word counter)
    long first;            // first position (in relation to the general word counter)
    long lastp;            // last position (in relation to the index position)
    long firstp;           // first position (in relation to the index position)
    struct link_ele *next; // next word pointer
} link_ele;

typedef struct tree_node
{
    struct tree_node *left;   // pointer to the left branch (a sub-tree)
    struct tree_node *right;  // pointer to the right branch (a sub-tree)
    struct tree_node *parent; // optional
    char word[64];
    long count;  // word counter
    long tdist;  // total sum of distances (in relation to the general word counter)
    long tdistp; // total sum of distances (in relation to the index position)
    long dmin;   // min distance (in relation to the general word counter)
    long dmax;   // max distance (in relation to the general word counter)
    long dminp;  // min distance (in relation to the index position)
    long dmaxp;  // max distance (in relation to the index position)
    long last;   // last position (in relation to the general word counter)
    long first;  // first position (in relation to the general word counter)
    long lastp;  // last position (in relation to the index position)
    long firstp; // first position (in relation to the index position)
    long data;   // the data item (we use an int here, but it can be anything)
} tree_node;

unsigned int hash_function(const char *str, unsigned int s)
{ // for 32-bit unsigned integers, s should be smaller that 16777216u
    unsigned int h;
    for (h = 0u; *str != '\0'; str++)
        h = (256u * h + (0xFFu & (unsigned int)*str)) % s;
    return h;
}

void add_node(tree_node **words, file_data_t *f, int size)
{
    int index = hash_function(f->word, size);
    tree_node *actual = words[index];
    if (actual != NULL) // if there is already an element in the ordered binary tree
    {
        if (strcmp(actual->word, f->word) == 0)
        { // if that element is the same
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
        { // if the element is not the same we travel through the next elements to check if there is any equal
            bool found = false;
            while (actual != NULL) // While word not found and children not null
            {
                if (strcmp(f->word, actual->word) < 0 && actual->left != NULL) // actual word is smaller
                    actual = actual->left;

                else if (strcmp(f->word, actual->word) > 0 && actual->right != NULL) // actual word is bigger
                    actual = actual->right;

                else if (strcmp(f->word, actual->word) == 0)
                { // if equal
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
                    break;
            }

            if (!found) // check that no elem was found
            {
                tree_node *temp = malloc(sizeof(tree_node));
                strcpy(temp->word, f->word);
                temp->first = f->word_num;
                temp->count = 1;
                temp->last = f->word_num;
                temp->lastp = f->current_pos;
                temp->firstp = f->word_pos;
                temp->parent = actual;
                temp->dmin = plus_inf;   // dist not altered
                temp->dmax = minus_inf;  // dist not altered
                temp->dminp = plus_inf;  // dist not altered
                temp->dmaxp = minus_inf; // dist not altered
                if (strcmp(f->word, actual->word) < 0)
                { // current word is the smallest in the node
                    actual->left = temp;
                }
                else if (strcmp(f->word, actual->word) > 0)
                { // current word is the biggest in the node
                    actual->right = temp;
                }
            }
        }
    }
    else
    { // New tree root
        tree_node *new = malloc(sizeof(tree_node));
        strcpy(new->word, f->word);
        new->parent = NULL;
        new->left = NULL;
        new->right = NULL;
        new->count = 0;
        new->dmin = plus_inf;   // dist not altered
        new->dmax = minus_inf;  // dist not altered
        new->dminp = plus_inf;  // dist not altered
        new->dmaxp = minus_inf; // dist not altered
        new->first = f->word_num;
        new->count++;
        new->last = f->word_num;
        new->lastp = f->current_pos;
        new->firstp = f->word_pos;
        words[index] = new;
    }
}

void add_ele(link_ele **words, file_data_t *f, int size)
{
    int index = hash_function(f->word, size);
    link_ele *actual = words[index];
    if (actual != NULL) // if an element in the list already exists in that index
    {
        if (strcmp(actual->word, f->word) == 0)
        { // if equal
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
        { // if not equal it is needed to run over all the elements
            bool found = false;
            while (actual->next != NULL)
            {
                actual = actual->next;
                if (strcmp(actual->word, f->word) == 0)
                { // if equal
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
            if (!found) // not found verification
            {
                link_ele *temp = malloc(sizeof(link_ele));
                strcpy(temp->word, f->word);
                temp->first = f->word_num;
                temp->count = 1;
                temp->last = f->word_num;
                temp->lastp = f->current_pos;
                temp->firstp = f->word_pos;
                temp->next = NULL;
                temp->dmin = plus_inf;   // dist not altered
                temp->dmax = minus_inf;  // dist not altered
                temp->dminp = plus_inf;  // dist not altered
                temp->dmaxp = minus_inf; // dist not altered
                actual->next = temp;
            }
        }
    }
    else
    { // New Start of a linked list
        count_array++;
        link_ele *new = malloc(sizeof(link_ele));
        strcpy(new->word, f->word);
        new->next = NULL;
        new->count = 0;
        new->dmin = plus_inf;   // dist not altered
        new->dmax = minus_inf;  // dist not altered
        new->dminp = plus_inf;  // dist not altered
        new->dmaxp = minus_inf; // dist not altered
        new->first = f->word_num;
        new->count++;
        new->last = f->word_num;
        new->lastp = f->current_pos;
        new->firstp = f->word_pos;
        words[index] = new;
    }
}

void add_ele_resize(link_ele **words, link_ele *f, int size)
{
    int index = hash_function(f->word, size);
    link_ele *actual = words[index];
    if (actual != NULL)
    {
        while (actual->next != NULL)
        {
            actual = actual->next;
        }
        actual = actual->next;
        link_ele *temp = malloc(sizeof(link_ele));
        strcpy(temp->word, f->word);
        temp->next = NULL;
        temp->count = f->count;
        temp->dmin = f->dmin;
        temp->dmax = f->dmax;
        temp->dminp = f->dminp;
        temp->dmaxp = f->dmaxp;
        temp->first = f->first;
        temp->count = f->count;
        temp->last = f->last;
        temp->lastp = f->lastp;
        temp->firstp = f->firstp;

    }
    else
    {
        link_ele *new = malloc(sizeof(link_ele));
        strcpy(new->word, f->word);
        new->next = NULL;
        new->count = f->count;
        new->dmin = f->dmin;
        new->dmax = f->dmax;
        new->dminp = f->dminp;
        new->dmaxp = f->dmaxp;
        new->first = f->first;
        new->count = f->count;
        new->last = f->last;
        new->lastp = f->lastp;
        new->firstp = f->firstp;
        words[index] = new;
    }

    char word[64];
    long count;            // word counter
    long tdist;            // total sum of distances (in relation to the general word counter)
    long tdistp;           // total sum of distances (in relation to the index position)
    long dmin;             // min distance (in relation to the general word counter)
    long dmax;             // max distance (in relation to the general word counter)
    long dminp;            // min distance (in relation to the index position)
    long dmaxp;            // max distance (in relation to the index position)
    long last;             // last position (in relation to the general word counter)
    long first;            // first position (in relation to the general word counter)
    long lastp;            // last position (in relation to the index position)
    long firstp;           // first position (in relation to the index position)
    struct link_ele *next; // next word pointer
}

link_ele **resize_link(link_ele **words, int *size)
{
    int newsize = 2 * (*size);
    link_ele **words_temp = (link_ele *)calloc(newsize, sizeof(link_ele *));
    for (int i = 0; i < (*size); i++)
    {
        if (words[i] != NULL)
        {
            link_ele *actual = words[i];
            while (actual->next != NULL)
            {
                add_ele_resize(words_temp, actual, newsize);
                actual = actual->next;
            }
        }
    }
    *size = 2 * (*size);
    return words_temp;
}

void get_info_link(link_ele **words, int size)
{
    char name[64];
    printf("Insert word for info: ");
    scanf("%[^\n]", name);
    fflush(stdin);
    //get info about a word
    int index = hash_function(name, size);
    link_ele *actual = words[index];
    bool found = false;
    if (actual != NULL)
    {
        while (actual != NULL)
        {
            if (strcmp(actual->word, name) == 0)
            {
                printf("\nInformation about word '%s'\n", actual->word);
                printf("\nCount: %ld\n", actual->count);
                printf("\nPosition (related to the index position of all the text):\n");
                printf("First: %ld\n", actual->first);
                printf("Last: %ld\n", actual->last);
                printf("\nPosition (related to the distinct word counter):\n");
                printf("First: %ld\n", actual->firstp);
                printf("Last: %ld\n", actual->lastp);
                if (actual->count > 1)
                {
                    printf("\nDistances beetween consecutive occurrences (related to the index position of all the text):\n");
                    printf("Smallest: %ld\n", actual->dminp);
                    printf("Average: %.2f\n", (float)(actual->tdistp) / (actual->count - 1)); // -1 number of dist and not words
                    printf("Largest: %ld\n", actual->dmaxp);
                    printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
                    printf("Smallest: %ld\n", actual->dmin);
                    printf("Average: %.2f\n", (float)(actual->tdist) / (actual->count - 1));
                    printf("Largest: %ld\n\n", actual->dmax);
                }
                else
                {
                    printf("\n No distances stats available.\n\n");
                }
                found = true;
                break;
            }
            actual = actual->next;
        }
    }

    if (!found)
    {
        printf("Word %s not found!\n", name);
        exit(0);
    }
}

void get_info_node(tree_node **words, int size)
{
    char name[64];
    printf("Insert word for info: ");
    scanf("%[^\n]", name);
    fflush(stdin);
    //get info about a word
    int index = hash_function(name, size);
    tree_node *actual = words[index];
    bool found = false;
    if (actual != NULL)
    {
        while (actual != NULL)
        {
            if (strcmp(name, actual->word) < 0 && actual->left != NULL) // word smaller than the node
                actual = actual->left;

            else if (strcmp(name, actual->word) > 0 && actual->right != NULL) // word bigger than the node
                actual = actual->right;

            else
            { // if equal
                printf("\nInformation about word '%s'\n", actual->word);
                printf("\nCount: %ld\n", actual->count);
                printf("\nPosition (related to the index position of all the text):\n");
                printf("First: %ld\n", actual->first);
                printf("Last: %ld\n", actual->last);
                printf("\nPosition (related to the distinct word counter):\n");
                printf("First: %ld\n", actual->firstp);
                printf("Last: %ld\n", actual->lastp);
                if (actual->count > 1)
                {
                    printf("\nDistances beetween consecutive occurrences (related to the index position of all the text):\n");
                    printf("Smallest: %ld\n", actual->dminp);
                    printf("Average: %.2f\n", (float)(actual->tdistp) / (actual->count - 1)); // -1 because number of distances and not words
                    printf("Largest: %ld\n", actual->dmaxp);
                    printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
                    printf("Smallest: %ld\n", actual->dmin);
                    printf("Average: %.2f\n", (float)(actual->tdist) / (actual->count - 1));
                    printf("Largest: %ld\n\n", actual->dmax);
                }
                else
                {
                    printf("\n No distances stats available.\n\n");
                }
                found = true;
                break;
            }
        }
    }
    if (!found)
    {
        printf("Word %s not found!\n", name);
        exit(0);
    }
}

int get_info_node_all(tree_node **words, int size)
{
    int c_stored = 0;
    count_diff = 0;
    bool found = false;
    for (int i = 0; i < size; i++)
    {
        tree_node *actual = words[i];
        tree_node *pre;
        if (actual != NULL)
        {
            while (actual != NULL)
            {
                if (actual->left == NULL)
                {
                    c_stored += actual->count;
                    count_diff++;
                    printf("\nInformation about word '%s'\n", actual->word);
                    printf("\nCount: %ld\n", actual->count);
                    printf("\nPosition (related to the index position of all the text):\n");
                    printf("First: %ld\n", actual->first);
                    printf("Last: %ld\n", actual->last);
                    printf("\nPosition (related to the distinct word counter):\n");
                    printf("First: %ld\n", actual->firstp);
                    printf("Last: %ld\n", actual->lastp);
                    if (actual->count > 1)
                    {
                        printf("\nDistances beetween consecutive occurrences (related to the index position of all the text):\n");
                        printf("Smallest: %ld\n", actual->dminp);
                        printf("Average: %.2f\n", (float)(actual->tdistp) / (actual->count - 1)); // -1 because number of distances and not words
                        printf("Largest: %ld\n", actual->dmaxp);
                        printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
                        printf("Smallest: %ld\n", actual->dmin);
                        printf("Average: %.2f\n", (float)(actual->tdist) / (actual->count - 1));
                        printf("Largest: %ld\n\n", actual->dmax);
                    }
                    else
                    {
                        printf("\n No distances stats available.\n\n");
                    }
                    found = true;
                    actual = actual->right;
                }
                else
                {
                    /* Find the inorder predecessor of current */
                    pre = actual->left;
                    while (pre->right != NULL && pre->right != actual)
                        pre = pre->right;

                    /* Make current as the right child of its inorder  
               predecessor */
                    if (pre->right == NULL)
                    {
                        pre->right = actual;
                        actual = actual->left;
                    }

                    /* Revert the changes made in the 'if' part to restore  
               the original tree i.e., fix the right child 
               of predecessor */
                    else
                    {
                        pre->right = NULL;
                        c_stored += actual->count;
                        count_diff++;
                        printf("\nInformation about word '%s'\n", actual->word);
                        printf("\nCount: %ld\n", actual->count);
                        printf("\nPosition (related to the index position of all the text):\n");
                        printf("First: %ld\n", actual->first);
                        printf("Last: %ld\n", actual->last);
                        printf("\nPosition (related to the distinct word counter):\n");
                        printf("First: %ld\n", actual->firstp);
                        printf("Last: %ld\n", actual->lastp);
                        if (actual->count > 1)
                        {
                            printf("\nDistances beetween consecutive occurrences (related to the index position of all the text):\n");
                            printf("Smallest: %ld\n", actual->dminp);
                            printf("Average: %.2f\n", (float)(actual->tdistp) / (actual->count - 1)); // -1 because number of distances and not words
                            printf("Largest: %ld\n", actual->dmaxp);
                            printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
                            printf("Smallest: %ld\n", actual->dmin);
                            printf("Average: %.2f\n", (float)(actual->tdist) / (actual->count - 1));
                            printf("Largest: %ld\n\n", actual->dmax);
                        }
                        else
                        {
                            printf("\n No distances stats available.\n\n");
                        }
                        found = true;
                        actual = actual->right;
                    }
                }
            }
        }
    }
    if (!found)
    {
        printf("No words found!\n");
        exit(0);
    }
    return c_stored;
}

int get_info_link_all(link_ele **words, int size, bool all)
{
    bool found = false;
    int c_st = 0;
    count_diff = 0;
    if (all == true)
        goto all;
    char name[64];
    printf("Insert word, or start of it, for info (empty for all): ");
    if (gets(name) != NULL)
    {
        int s_name = strlen(name);
        for (int i = 0; i < size; i++)
        {
            link_ele *actual = words[i];
            while (actual != NULL)
            {
                if (strncmp(name, actual->word, s_name) == 0)
                {
                    found = true;
                    printf("\nInformation about word '%s'\n", actual->word);
                    printf("\nCount: %ld\n", actual->count);
                    printf("\nPosition (related to the index position of all the text):\n");
                    printf("First: %ld\n", actual->first);
                    printf("Last: %ld\n", actual->last);
                    printf("\nPosition (related to the distinct word counter):\n");
                    printf("First: %ld\n", actual->firstp);
                    printf("Last: %ld\n", actual->lastp);
                    if (actual->count > 1)
                    {
                        printf("\nDistances beetween consecutive occurrences (related to the index position of all the text):\n");
                        printf("Smallest: %ld\n", actual->dminp);
                        printf("Average: %.2f\n", (float)(actual->tdistp) / (actual->count - 1)); // -1 because number of distances and not words
                        printf("Largest: %ld\n", actual->dmaxp);
                        printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
                        printf("Smallest: %ld\n", actual->dmin);
                        printf("Average: %.2f\n", (float)(actual->tdist) / (actual->count - 1));
                        printf("Largest: %ld\n\n", actual->dmax);
                    }
                    else
                    {
                        printf("\nNo distances stats available.\n\n");
                    }
                }
                actual = actual->next;
            }
        }
    }
    else
    {
    all:
        for (int i = 0; i < size; i++)
        {
            link_ele *actual = words[i];
            while (actual != NULL)
            {
                found = true;
                c_st += actual->count;
                count_diff++;
                printf("\nInformation about word '%s'\n", actual->word);
                printf("\nCount: %ld\n", actual->count);
                printf("\nPosition (related to the index position of all the text):\n");
                printf("First: %ld\n", actual->first);
                printf("Last: %ld\n", actual->last);
                printf("\nPosition (related to the distinct word counter):\n");
                printf("First: %ld\n", actual->firstp);
                printf("Last: %ld\n", actual->lastp);
                if (actual->count > 1)
                {
                    printf("\nDistances beetween consecutive occurrences (related to the index position of all the text):\n");
                    printf("Smallest: %ld\n", actual->dminp);
                    printf("Average: %2f\n", (float)(actual->tdistp) / (actual->count - 1)); // -1 because number of distances and not words
                    printf("Largest: %ld\n", actual->dmaxp);
                    printf("\nDistances beetween consecutive occurrences (related to the distinct word counter):\n");
                    printf("Smallest: %ld\n", actual->dmin);
                    printf("Average: %2f\n", (float)(actual->tdist) / (actual->count - 1));
                    printf("Largest: %ld\n\n", actual->dmax);
                }
                else
                {
                    printf("\nNo distances stats available.\n\n");
                }

                actual = actual->next;
            }
        }
    }
    if (!found)
    {
        printf("No words found!\n");
        exit(0);
    }
    fflush(stdin);
    return c_st;
}

int open_text_file(char *file_name, file_data_t *fd)
{
    fd->fp = fopen(file_name, "rb");

    if (fd->fp == NULL)
        return -1;
    fd->word_pos = 0;
    fd->word_num = 0;
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
    printf("\nUsage: %s -l -b -t\n\n", argv[0]);
    printf("-l Initialize program using HashTable with Linked List\n");
    printf("-b Initialize program using HashTable with Ordered Binary Tree\n");
    printf("-t Initialize program for Tests\n");

    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'l')
    {
        count_array = 0;
        printf("Initializing HashTable with Linked List\n");
        int s_hash = 500;
        link_ele **words = (link_ele *)calloc(s_hash, sizeof(link_ele *)); // creates and announce them as zero (null)
        file_data_t *f = malloc(sizeof(file_data_t));
        char file[64];
        printf("Insert filename for stats (e.g.'SherlockHolmes.txt'): ");
        scanf("%[^\n]", file);
        fflush(stdin);
        if (!open_text_file(file, f))
        {
            while (!read_word(f))
            {
                if ((double)count_array / s_hash >= 0.8)
                {
                    words = resize_link(words, &s_hash);
                    count_array = 0;
                }
                add_ele(words, f, s_hash);
            }
            printf("File read successfully!\n");
            close_text_file(f);
        }
        else
        {
            printf("------------------\n");
            printf("Error opening file!\n");
            printf("------------------\n");
            exit(0);
        }
        printf("\n1 - Search for a certain word stats\n2 - Search with a piece of a word or list all words stats\n");
        char option[5];
        printf("\nOption: ");
        scanf("%[^\n]", option);
        fflush(stdin);
        if (strcmp(option, "1") == 0)
            get_info_link(words, s_hash);
        else if (strcmp(option, "2") == 0)
            get_info_link_all(words, s_hash, false);
        else
        {
            printf("Invalid option");
            exit(0);
        }
    }
    else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'b')
    {
        printf("Initializing HashTable with Ordered Binary Tree\n");
        int s_hash = 500;
        tree_node **words = (tree_node *)calloc(s_hash, sizeof(tree_node *)); // creates and announce them as zero (null)
        file_data_t *f = malloc(sizeof(file_data_t));
        char file[64];
        printf("Insert filename for stats (e.g.'SherlockHolmes.txt'): ");
        scanf("%[^\n]", file);
        fflush(stdin);
        if (!open_text_file(file, f))
        {
            while (!read_word(f))
            {
                add_node(words, f, s_hash);
            }
            printf("File read successfully!\n");
            close_text_file(f);
        }
        else
        {
            printf("------------------\n");
            printf("Error opening file!\n");
            printf("------------------\n");
            exit(0);
        }
        printf("\n1 - Search for a certain word stats\n2 - Show all words stats\n");
        char option[5];
        printf("\nOption: ");
        scanf("%[^\n]", option);
        fflush(stdin);
        if (strcmp(option, "1") == 0)
            get_info_node(words, s_hash);
        else if (strcmp(option, "2") == 0)
            get_info_node_all(words, s_hash);
        else
        {
            printf("Invalid option");
            exit(0);
        }
    }
    else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 't')
    {
        char file[64];
        printf("Insert filename for stats (e.g.'SherlockHolmes.txt'): ");
        scanf("%[^\n]", file);
        fflush(stdin);

        printf("Initializing HashTable with Ordered Binary Tree\n");
        reset_time();
        int s_hash = 500;
        int count_stored = 0;
        (void)elapsed_time();
        tree_node **words = (tree_node *)calloc(s_hash, sizeof(tree_node *)); // creates and anounce them as zero (null)
        file_data_t *f = malloc(sizeof(file_data_t));

        if (!open_text_file(file, f))
        {
            while (!read_word(f))
            {
                add_node(words, f, s_hash);
            }
            printf("File read successfully!\n");
            close_text_file(f);
        }
        else
        {
            printf("------------------\n");
            printf("Error opening file!\n");
            printf("------------------\n");
            exit(0);
        }

        cpu_time = elapsed_time();
        printf("%s %.6f s \n", "File read! Elapsed Time! - Reading", cpu_time);

        FILE *fw = fopen("results.txt", "a+");

        if (fw == NULL)
        {
            printf("Erro a abrir o ficheiro escrita!\n");
            exit(1);
        }
        else
        {
            printf("%s\n", "Aberto ficheiro results.txt");
            fprintf(fw, "Filename \t %s \n", file);
            fprintf(fw, "HashTable OBT Reading Time \t %.6f \n", cpu_time);
        }

        reset_time();

        printf("\nPrinting all words stored...\n");
        (void)elapsed_time();
        usleep(5000000);
        count_stored = get_info_node_all(words, s_hash);
        printf("\n ------------------------------------------------------------------ \n");
        printf("\n Words read - %ld\n", f->word_num);
        printf(" Words stored - %d\n", count_stored);
        printf("%s %d \n", "Number of different word", count_diff);
        cpu_time = elapsed_time();
        printf("%s %.6f s \n", "Tabel Traveled and Printed! Elapsed Time!", cpu_time);

        if (fw == NULL)
        {
            printf("Erro a abrir o ficheiro results!\n");
            exit(1);
        }
        else
        {
            fprintf(fw, "HashTable OBT Words Read \t %ld \n", f->word_num);
            fprintf(fw, "HashTable OBT Words Stored \t %d \n", count_stored);
            fprintf(fw, "%s %d \n", "Number of different word", count_diff);
            fprintf(fw, "HashTable OBT Time Travel Print \t %.6f \n", cpu_time);
        }

        free(words);
        free(f);
        //--------------------------------------------------------------//

        printf("\n....................................................................\n");

        printf("\nInitializing HashTable with Linked List\n");
        s_hash = 500;
        count_array = 0;
        count_stored = 0;
        reset_time();
        (void)elapsed_time();
        link_ele **words1 = (link_ele *)calloc(s_hash, sizeof(link_ele *)); // creates and anounce them as zero (null)
        file_data_t *f1 = malloc(sizeof(file_data_t));

        if (!open_text_file(file, f1))
        {
            while (!read_word(f1))
            {
                if ((double)count_array / s_hash >= 0.8)
                {
                    words1 = resize_link(words1, &s_hash);
                    count_array = 0;
                }
                add_ele(words1, f1, s_hash);
            }
            printf("File read successfully!\n");
            close_text_file(f);
        }
        else
        {
            printf("------------------\n");
            printf("Error opening file!\n");
            printf("------------------\n");
            exit(0);
        }

        cpu_time = elapsed_time();
        printf("%s %.6f s \n", "File read! Elapsed Time! - Reading", cpu_time);

        if (fw == NULL)
        {
            printf("Erro a abrir o ficheiro escrita!\n");
            exit(1);
        }
        else
        {
            fprintf(fw, "Filename \t %s \n", file);
            fprintf(fw, "HashTable LL Reading Time \t %.6f \n", cpu_time);
        }

        reset_time();

        printf("\nPrinting all words stored...\n");
        (void)elapsed_time();
        usleep(5000000);
        count_stored = get_info_link_all(words1, s_hash, true);
        printf("\n ------------------------------------------------------------------ \n");
        printf("\n Words read - %ld\n", f1->word_num);
        printf(" Words stored - %d\n", count_stored);
        printf("%s %d \n", "Number of different word", count_diff);
        cpu_time = elapsed_time();
        printf("%s %.6f s \n", "Tabel Traveled and Printed! Elapsed Time!", cpu_time);

        if (fw == NULL)
        {
            printf("Erro a abrir o ficheiro results!\n");
            exit(1);
        }
        else
        {
            fprintf(fw, "HashTable LL Words Read \t %ld \n", f->word_num);
            fprintf(fw, "HashTable LL Words Stored \t %d \n", count_stored);
            fprintf(fw, "%s %d \n", "Number of different word", count_diff);
            fprintf(fw, "HashTable LL Time Travel Print \t %.6f \n", cpu_time);
        }

        fclose(fw);
        free(words1);
        free(f1);
    }
    else
    {
        usage(argv);
    }
}
