#ifndef GAUSSIAN_ANALYSIS_H
#define GAUSSIAN_ANALYSIS_H


#include "cluster_analysis.h"
#include <TFile.h>
#include <TH1D.h>



class GaussianAnalysis : public ClusterAnalysis{
  std::unique_ptr<TFile> clustered_tfile;
	std::string filename;
	TTree* ttree;
  // maybe unordered better?
  using Cluster_Map = std::map<std::string, std::vector<General::adc_type>>;
  using Cluster_Vec = std::vector<std::pair<std::string, std::vector<General::adc_type>>>;
private:
  std::unique_ptr<TFile> outfile;
  std::vector<std::unique_ptr<TH1D>> hists;

  bool open();
  bool close();
public:
  void fit_gaussian(std::vector<Cluster_Vec> &events, int min = 0, int max = 0, int clusters = 1);

  void begin_analysis(int clusters = 1);
  void squawk();
  void fill_map(Cluster_Map &map, unsigned int tag, General::adc_type val);
};


#endif // GAUSSIAN_ANALYSIS_H
