#ifndef HISTOGRAM_ANALYSIS_H
#define HISTOGRAM_ANALYSIS_H

#include "cluster_analysis.h"

#include <iostream>

class HistogramAnalysis : public ClusterAnalysis{

using Sum_T = std::vector<std::pair<General::energy_type, std::vector<General::float_type>>>;

private:
	// POD instead?
	Sum_T grid_sums;
	Sum_T cluster_sums;
public:
	void calculate_sums();

	void squawk();

};
#endif // HISTOGRAM_ANALYSIS_H
