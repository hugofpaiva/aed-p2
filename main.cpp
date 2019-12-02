#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

void read(const char* nfile)
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
     ∆       cout << output << endl;
            count++;
        }
        file.close();
    }
    
    
    cout << count << endl;
}

int main(int argc, char const *argv[])
{

    cout << "Hello World!" << endl;
    read("SherlockHolmes.txt");
    return 0;
}
