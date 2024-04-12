#ifndef CLUSTER_EVENTS_H
#define CLUSTER_EVENTS_H

#include "clustering.h"
#include "cluster_writer.h"
#include <TFile.h>
#include <vector>
#include <memory>

class ClusterEvents{

using vec_cl_ptr = std::vector<std::unique_ptr<Clustering>>;

private:

	std::unique_ptr<TFile> events_file;
	std::string filename;
	// vector of Clustering?
	
	std::string tfile_options = "READ";
	std::string TREE_NAME = "T"; // Should be moved to some header definition

public:
	ClusterEvents(std::string _filename) : filename(_filename) {}
	// For each event performs clustering according to a list.
	// The list of Clustering is prepared outside.
	bool run_clustering(vec_cl_ptr &clustering_vec, Grid &g, unsigned long start = 0, unsigned long end = 0);

	//bool cluster();

	bool open();
	bool close();


	void setup_writers(
		std::vector<std::unique_ptr<ClusterWriter>> &writers,
		vec_cl_ptr &clustering_vec);

	bool cluster_event(vec_cl_ptr &clustering_vec);
	bool write_event(std::vector<std::unique_ptr<ClusterWriter>> &writers, long event_number);

	// Writes the clusters for each event to files.
	bool write_short_format();


	// Writes every cell and their tags to files.
	bool write_long_format();

};


#endif // CLUSTER_EVENTS_H
