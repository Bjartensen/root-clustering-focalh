#ifndef GAUSSIAN_ANALYSIS_H
#define GAUSSIAN_ANALYSIS_H


#include "cluster_analysis.h"
#include <TFile.h>



class GaussianAnalysis : public ClusterAnalysis{
  // maybe unordered better?
  using Cluster_Map = std::map<std::string, std::vector<General::adc_type>>;
private:
  std::unique_ptr<TFile> outfile;

public:
  void fit_gaussian(int min, int max);

  void begin_analysis();
  void squawk();
  void fill_map(Cluster_Map &map, std::string tag, General::adc_type val);
};


#endif // GAUSSIAN_ANALYSIS_H
