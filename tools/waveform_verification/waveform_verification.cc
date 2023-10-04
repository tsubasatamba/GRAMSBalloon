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
  const std::string root_filename = argv[1];

  std::string key1 = "wf_channel0";
  std::string key2 = "wf_channel2";

  TFile* f = new TFile(root_filename.c_str());
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
    }
  }

  double maximum_shift = 1000;
  double score = 0.0;
  int shift = 0;
  int time_bin_shift = 0;


  for (int i = 0; i < num_entries; i++) {
    double min_score = 1E18;
    int event_id_shift = 100000;
    for (int j = i - 2; j <= i + 2; j++) {
      if (j < 0 || j >= num_entries) continue;
      calculate_diff_score(v1[i], v2[j], maximum_shift, score, shift);
      //std::cout << i << " " << j << " " << score << " " << shift << std::endl;
      if (score < min_score) {
        min_score = score;
        event_id_shift = j - i;
        time_bin_shift = shift;
      }
    }
    if (time_bin_shift < -20 || time_bin_shift > -12) continue;
    double dt1 = 0.;
    double dt2 = 0.;
    if (i >= 1) {
      dt1 = 1000 * (time[i] - time[i - 1]);
    }
    if (i < num_entries - 1) {
      dt2 = 1000 * (time[i + 1] - time[i]);
    }
    std::cout << i << " " << event_id_shift << " " << dt1 << " " << dt2 << std::endl;

  }

  return 0;
}
