#ifndef CLUSTER_READER_H
#define CLUSTER_READER_H

#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>
#include <TString.h>
#include "definitions.h"
#include <memory>
#include <string>

class ClusterReader{
private:
	std::unique_ptr<TFile> clustered_tfile;
	TTree* ttree;
	std::string filename;
	unsigned long entry;
	TFileGeneric::EventPtr event; // Event with pointers to x,y... etc
	General::EventsHeader header;

public:
	ClusterReader(std::string _filename) : filename(_filename), entry(0){}
	bool open();
	bool close();
	bool read_event(TFileGeneric::EventPtr &ev);
	bool read_event(TFileGeneric::EventPtr &ev, unsigned long en);


	void read_events_header();
	General::EventsHeader& get_events_header();

	TTree* get_ttree();

	void set_ttree_branches();

};
#endif // CLUSTER_READER_H
