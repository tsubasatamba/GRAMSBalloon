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

void calculate_diff_score(const std::vector<double>& v1, const std::vector<double>& v2, int maximum_shift, double& score, double& shift)
{
  score = 1E18;
  shift = 10000;
  const int n1 = v1.size();
  const int n2 = v2.size();
  if (n1!=n2) {
    std::cerr << "v1 and v2 should have the same size." << std::endl;
    exit(1);
  }
  if (maximum_shift>=n1 || maximum_shift<0) {
    std::cerr << "maximum shift too large or negative. maximum_shift = " << maximum_shift << std::endl;
    exit(1);
  }

  for (int diff=-maximum_shift; diff<=maximum_shift; diff++) {
    double sum = 0.0;
    int num = 0;
    for (int i=0; i<n1; i++) {
      int j = i + diff;
      if (j<0 || j>=n1) continue;
      const double w = v1[i]-v2[j];
      sum += w*w;
      num++;
    }
    if (num==0) {
      std::cerr << "num inappropriate" << std::endl;
      continue;
    }
    const double now = sum/static_cast<double>(num);
    if (now<score) {
      score = now;
      shift = diff;
    }
  }
}


int main(int argc, char **argv)
{
  const std::string root_filename = argv[1];

  std::string key1 = "wf_channel0";
  std::string key2 = "wf_channel1";

  TFile* f = new TFile(root_filename.c_str());
  TTree *tree = (TTree*)f->Get("tree");
  std::vector<double>* wf1;
  std::vector<double>* wf2;
  tree->SetBranchAddress(key1.c_str(), (&wf1));
  tree->SetBranchAddress(key2.c_str(), (&wf2));
  const int num_entries = tree->GetEntries();


  double effective_bin = 8000;
  double maximum_shift = 1000;
  double score = 0.0;
  double shift = 0.0;

  for (int i=0; i<num_entries; i++) {
    std::vector<double> v1;
    tree->GetEntry(i);
    for (int k=0; k<effective_bin; k++) {
      v1.push_back((*wf1)[k]);
    }
    for (int j=i-1; j<=i+1; j++) {
      if (j<0 || j>=num_entries) continue;
      std::vector<double> v2;
      tree->GetEntry(j);
      for (int k=0; k<effective_bin; k++) {
        v2.push_back((*wf2)[k]);
      }
      calculate_diff_score(v1, v2, maximum_shift, score, shift);
      std::cout << i << " " << j << " " << score << " " << shift << std::endl;
    }
  }
    
  return 0;
}
