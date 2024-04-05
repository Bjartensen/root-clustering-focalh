#ifndef CLUSTER_EVENTS_H
#define CLUSTER_EVENTS_H

#include "clustering.h"
#include <TFile.h>
#include <vector>

// Abstract class
class ClusterEvents{
private:
	TFile &events_file;
	// vector of Clustering?

public:
	bool RunClustering(std::vector<Clustering*> &cluster_vec);

	// Derived classes must implement this.
	// return vector?
	virtual bool PrepareClustering() = 0;

	//

};




#endif // CLUSTER_EVENTS_H
