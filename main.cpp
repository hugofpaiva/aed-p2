#include <string.h>
#include <iostream>
#include <fstream>

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
            cout << output << endl;
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

    cout << "Hello World!" << endl;
    read("SherlockHolmes.txt");
    return 0;
}
