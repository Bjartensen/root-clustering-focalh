#include "focal_coordinates.h"
#include <iostream>
#include <fstream>
#include <sstream>

FocalCoordinates::FocalCoordinates(std::string module_coordinates_filename, std::string channel_map_filename){


	// read files and insert into channel_map

	// Module coordinates
	std::map<std::tuple<unsigned short, unsigned short, unsigned short>, std::pair<double, double>> module_coordinates_map;
	std::map<std::pair<unsigned short, unsigned short>, std::tuple<unsigned short, unsigned short, unsigned short>> channel_map_map;

	std::ifstream module_coordinates(module_coordinates_filename, std::ios::in);
	if (!module_coordinates.is_open()){
		std::cout << "Failed to open coordinates file." << std::endl;
	}
	else{
		// Read header
		std::vector<std::string> header = read_csv_line(module_coordinates);
		unsigned short mod_idx, row_idx, col_idx, x_idx, y_idx;
		for (unsigned short i = 0; i < header.size(); i++){
			if (header.at(i) == "module")
				mod_idx = i;
			else if (header.at(i) == "row")
				row_idx = i;
			else if (header.at(i) == "col")
				col_idx = i;
			else if (header.at(i) == "x")
				x_idx = i;
			else if (header.at(i) == "y")
				y_idx = i;
			else{
				std::cout << "Wrong or missing header names: Should be module, row, col, x, y." << std::endl;
			}
		}

		std::vector<std::string> temp_row;
		while (!module_coordinates.eof()){
			temp_row = read_csv_line(module_coordinates);
			if (temp_row.size() == 0)
				continue;
			unsigned short _mod = std::stoi(temp_row.at(mod_idx));
			unsigned short _row = std::stoi(temp_row.at(row_idx));
			unsigned short _col = std::stoi(temp_row.at(col_idx));
			double _x = std::stod(temp_row.at(x_idx));
			double _y = std::stod(temp_row.at(y_idx));

			std::tuple<unsigned short, unsigned short, unsigned short> temp_tuple(_mod,_row,_col);
			std::pair<double, double> temp_pair(_x,_y);
			if (!module_coordinates_map.insert(std::make_pair(temp_tuple, temp_pair)).second)
				std::cout << "ERROR: Couldn't insert into map (check uniqueness)." << std::endl;
		}
	}


	// Channel mapping
	std::ifstream channel_mapping(channel_map_filename, std::ios::in);
	if (!channel_mapping.is_open()){
		std::cout << "Failed to open channel mapping file." << std::endl;
	}
	else{
		// Read header
		std::vector<std::string> header = read_csv_line(channel_mapping);
		unsigned short board_idx, channel_idx, mod_idx, row_idx, col_idx;
		for (unsigned short i = 0; i < header.size(); i++){
			if (header.at(i) == "board")
				board_idx = i;
			else if (header.at(i) == "channel")
				channel_idx = i;
			else if (header.at(i) == "module")
				mod_idx = i;
			else if (header.at(i) == "row")
				row_idx = i;
			else if (header.at(i) == "col")
				col_idx = i;
			else{
				std::cout << "Wrong or missing header names: Should be board, channel, module, row, col." << std::endl;
			}
		}

		std::vector<std::string> temp_row;
		while (!channel_mapping.eof()){
			temp_row = read_csv_line(channel_mapping);
			if (temp_row.size() == 0)
				continue;
			unsigned short _board = std::stoi(temp_row.at(board_idx));
			unsigned short _channel = std::stoi(temp_row.at(channel_idx));
			unsigned short _mod = std::stoi(temp_row.at(mod_idx));
			unsigned short _row = std::stoi(temp_row.at(row_idx));
			unsigned short _col = std::stoi(temp_row.at(col_idx));

			std::tuple<unsigned short, unsigned short, unsigned short> temp_tuple(_mod,_row,_col);
			std::pair<unsigned short, unsigned short> temp_pair(_board,_channel);
			if (!channel_map_map.insert(std::make_pair(temp_pair, temp_tuple)).second){
				std::cout << "ERROR: Couldn't insert into map (check uniqueness)." << std::endl;
				std::cout << _board << "," << _channel << ",";
				std::cout << _mod << "," << _row << "," << _col << std::endl;
			}
		}
	}


	for (auto &c : channel_map_map){

		auto channel_module_map = module_coordinates_map[c.second];


		std::pair<unsigned short, unsigned short> board_channel_pair(c.first.first, c.first.second);
		std::pair<double, double> coordinates_pair(channel_module_map.first, channel_module_map.second);

		if (!channel_coordinates_map.insert(std::make_pair(board_channel_pair, coordinates_pair)).second)
			std::cout << "ERROR: Couldn't insert into channel_coordinate_map." << std::endl;



	}
	
}

std::vector<std::string> FocalCoordinates::read_csv_line(std::ifstream &stream){
	std::vector<std::string> tokens;
	std::string row;
	std::getline(stream, row);
	std::stringstream line_stream(row);
	std::string token;
	while(std::getline(line_stream, token, ','))
		tokens.push_back(token);
	return tokens;
}

bool FocalCoordinates::board_channel_coordinates(unsigned short board, unsigned short channel, double &x, double &y){
	auto find_it = channel_coordinates_map.find(std::make_pair(board, channel));
	if (find_it == channel_coordinates_map.end())
		return false;
	x = find_it->second.first;
	y = find_it->second.second;
	return true;
}
	

void FocalCoordinates::print_module_coordinates(std::string infile){


}

void FocalCoordinates::generate_module_coordinates(std::string outfile){
	// Generate Focal module coordinates.
	// The sides of the modules are 1.5mm copper plates.
	// The modules have a 3mm margin.
	// Modules are 65x65mm^2 with plate
	
	std::ofstream file;
	file.open(outfile);
	const std::string sep = ",";
	const std::string endline = "\n";
	const std::string mod_csv = "module";
	const std::string row_csv = "row";
	const std::string col_csv = "col";
	const std::string x_csv = "x";
	const std::string y_csv = "y";
	file << mod_csv + sep + row_csv + sep + col_csv + sep + x_csv + sep + y_csv + endline;

	const double detector_width = 19.5; // cm
	const double detector_height = 19.5; // cm

	const double module_width = detector_width/3.0;
	const double module_height = detector_height/3.0;

	const unsigned int center_module_rows = 7;
	const unsigned int center_module_cols = 7;

	const unsigned int outer_module_rows = 5;
	const unsigned int outer_module_cols = 5;


	const std::pair<double, double> center(0.0, 0.0);

	// Center module


	short count  = 0;
	for (short row = 0; row < 3; row++){
		for (short col = 0; col < 3; col++){
			// Center module
			if (row == 1 && col == 1){
				for (short row = 0; row < center_module_rows; row++){
					for (short col = 0; col < center_module_cols; col++){
						double center_cell_height = module_height/center_module_cols;
						double center_cell_width = module_width/center_module_rows;
						double y_coord = center.second + center_cell_height*(-row + 3.0);
						double x_coord = center.first + center_cell_width*(col - 3.0);
						file << 4 << sep << row << sep << col << sep << x_coord << sep << y_coord << endline;
					}
				}
			}else{
				std::pair<double, double> module_center;
				module_center = std::make_pair(module_width*(col - 1), module_height*(-row + 1));
				std::cout << "(" << row << "," << col << "), mod: " << count << " at: " << module_center.first << "," << module_center.second << std::endl;


				// Outer modules
				for (short mod_row = 0; mod_row < outer_module_rows; mod_row++){
					for (short mod_col = 0; mod_col < outer_module_cols; mod_col++){
						double module_cell_height = module_height/outer_module_cols;
						double module_cell_width = module_width/outer_module_rows;
						double y_coord = module_center.second + module_cell_height*(-mod_row + 2.0);
						double x_coord = module_center.first + module_cell_width*(mod_col - 2.0);
						file << count << sep << mod_row << sep << mod_col << sep << x_coord << sep << y_coord << endline;
					}
				}
			}

			count++;
		}
	}


	file.close();
	


}

