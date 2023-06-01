#include <string>
#include <fstream>
#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TelemetryDefinition.hh"

int main(int argc, char **argv){
    TString binaryName;
    TString rootName;

    if (argc == 2)
    {
        binaryName = Form("%s.dat", argv[1]);
        rootName = Form("%s.root", argv[1]);
    }
    else
    {
        std::cout << "Usage :" << std::endl;
        std::cout << "./prod file_name_without_extension" << std::endl;
        std::cout << " or " << std::endl;
        std::cout << "./prod run_number file_number" << std::endl;
        return -1;
    }

    std::cout << binaryName << " ---> " << rootName << std::endl;

    FILE *dataFile = fopen(binaryName.Data(), "rb");
    

}