#include "cluster_analysis.h"


bool ClusterAnalysis::add_file(std::string filename){
	TFile check(filename.c_str(), "READ");
	if (check.IsZombie()) return false;
	check.Close();
	cluster_files.push_back(filename);
	return true;
}


std::string ClusterAnalysis::show_files(){
	std::string files_string = "";
	for (int i = 0; i < cluster_files.size(); i++){
		files_string += cluster_files.at(i);
		if (i < cluster_files.size()-1) files_string += ",";
	}

	return files_string;
}



std::vector<std::string>& ClusterAnalysis::get_files(){
	return cluster_files;
}
