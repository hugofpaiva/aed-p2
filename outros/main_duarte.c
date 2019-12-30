#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

typedef struct file_data {
    // public data
    long word_pos; // zero-based
    long word_num; // zero-based
    char word[64];
    // private data
    FILE* fp;
    long current_pos; // zero-based
} file_data_t;

//Representa cada palavra distinta num determinado ficheiro
typedef struct word {
    struct word* next;
    char word[64];
    unsigned long hash;
    int first_location;
    int last_location;
    int max_dist;
    int min_dist;
    int medium_dist;
    int count;
} word_t;

typedef struct hash_table {
    unsigned int size;
    unsigned int count;
    word_t** table;
} hash_table_t;

int open_text_file(char* file_name, file_data_t* fd)
{
    fd->fp = fopen(file_name, "r");
    if (fd->fp == NULL)
        return -1;
    fd->word_pos = -1;
    fd->word_num = -1;
    ;
    fd->word[0] = '\0';
    fd->current_pos = -1;
    return 0;
}

void close_text_file(file_data_t* fd)
{
    fclose(fd->fp);
    fd->fp = NULL;
}

int read_word(file_data_t* fd)
{
    int i, c;
    // skip white spaces
    do {
        c = fgetc(fd->fp);
        if (c == EOF)
            return -1;
        fd->current_pos++;

    } while (c <= 32);
    //record word
    fd->word_pos = fd->current_pos;
    fd->word_num++;
    fd->word[0] = (char)c;
    for (i = 1; i < (int)sizeof(fd->word) - 1; i++) {
        c = fgetc(fd->fp);
        if (c == EOF)
            break;
        // end of file
        fd->current_pos++;
        if (c <= 32)
            break;
        // terminate word
        fd->word[i] = (char)c;
    }
    fd->word[i] = '\0';
    return 0;
}

unsigned long hash(unsigned char* str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return abs(hash);
}

int main(int argc, char* argv[])
{
    file_data_t* fl;
    fl = (file_data_t*)malloc(sizeof(file_data_t));

    if (open_text_file("Teste.txt", fl) == -1) {
        return EXIT_FAILURE;
    }

    // HASHTABLE INITIALIZATION
    hash_table_t* hash_table = NULL;
    hash_table = malloc(sizeof(hash_table_t));
    hash_table->table = malloc(2000 * sizeof(word_t*));
    hash_table->size = 2000;
    hash_table->count = 0;
    for (int i = 0; i < hash_table->size; i++) {
        hash_table->table[i] = NULL;
    }

    // POINTERS DECLARATION TO USE INSIDE WHILE CYCLE
    word_t* head;
    word_t* prev;
    head = (word_t*)malloc(sizeof(word_t));
    prev = (word_t*)malloc(sizeof(word_t));

    int hashcode=0;
    int word_counter=0;

    while (read_word(fl) != -1) {



        // DYNAMIC RESIZE
        if (hash_table->count >= hash_table->size / 2) {

            word_t **table, *curr, *next;
            size_t i, k;
            next = malloc(sizeof(word_t));
            curr = malloc(sizeof(word_t));
            int new_size = hash_table->size * 2;
            table = malloc(new_size * sizeof(word_t*));
            if (!table)
                return -1; // OUT OF MEMORY

            // INITIALIZE NEW TABLE TO EMPTY
            for (i = 0; i < new_size; i++) {
                table[i] = NULL;
            }

            for (i = 0; i < hash_table->size; i++) {
                // DETACH THE SINGLY LINKED LIST
                next = hash_table->table[i];
                hash_table->table[i] = NULL;
                while (next) {
                    // DETACH THE NEXT ELEMENT AS CURRENT
                    curr = next;
                    next = next->next;

                    // K IS THE INDEX OF CURR IN THE NEW TABLE
                    k = curr->hash % new_size; // o curr->hash é o resultado da word em curr ao passar pela função hash()

                    // PREPEND TO THE LINKED LIST IN TABLE[K]
                    if (curr != table[k]) {
                        curr->next = table[k];
                        table[k] = curr;
                    }
                }
            }
            // NO LONGER NEED NEXT AND CURR
            free(next);
            free(curr);

            // NO LONGER NEED THE OLD HASH TABLE
            free(hash_table->table);

            // REPLACE THE OLD HASH TABLE WITH THE NEW ONE
            hash_table->table = table;
            hash_table->size = new_size;
        }
        word_counter++;
        int flag = 0; //IF A WORD IS FOUND, THEN WE DONT NEED TO CREATE IT
        hashcode = hash(fl->word) % hash_table->size;
        head = hash_table->table[hashcode];
        if (head == NULL) { // IF THERES NOTHING IN TABLE[HASHCODE], CREATE A NEW WORD THERE
            word_t* new;
            new = (word_t*)malloc(sizeof(word_t));
            new->next = NULL;
            new->hash = hash(fl->word);
            new->first_location = fl->current_pos;
            new->last_location = fl->current_pos;
            new->max_dist = NULL;
            new->min_dist = NULL;
            new->medium_dist = 0;
            new->count = 1;
            strcpy(new->word, fl->word);
            hash_table->table[hashcode] = new;
            hash_table->count += 1;
        }
        else {
            while (head != NULL) {
                if (strcmp(head->word, fl->word) == 0) { // IF MATCH IS FOUND
                    flag = 1; // MATCH FOUND
                    int temp = head->last_location;
                    int dist = fl->current_pos - temp;
                    head->last_location = fl->current_pos;
                    if (dist > head->max_dist || head->max_dist == NULL) {
                        head->max_dist = dist;
                    }
                    if (dist < head->min_dist || head->min_dist == NULL) {
                        head->min_dist = dist;
                    }
                    head->medium_dist = head->medium_dist + (dist - head->medium_dist) / head->count;
                    head->count++;
                    break;
                }
                prev = head;
                head = head->next;
            }
            if (flag == 0) { // MATCH WAS FOUND? IF NOT, CREATE NEW WORD AND ATTACH IT TO THE LAST WORD (PREV)
                word_t* new;
                new = (word_t*)malloc(sizeof(word_t));
                new->next = NULL;
                new->hash = hash(fl->word);
                new->first_location = fl->current_pos;
                new->last_location = fl->current_pos;
                new->max_dist = NULL;
                new->min_dist = NULL;
                new->medium_dist = 0;
                new->count = 1;
                strcpy(new->word, fl->word);
                prev->next = new;
            }
        }
    }
    // HASHING DONE //

    // TESTING //
    int new_words = 0;
    word_t* word;

    // PRINT HASH TABLE
    for (int k = 0; k < hash_table->size; k++) {
        printf("%d: ", k);
        if (hash_table->table[k] == NULL) {
            printf("NULL\n");
        }
        else {
            word = hash_table->table[k];
            while (word->next != NULL) {
                printf("%s (%ld, %d, %d, %d, %d , %d) --> ", word->word, word->hash, word->first_location, word->max_dist, word->min_dist, word->medium_dist, word->count);
                word = word->next;
                new_words++;
            }
            printf("%s (%ld, %d, %d, %d, %d , %d) --> NULL\n", word->word, word->hash, word->first_location, word->max_dist, word->min_dist, word->medium_dist, word->count);
            new_words++;
        }
    }

    printf("===================================\n");
    printf("Words read: %d\n", word_counter);
    printf("Hash table count: %d\n", hash_table->count);
    printf("Hash table size: %d\n", hash_table->size);
    printf("Words inside table: %d\n", new_words);



    // SEARCH TEST //

    file_data_t* ft;
    ft = (file_data_t*)malloc(sizeof(file_data_t));
    word_t* head_test;
    head_test = (word_t*)malloc(sizeof(word_t));
    if (open_text_file("Teste.txt", ft) == -1) {
        return EXIT_FAILURE;
    }

    while (read_word(ft) != -1) {
        int hashcode_test=0;
        int flag_search = 0;

        hashcode_test = hash(fl->word) % hash_table->size;
        head_test = hash_table->table[hashcode];
        while (head_test) {
           if (strcmp(head_test->word, fl->word) == 0){
               flag_search = 1;
           }
           head_test = head_test->next;
        }

        assert(flag_search);

    }

    // WORDS INSIDE TABLE TEST //

    int test_counter=0;
    word_t* head_test_2;
    head_test_2 = (word_t*)malloc(sizeof(word_t));

    for (int k = 0; k < hash_table->size; k++){
        head_test_2 = hash_table->table[k];
        while (head_test_2) {
           test_counter++;
           head_test_2 = head_test_2->next;
        }
    }

    assert(new_words == test_counter);

    // TOP 10 MORE FREQUENT WORDS //
    char a[10][64];
    int max_test = 0;
    word_t* head_test_3;
    char word_test[64];
    head_test_3 = (word_t*)malloc(sizeof(word_t));
    for (int j = 0; j < 10; j++){
        for (int k = 0; k < hash_table->size; k++){
            head_test_3 = hash_table->table[k];
            while (head_test_3) {
                if (head_test_3->count > max_test){
                    int flagg = 1;
                    for (int l = 0; l < j; l++){
                        if (strcmp(a[l],head_test_3->word) == 0){
                            flagg = 0;
                            break;
                        }
                    }
                    if (flagg){
                        max_test = head_test_3->count;
                        memset(word_test, 0, 64);
                        strcpy(word_test, head_test_3->word);
                    }
                }
                head_test_3 = head_test_3->next;
            }
        }

        strcpy(a[j], word_test);
    }
    for (int k = 0; k < 10; k++){
        printf("%s\n", a[k]);
    }
    return 0;

}