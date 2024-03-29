#include <vector>
#include <fstream>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    std::vector<int16_t> data;
    char *FILENAME = argv[1];
    std::ifstream ifs(std::string(FILENAME), std::ios::binary);
    char temp[8146];
    ifs.read(temp, sizeof(char) * 8146);
    int index = 0;
    while (index<64) {
        short val = ((static_cast<int16_t>(temp[index]))<<8) + (static_cast<short>(temp[index+1]));
        std::cout << index/2 << " " << (int)temp[index] << " " << (int)temp[index+1] << " " <<  val << std::endl;
        index += 2;
    }

    const int num_events = 5;
    const int num_channels = 4;
    const int num_bins = 180;
    for (int i=0; i<num_events; i++) {
        const int ev_header_size = 5;
        for (int j=0; j<ev_header_size; j++) {
            index += 2;
        }
        for (int j=0; j<num_channels; j++) {
            std::cout << "channel " << j << std::endl;
            std::vector<short> data;
            for (int k=0; k<num_bins; k++) {
                int16_t val = ((static_cast<short>(temp[index]))<<8) + (static_cast<short>(temp[index+1]));
                data.push_back(val);
                index += 2;
            }
            std::cout << "size: " << num_bins << std::endl;
            for (int k=0; k<num_bins; k++) {
                std::cout << data[k] << " ";
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}
