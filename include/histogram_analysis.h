#ifndef HISTOGRAM_ANALYSIS_H
#define HISTOGRAM_ANALYSIS_H

#include "cluster_analysis.h"


class HistogramAnalysis : public ClusterAnalysis{

using sums = std::vector<std::pair<General::energy_type, std::vector<General::float_type>>>;

private:
	// POD instead?
	sums grid_sums;
	sums cluster_sums;
public:
	void calculate_sums();

};
#endif // HISTOGRAM_ANALYSIS_H
