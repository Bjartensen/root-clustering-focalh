#ifndef CLUSTER_WRITER_H
#define CLUSTER_WRITER_H

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "cell.h"
#include <map>




class ClusterWriter{

using Vec = std::vector<double>;
using StringVec = std::vector<std::string>;

// A bit spooky that this class should know this structure from Clustering
using TaggedCells = std::map<Cell*, std::string>;

private:
	//Geometry geo;
	std::fstream file;
	std::string filename;

	// Move to enums?
	//const std::string DELIM = ",";
	const char DELIM = ',';
	
	const std::string EOL = "\n";
	const std::string CLUSTER_EXT = ".cluster";
	const std::string CLUSTER_META_EXT = ".clustermeta";
	const std::string EVENT_HEADER_START = "E:";


	struct EventLine{
		double x;
		double y;
		double val;
		std::string cluster_id;
	};


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

	ClusterWriter(std::string _filename, std::ios_base::openmode _mode) : filename(_filename), mode(_mode){}
	bool open();
	bool close();
	
	// Write
	bool write_event(TaggedCells &cells, const long event_number);
	bool write_event_header(const long event_number);
	bool write_line(const double &x, const double &y, const double &value, const std::string cluster_id);
	bool write_metadata();

	// Read
	bool read_event(Event &event, long &event_number);
	bool read_event_line(std::string line, EventLine &event_line);
	bool read_metadata();
	bool read_event_header(std::string &line, long &event_number);
	bool is_event_header(std::string &line);

};



#endif // CLUSTER_WRITER_H
