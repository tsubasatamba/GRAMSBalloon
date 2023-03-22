#include <vector>
#include <fstream>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    std::vector<short> data;
    char *FILENAME = argv[1];
    std::ifstream ifs(std::string(FILENAME), std::ios::binary);
    char temp[2];
    short _data;
    for (int i = 0; i < 32; i++)
    {
        ifs.read(&temp[0], 2);
    }
    int i = 0;
    while (!ifs.eof())
    {
        ifs.read(&temp[0], 2);
        _data = static_cast<short>(temp[0]) + static_cast<short>(temp[1]) << 8;

        std::cout << _data << " ";
        if (i == 8)
        {
            std::cout << std::endl;
        }
        i++;
    }
    
    return 0;
}