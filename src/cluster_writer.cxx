#include "cluster_writer.h"

bool ClusterWriter::write_event(TaggedCells &cells, const long event_number){

	if (mode != std::ios_base::out) return false;
	if (!file.is_open()) return false;
	if (!file) return false;


	// Write event header
	write_event_header(event_number);

	// Write event
	for (TaggedCells::iterator it = cells.begin(); it != cells.end(); it++){
		write_line(it->first->get_x(), it->first->get_y(), it->first->get_value(), it->second);
	}

	return true;
}


bool ClusterWriter::write_line(const double &x, const double &y, const double &value, const std::string cluster_id){
	if (mode != std::ios_base::out) return false;
	if (!file) return false;
	file << x << DELIM << y << DELIM << value << DELIM << cluster_id << EOL;
	return true;
}


bool ClusterWriter::write_event_header(const long event_number){
	if (mode != std::ios_base::out) return false;
	if (!file) return false;
	file << EVENT_HEADER_START << event_number << EOL;
	return true;
}



bool ClusterWriter::read_event(TaggedCells &cells, const long &event_number){

	// Take in first line. If not of header format, return false.
	// Else, loop until next header format or EOF? How can I know
	// when the next header format line is without consuming it?


	std::string line;

	// getline all
	

	// getline csv

	Event e;

	return true;
}

bool ClusterWriter::open(){
	file.open(filename+CLUSTER_EXT, mode);
	if (!file) return false;
	return true;
}


bool ClusterWriter::close(){
	file.close();
	return true;
}
