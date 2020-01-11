

# AED Problem #2

The main goal of this second practical work was to implement a Hash Table where it would be stored all the different words present in a text file. Each entry of that Table should be a structure capable of storing:

- The number of occurrences of each distinct word  
- The location of the first and last occurrences of each distinct word  
- The smallest, largest, and average distances between consecutive occurrences of the same distinct word

As a mean of comparison, the Hash Table was developed using two different structures for each table entry: a Linked List and a Ordered Binary Tree. By this way, it was possible to compare times of search and execution between them.

Lastly, it was also asked that the Table should grow dynamically. This means that when the Table is nearly full, it should be resized to a larger size.

##  Getting Started
These instructions will help to compile and run developed programs on your local machine.

### Prerequisites
To compile programs, it is necessary to have a C compiler installed on your local machine, for example cc. 

### Compiling
The following command **compiles main program** (*main.c*) where <executable_filename>  will be the executable filename: 

```
cc -Wall -O2 main.c -o <executable_filename> -lm
```

## Running

**Options:**
```
-l ........................ Initialize program using HashTable with Linked Lists
    
-b ........................ Initialize program using HashTable with Ordered Binary Trees
    
-t ........................ Initialize program and runs some tests
```
## Details
All the details, including the results, can be found in the [Work Report](/relatorio/AED_Report_2.pdf).

## Authors

 - **[Hugo Paiva de Almeida](https://github.com/hugofpaiva) - 93195**
 - **[João Laranjo](https://github.com/joaolaranjo) - 91153**
 
## Grade
Evaluated with **16** out of 20.


