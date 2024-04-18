#ifndef CLUSTER_EVENTS_H
#define CLUSTER_EVENTS_H

#include "clustering.h"
#include "cluster_writer.h"
#include <TFile.h>
#include <TParameter.h>
#include <vector>
#include <memory>
#include "definitions.h"

class ClusterEvents{

using vec_cl_ptr = std::vector<std::unique_ptr<Clustering>>;

private:

	std::unique_ptr<TFile> events_file;
	std::string filename;
	
	std::string tfile_options = "READ";

	General::EventsHeader header;

public:
	ClusterEvents(std::string _filename) : filename(_filename) {}
	// For each event performs clustering according to a list.
	// The list of Clustering is prepared outside.
	bool run_clustering(vec_cl_ptr &clustering_vec, Grid &g, unsigned long start = 0, unsigned long end = 0);

	//bool cluster();

	bool open();
	bool close();

	std::string get_full_path(std::string algo_pars);


	void setup_writers(
		std::vector<std::unique_ptr<ClusterWriter>> &writers,
		vec_cl_ptr &clustering_vec);

	bool cluster_event(vec_cl_ptr &clustering_vec);
	bool write_event(std::vector<std::unique_ptr<ClusterWriter>> &writers, long event_number);

	void set_events_header_energy(General::energy_type energy);
	void set_events_header_source(std::string source);
	void set_events_header_description(std::string description);
	General::energy_type get_ttree_energy(TFile &f);

};


#endif // CLUSTER_EVENTS_H
