#include <string.h>
#include <iostream>
#include <fstream>


//  wc SherlockHolmes.txt  shell command count words by space
using namespace std;

int read(const char* nfile)
{
    ifstream file;
    file.open(nfile);
    char output[100];
    int count;
    if (file.is_open())
    {
        count = 0;
        while (!file.eof()) //eof - end of file
        {
            file >> output;
            cout << output << " " << count << endl;
            count++;
        }
        file.close();
    }else{
        perror("Erro ao ler o ficheiro!\n");
        return EXIT_FAILURE;
    }
    
    cout << count << endl;
    return 0;
}

int main(int argc, char const *argv[])
{
    read("SherlockHolmes.txt");
    return 0;
}
