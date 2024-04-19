#ifndef HISTOGRAM_ANALYSIS_H
#define HISTOGRAM_ANALYSIS_H

#include "cluster_analysis.h"
#include "plot_histogram.h"

#include <iostream>

class HistogramAnalysis : public ClusterAnalysis{

using Sum_T = std::vector<std::pair<General::energy_type, std::vector<General::float_type>>>;

private:
	// POD instead?
  // I think this class should be able to choose which cluster or no cluster
  // For instance a level field. Level 0 is the entire grid.
  // Level 1 is largest cluster, level 2 is second largest, and so on.
  // Perhaps even something even cleverer, by combining levels?
	Sum_T grid_sums;
	Sum_T cluster_sums;
public:
	void calculate_sums();
	void squawk();

  void plot(std::string name, std::string title);


  double calculate_hist_x_max();
  double calculate_hist_y_max();
  int calculate_hist_bins();
};
#endif // HISTOGRAM_ANALYSIS_H
