#include <string.h>
#include <iostream>
#include <fstream>

// wc SherlockHolmes.txt  shell command count words by space
using namespace std;

int read(string nfile)
{

    // filestream variable file
    fstream file;
    string word;
    int count;

    // opening file
    file.open(nfile);

    // extracting words from the file
    while (file >> word)
    {
        // displaying content
        cout << word << count << endl;
        count++;
    }
     cout << count << endl;
    return 0;
}

int main(int argc, char const *argv[])
{
    read("SherlockHolmes.txt");
    return 0;
}
