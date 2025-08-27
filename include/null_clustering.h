#ifndef NULL_CLUSTERING_H
#define NULL_CLUSTERING_H

#include "clustering.h"


// Move to own file at some point
class NullClustering : public Clustering{

public:
	NullClustering(Grid &_g) : Clustering(_g) {}

	NullClustering() : Clustering() {}

	bool tag();
	std::string name();

};

#endif // NULL_CLUSTERING_H
