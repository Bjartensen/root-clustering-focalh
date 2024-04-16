#ifndef CLUSTER_ANALYSIS_H
#define CLUSTER_ANALYSIS_H

#include <vector>
#include <string>
#include <TFile.h>
#include "cluster_reader.h"
#include "definitions.h"

class ClusterAnalysis{
private:
	std::vector<std::string> cluster_files;
	std::string directory;
public:
	ClusterAnalysis(){};
	bool add_file(std::string filename);
	bool find_files(std::string folder);
	bool add_folder_files(std::string folder);
	std::string show_files();
	std::vector<std::string>& get_files();
};

#endif // CLUSTER_ANALYSIS_H
