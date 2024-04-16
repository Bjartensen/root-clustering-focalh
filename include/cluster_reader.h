#ifndef CLUSTER_READER_H
#define CLUSTER_READER_H

#include <TFile.h>
#include <TTree.h>
#include "definitions.h"
#include <memory>
#include <string>

class ClusterReader{
private:
	std::unique_ptr<TFile> clustered_tfile;
	TTree* ttree;
	std::string filename;
	unsigned long entry;
	TTreeClustered::EventPtr event; // Event with pointers to x,y... etc

public:
	ClusterReader(std::string _filename) : filename(_filename), entry(0){}
	bool open();
	bool close();
	bool read_event(TTreeClustered::EventPtr &ev);
	bool read_event(TTreeClustered::EventPtr &ev, unsigned long en);

	TTree* get_ttree();

	void set_ttree_branches();

};
#endif // CLUSTER_READER_H
