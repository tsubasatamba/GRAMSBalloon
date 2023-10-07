#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <sstream>
#include <algorithm>
#include <utility>
#include <regex>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TFile.h>
#include <TTree.h>
#include "TRint.h"
#include "TH1D.h"

void calculate_diff_score(const std::vector<double>& v1, const std::vector<double>& v2, int maximum_shift, double& score, int& shift) {
  score = 1E18;
  shift = 10000;
  const int n1 = v1.size();
  const int n2 = v2.size();

  if (n1 != n2) {
    std::cerr << "v1 and v2 should have the same size." << std::endl;
    exit(1);
  }
  if (maximum_shift >= n1 || maximum_shift < 0) {
    std::cerr << "maximum shift too large or negative. maximum_shift = " << maximum_shift << std::endl;
    exit(1);
  }


  for (int diff = -maximum_shift; diff <= maximum_shift; diff++) {
    double sum = 0.0;
    int num = 0;
    for (int i = 0; i < n1; i++) {
      int j = i + diff;
      if (j < 0 || j >= n1) continue;
      const double w = v1[i] - v2[j];
      sum += w * w;
      num++;
    }
    if (num == 0) {
      std::cerr << "num inappropriate" << std::endl;
      continue;
    }
    const double now = sum / static_cast<double>(num);
    if (now < score) {
      score = now;
      shift = diff;
    }
  }
}


int main(int argc, char** argv) {
  const std::string output_root_filename = argv[1];
  // std::vector<std::string> root_filenames;
  // for (int i = 1; i < argc; i++) {
  //   root_filenames.push_back(std::string(argv[i]));
  // }
  std::cout << "argc: " << argc << std::endl;
  const int filenum_max = argc - 2;
  std::string key1 = "wf_channel0";
  std::string key2 = "wf_channel2";
  const int event_per_file = 100;
  std::cout << "filenum_max: " << filenum_max << std::endl;//For Debug
  const int max_num_event = filenum_max * event_per_file;
  TFile* outputfile = new TFile(output_root_filename.c_str(), "RECREATE");
  TH1D* hist = new TH1D("hist1", "hist1", max_num_event, -0.5, max_num_event + 0.5);
  TH1D* hist2 = new TH1D("hist2", "hist2", max_num_event, -0.5, max_num_event + 0.5);
  TH1D* hist_after = new TH1D("hist_after", "hist_after", max_num_event, -0.5, max_num_event + 0.5);
  TH1D* hist2_after = new TH1D("hist2_after", "hist2_after", max_num_event, -0.5, max_num_event + 0.5);
  TTree* outputtree = new TTree("analysis_result", "analysis_result");
  bool is_bad_event = false;
  double dt_before, dt_after, minimum_score_tree;
  int file_index, event_index, minimum_id_shift, event_index_whole;
  TTree* detail_information = nullptr;
  outputtree->Branch("event_index_whole", &event_index_whole, "event_index_whole/I");
  outputtree->Branch("file_index", &file_index, "file_index/I");
  outputtree->Branch("event_index", &event_index, "event_index/I");
  outputtree->Branch("IsBadEvent", &is_bad_event);
  outputtree->Branch("dt_before", &dt_before, "dt_before/D");
  outputtree->Branch("dt_after", &dt_after, "dt_after/D");
  outputtree->Branch("minimum_score", &minimum_score_tree, "minimum_score/D");
  outputtree->Branch("minimum_id_shift", &minimum_id_shift, "minimum_id_shift/I");
  outputtree->Branch("detail_information", &detail_information);
  hist->SetMarkerColor(kRed);
  hist_after->SetMarkerColor(kRed);
  hist->SetMarkerSize(5);
  hist_after->SetMarkerSize(5);
  hist2->SetMarkerColor(kBlack);
  hist2_after->SetMarkerColor(kBlack);
  hist2->SetMarkerSize(5);
  hist2_after->SetMarkerSize(5);
  hist->SetXTitle("Event index");
  hist_after->SetXTitle("Event index");
  hist2->SetXTitle("Event index");
  hist2_after->SetXTitle("Event index");
  hist->SetYTitle("dt_before [ms]");
  hist_after->SetYTitle("dt_after [ms]");
  hist2->SetYTitle("dt_before [ms]");
  hist2_after->SetYTitle("dt_after [ms]");
  int file_index_offset = 0;
  for (int filenum = 0; filenum < filenum_max; filenum++) {
    file_index = filenum;
    TFile* f = new TFile(argv[filenum + 2]);
    TTree* tree = (TTree*)f->Get("tree");
    std::vector<double>* wf1 = nullptr;
    std::vector<double>* wf2 = nullptr;
    int event_time_sec, event_time_usec;
    tree->SetBranchAddress(key1.c_str(), &wf1);
    tree->SetBranchAddress(key2.c_str(), &wf2);
    tree->SetBranchAddress("event_time_sec", &event_time_sec);
    tree->SetBranchAddress("event_time_usec", &event_time_usec);
    //wf1->resize(8192);
    //wf2->resize(8192);
    const int num_entries = tree->GetEntries();

    std::vector<std::vector<double>> v1(num_entries);
    std::vector<std::vector<double>> v2(num_entries);
    std::vector<double> time;
    double effective_bin = 8000;

    for (int i = 0; i < num_entries; i++) {
      tree->GetEntry(i);
      if (effective_bin > wf1->size() || effective_bin > wf2->size()) {
        std::cerr << "effective_bin inappropriate" << std::endl;
        std::cout << effective_bin << " " << wf1->size() << " " << wf2->size() << std::endl;
        exit(1);
      }
      for (int j = 0; j < effective_bin; j++) {
        v1[i].push_back((*wf1)[j]);
        v2[i].push_back((*wf2)[j]);
      }
      double time_event = static_cast<double>(event_time_sec) + static_cast<double>(event_time_usec) * 1e-6;
      time.push_back(time_event);
    }

    // zero check
    for (int i = 0; i < num_entries; i++) {
      event_index = i;
      event_index_whole = file_index_offset + i;
      detail_information = new TTree(Form("detail_%d", i), Form("detail_%d", i));
      const double max1 = *std::max_element(v1[i].begin(), v1[i].end());
      const double min1 = *std::min_element(v1[i].begin(), v1[i].end());
      const double max2 = *std::max_element(v2[i].begin(), v2[i].end());
      const double min2 = *std::min_element(v2[i].begin(), v2[i].end());
      const double diff1 = max1 - min1;
      const double diff2 = max2 - min2;
      //std::cout << "diff" << i << " " << diff1 << " " << diff2 << std::endl;
      const double threshold = 0.33E-3;
      if (diff2 < threshold) {
        std::cout << "event " << i << " did not detect any signal." << std::endl;
        is_bad_event = true;
      }
      else {
        is_bad_event = false;
      }
      // }

      double maximum_shift = 1000;
      double score = 0.0;
      int shift = 0;
      int time_bin_shift = 0;
      int event_id_shift_tree = 0;
      detail_information->Branch("event_id_shift", &event_id_shift_tree, "event_id_shift/I");
      detail_information->Branch("score", &score, "score/D");
      detail_information->Branch("shift", &shift, "shift/D");

      // for (int i = 0; i < num_entries; i++) {
      double min_score = 1E18;
      int event_id_shift = 100000;
      for (int j = i - 2; j <= i + 2; j++) {
        if (j < 0 || j >= num_entries) continue;
        calculate_diff_score(v1[i], v2[j], maximum_shift, score, shift);
        event_id_shift_tree = j - i;
        //std::cout << i << " " << j << " " << score << " " << shift << std::endl;
        if (score < min_score) {
          min_score = score;
          event_id_shift = j - i;
          time_bin_shift = shift;
        }
        detail_information->Fill();
      }
      if (time_bin_shift < -20 || time_bin_shift > -12) {
        is_bad_event = true;
      }
      double dt1 = 0.;
      double dt2 = 0.;
      if (i >= 1) {
        dt1 = 1000 * (time[i] - time[i - 1]);
        dt_before = dt1;
      }
      if (i < num_entries - 1) {
        dt2 = 1000 * (time[i + 1] - time[i]);
        dt_after = dt2;
      }

      minimum_id_shift = event_id_shift;
      minimum_score_tree = min_score;

      if (is_bad_event) {
        outputtree->Fill();
        delete detail_information;
        continue;
      }

      // std::cout << i << " " << event_id_shift << " " << dt1 << " " << dt2 << std::endl;
      if (event_id_shift == -1) {
        // std::cout << "file_index_offset + i: " << file_index_offset + i << std::endl;
        const int histbin = hist->FindBin(file_index_offset + i);
        const int histbin_after = hist_after->FindBin(file_index_offset + i);
        hist->SetBinContent(histbin, dt1);
        hist_after->SetBinContent(histbin_after, dt2);
        hist->SetBinError(histbin, 0.);
        hist_after->SetBinError(histbin_after, 0.);
      }
      else if (event_id_shift == 0) {
        const int histbin = hist2->FindBin(file_index_offset + i);
        const int histbin_after = hist2_after->FindBin(file_index_offset + i);
        std::cout << histbin << " " << dt1 << std::endl;
        hist2->SetBinContent(histbin, dt1);
        hist2_after->SetBinContent(histbin_after, dt2);
        hist2->SetBinError(histbin, 0.);
        hist2_after->SetBinError(histbin_after, 0.);
      }
      outputtree->Fill();
      delete detail_information;
    }
    file_index_offset += num_entries;
    f->Close();
    delete f;
  }
  outputfile->cd();
  hist->Write();
  hist2->Write();
  hist_after->Write();
  hist2_after->Write();
  outputtree->Write();
  delete outputtree;
  delete hist2;
  delete hist2_after;
  delete hist_after;
  delete hist;
  outputfile->Close();
  delete outputfile;
  return 0;
}
