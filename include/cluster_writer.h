#ifndef CLUSTER_WRITER_H
#define CLUSTER_WRITER_H

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "cell.h"
#include <map>
#include "clustering.h"
#include "TFile.h"
#include "TParameter.h"
#include "TObjString.h"
#include "definitions.h"

class ClusterWriter{


private:
	//Geometry geo;
	std::fstream file;
	std::string filename;

	std::unique_ptr<TFile> clustered_tfile;
	std::unique_ptr<TTree> ttree;

	Clustering &clustering;

	const char DELIM = ',';
	
	const std::string EOL = "\n";
	const std::string CLUSTER_EXT = ".cluster";
	const std::string CLUSTER_META_EXT = ".clustermeta";
	const std::string EVENT_HEADER_START = "E:";

	TTreeClustered::x_type x_pos;
	TTreeClustered::y_type y_pos;
	TTreeClustered::value_type value;
	TTreeClustered::class_type class_label;
	TTreeClustered::cluster_type cluster;

	struct EventLine{
		double x;
		double y;
		double val;
		std::string cluster_id;
	};
	
	General::EventsHeader header;

	// I don't see a reason for wanting both for the same file.
	// I'll add blockers for when trying to access read or write
	// when it doesn't make sense according to this flag.
	std::ios_base::openmode mode;

public:

	// Struct-of-Array for event values
	struct Event{
		std::vector<double> x_vec;
		std::vector<double> y_vec;
		std::vector<double> val_vec;
		std::vector<std::string> cluster_id_vec;
	};

	ClusterWriter(Clustering &_clustering, std::string _filename, std::ios_base::openmode _mode) : clustering(_clustering), filename(_filename), mode(_mode){}
	bool open();
	bool close();
	
	// Write
	bool write_event(const long event_number);
	//bool write_event_header(const long event_number);
	//bool write_event_line(double x, double y, double _value, std::string _cluster);

	void set_events_header(General::EventsHeader &h);
	void write_events_header();


	bool set_ttree_branches();
	bool clear_containers();
};



#endif // CLUSTER_WRITER_H
