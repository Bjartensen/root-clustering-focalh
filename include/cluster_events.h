#ifndef CLUSTER_EVENTS_H
#define CLUSTER_EVENTS_H

#include "clustering.h"
#include <TFile.h>
#include <vector>

class ClusterEvents{
private:
	TFile &events_file;
	// vector of Clustering?

public:
	// For each event performs clustering according to a list.
	// The list of Clustering is prepared outside.
	bool run_clustering(std::vector<Clustering*> &cluster_vec);



	// Writes the clusters for each event to files.
	bool write_short_format();


	// Writes every cell and their tags to files.
	bool write_long_format();

};




#endif // CLUSTER_EVENTS_H
