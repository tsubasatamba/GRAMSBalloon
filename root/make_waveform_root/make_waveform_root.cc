#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

int main(int argc, char** argv){
	TString binaryName;
	TString rootName;

	if (argc==2) {
		binaryName = Form("%s.dat", argv[1]);
		rootName   = Form("%s.root", argv[1]);
	}
	else if (argc==3) {
		binaryName = Form("binary_data/GRAMS_RUN_%d_%d.dat",std::stoi(argv[1]),std::stoi(argv[2]));
		rootName   = Form("binary_data/GRAMS_RUN_%d_%d.root",std::stoi(argv[1]),std::stoi(argv[2]));
	}
	else {
		std::cout << "Usage :" << std::endl;
		std::cout << "./prod file_name_without_extension" << std::endl;
		std::cout << " or " << std::endl;
		std::cout << "./prod run_number file_number" << std::endl;
		return -1;
	}
	std::cout << binaryName << " ---> " << rootName << std::endl;

	int  runid;
	int  fileid;
	int  eventid;
	long timestamp;
	short chmask;
	short sample;
	short trigpsample;
	double freq;
	std::vector<short> wf[4];

	FILE *dataFile = fopen(binaryName.Data(), "rb");

	/*===== Read header =====*/
	auto *fileHeader = new short[16]();
	const size_t fhsize_ = sizeof(short)*16;
	fread(fileHeader, fhsize_, 1, dataFile);
	const short TRIGMODE_    = fileHeader[0];
	const short TRIGCH_      = fileHeader[1];
	const short TRIGLEV_     = fileHeader[2];
	const short CHMASK_      = fileHeader[3];
	const short SAMPLE_      = fileHeader[4];
	const short TRIGPSAMPLE_ = fileHeader[5];
	const short NEVENT_      = fileHeader[6];
	const double FREQ_       = fileHeader[7]*1e-3;
	const short ADC_RANGE_[4] = {fileHeader[8], fileHeader[9], fileHeader[10], fileHeader[11]};

	/*===== Definition =====*/
    auto *rgdSample  = new short[SAMPLE_]();
	auto *fileFooter = new short[16]();
	auto *eventHeader = new short[4]();
	const size_t chsize_ = sizeof(short)*SAMPLE_;
	const size_t ffsize_ = sizeof(short)*16;
	const size_t ehsize_ = sizeof(short)*4;


	/*===== Define output file and tree =====*/
	auto *outFile = TFile::Open(rootName.Data(), "RECREATE");
	std::vector<int> chList;
	
	auto *header = new TTree("header", "header");
	header->Branch("runid",     &runid,     "runid/I");
	header->Branch("fileid",    &fileid,    "fileid/I");
	header->Branch("timestamp", &timestamp, "timestamp/L");
	header->Branch("chmask",    &chmask,    "chmask/S");
	header->Branch("sample",    &sample,    "sample/S");
	header->Branch("trigpsample", &trigpsample,    "trigpsample/S");
	header->Branch("freq",      &freq,      "freq/D");

	auto *tree   = new TTree("tree", "tree");
	tree->Branch("eventid", &eventid, "eventid/I");
	for(int ich=0; ich<4; ++ich){
		if(0x1&(CHMASK_>>ich)){
			tree->Branch(Form("ch%d",ich), &wf[ich]);
			chList.push_back(ich);
		} // if
	} // for ich


	/*===== Fill header =====*/
	auto idxRun  = binaryName.Index("RUN_")+4;
	auto idxFile = binaryName.Index("_", idxRun)+1;
	auto idxLast = binaryName.Index(".", idxFile);
	auto strRun  = binaryName.Copy();
	strRun.Remove(idxFile-1, binaryName.Length()-idxFile+1);
	strRun.Remove(0, idxRun);
	runid   = strRun.Atoi();
	auto strFile = binaryName.Copy();
	strFile.Remove(idxLast, binaryName.Length());
	strFile.Remove(0, idxFile);
	fileid  = strFile.Atoi();

	chmask = CHMASK_;
	sample = SAMPLE_;
	freq   = FREQ_*1e-3; // [MHz]

	header->Fill();

	
	/*===== Read each event =====*/
	for (short iev=0; iev<NEVENT_; ++iev){
		fread(eventHeader, ehsize_, 1, dataFile);
		for (const auto &ich : chList){
			fread(rgdSample, chsize_, 1, dataFile);
			wf[ich].clear();
			for(short ibin=0; ibin<SAMPLE_; ++ibin){
				wf[ich].push_back(rgdSample[ibin]);
			} // for ibin
		} // for chList

		tree->Fill();

	} // for iev

	outFile->cd();
	header->Write();
	tree->Write();
	outFile->Close();

	return 1;
}
