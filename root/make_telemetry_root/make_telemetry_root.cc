#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TelemetryDefinition.hh"

int main(int argc, char **argv)
{
    constexpr int TELEMETRY_LENGTH = 132;
    std::vector<TString> binaryName;
    TString rootName;

    // if (argc == 2)
    // {
    //     binaryName.push_back(Form("%s.dat", argv[1]));
    //     rootName = Form("%s.root", argv[1]);
    // }
    int num_input = argc;
    const size_t telSize = TELEMETRY_LENGTH * sizeof(uint8_t);
    if (num_input > 2)
    {
        for (int i = 1; i < num_input; i++)
        {
            binaryName.push_back(Form("%s.dat", argv[i]));
        }
        rootName = Form("%s_%s.root", argv[1], num_input);
    }
    else
    {
        std::cout << "Usage :" << std::endl;
        std::cout << "./prod file_name_without_extension" << std::endl;
        return -1;
    }
    TFile *outFile = TFile::Open(rootName.Data(), "RECREATE");

    std::unique_ptr<TTree> HK_telemetery = std::make_unique<TTree>("HK","HK");



    std::cout << binaryName << " ---> " << rootName << std::endl;
    for (int i = 0; i < num_input; i++)
    {
        FILE *dataFile = fopen(binaryName[i].Data(), "rb");
        while (1)
        {
        std::vector<uint8_t> telemetry(TELEMETRY_LENGTH);
        fread(telemetry.data() + i, telSize, 1, dataFile);
        break;
        }
    }

}
