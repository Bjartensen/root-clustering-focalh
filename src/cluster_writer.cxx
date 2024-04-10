#include "cluster_writer.h"

						#include <iostream>

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



// Take in first line. If not of header format, return false.
// Else, loop until next header format or EOF? How can I know
// when the next header format line is without consuming it?
bool ClusterWriter::read_event(Event &event, long &event_number){
	if (mode != std::ios_base::in) return false;
	if (!file.is_open()) return false;
	if (!file) return false;

	// Read entire line
	std::string line;
	std::getline(file, line);

	// Check if event header
	if (!read_event_header(line, event_number)) return false;

	// Loop until next header or EOF
	// Be mindful that you are consuming the file. Need to move one back
	// after finding the the next event.

	//Event e;
	EventLine el;
	int file_pos = file.tellg();

	while (std::getline(file, line)){
		// move seek one back
		// break



		// if another event header
		if (is_event_header(line)){
			file.seekg(file_pos, std::ios_base::beg);
			std::cout << "Another event!" << std::endl;
			break;
		}


		// Fill event
		read_event_line(line, el);
		event.x_vec.push_back(el.x);
		event.y_vec.push_back(el.y);
		event.val_vec.push_back(el.val);
		event.cluster_id_vec.push_back(el.cluster_id);


		//std::cout << el.x << "," << el.y << "," << el.val << "," << el.cluster_id << std::endl;

		file_pos = file.tellg();
	}

	return true;
}



bool ClusterWriter::read_event_line(std::string line, EventLine &event_line){
	std::stringstream line_stream(line);
	std::string str_x;
	std::string str_y;
	std::string str_val;
	std::string str_cluster_id;

	std::getline(line_stream, str_x, DELIM);
	std::getline(line_stream, str_y, DELIM);
	std::getline(line_stream, str_val, DELIM);
	std::getline(line_stream, str_cluster_id, DELIM);

	event_line.x = std::stod(str_x);
	event_line.y = std::stod(str_y);
	event_line.val = std::stod(str_val);
	event_line.cluster_id = str_cluster_id;


	return true;
}


bool ClusterWriter::read_event_header(std::string &line, long &event_number){
	size_t match_pos = line.rfind(EVENT_HEADER_START);
	if (match_pos == std::string::npos) return false;

	line.erase(0, match_pos+EVENT_HEADER_START.size());
	event_number = std::stol(line, 0);
	return true;
}

bool ClusterWriter::is_event_header(std::string &line){
	if (line.rfind(EVENT_HEADER_START) == std::string::npos) return false;
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
