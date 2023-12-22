#ifndef FOCAL_COORDINATES_H
#define FOCAL_COORDINATES_H

#include <string>
#include <map>
#include <vector>



class FocalCoordinates{
	// Map of pair<board,ch> to pair<x,y>
	std::map<std::pair<unsigned short, unsigned short>, std::pair<double, double>> channel_coordinates_map;

public:

	FocalCoordinates(){;} // not sure if should be possible
	FocalCoordinates(std::string module_coordinates_filename, std::string channel_map_filename);

	void print_module_coordinates(std::string infile);
	void generate_module_coordinates(std::string outfile);

	// read csv line function?
	std::vector<std::string> read_csv_line(std::ifstream &stream);

	bool board_channel_coordinates(unsigned short board, unsigned short channel, double &x, double &y);


	bool frames_to_cluster_digits(std::string frames_filename, std::string cluster_digits_filename);

};

#endif //FOCAL_COORDINATES_H
