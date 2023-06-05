#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TelemetryDefinition.hh"

#define VERBOSE 2

using namespace gramsballoon;

int main(int argc, char **argv)
{
    constexpr int TELEMETRY_LENGTH = 132;

    if (argc != 2)
    {
        std::cerr << "Usage: ./Prod inputfile outputfile" << std::endl;
        return -1;
    }
    const std::string input_filename(argv[1]);
    const std::string output_filename(argv[2]);
    std::fstream ifs(input_filename, std::ios_base::binary | std::ios::in);

#if VERBOSE > 0
    std::cout << "Input filename: " << input_filename << std::endl;
    std::cout << "Output filename: " << output_filename << std::endl;
#endif

    std::vector<std::string>
        binary_files;

    while (!ifs.eof())
    {
        std::string line;
        std::getline(ifs, line);
        binary_files.push_back(line);
#if VERBOSE > 1
        std::cout << line << std::endl;
#endif
    }

    ifs.close();

    std::vector<std::vector<uint8_t>> Telemetry;
    for (int i = 0; i < binary_files.size(); i++)
    {
        std::fstream ifs(binary_files[i]);

        while (!ifs.eof())
        {
            std::vector<uint8_t> buf(TELEMETRY_LENGTH);
            for (int j = 0; j < TELEMETRY_LENGTH; j++)
            {
                ifs >> buf[i];
            }
            Telemetry.push_back(buf);
        }
    }

    TFile *rootfile = TFile::Open(output_filename.c_str());
    std::unique_ptr<TTree> HK_telemetry = std::make_unique<TTree>("HK", "HK");

    for (int i = 0;)
}
