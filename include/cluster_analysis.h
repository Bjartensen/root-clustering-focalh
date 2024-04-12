#ifndef CLUSTER_ANALYSIS_H
#define CLUSTER_ANALYSIS_H

#include <vector>
#include <string>


class ClusterAnalysis{
private:
	std::vector<std::string> cluster_files;
	std::string directory;
public:
	ClusterAnalysis(){};
	bool add_file(std::string filename);
	bool find_files(std::string folder);
	bool add_folder_files(std::string folder);
	bool find_meta_file(std::string cluster_file, std::string &meta_file);
	bool find_cluster_file(std::string meta_file, std::string &cluster_file);

};

#endif // CLUSTER_ANALYSIS_H
