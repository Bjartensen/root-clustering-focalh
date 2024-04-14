#include "cluster_writer.h"
#include "definitions.h"


#include <iostream>

bool ClusterWriter::open(){

	clustered_tfile = std::make_unique<TFile>(filename.c_str(), "RECREATE");
	ttree = std::make_unique<TTree>(TTreeClustered::TreeName.c_str(), TTreeClustered::TreeTitle.c_str());
	//clustered_tfile->cd();

	if (!set_ttree_branches()) return false;

	if (!clustered_tfile) return false;
	if (clustered_tfile->IsZombie()) return false;

	/*
	file.open(filename+CLUSTER_EXT, mode);
	if (!file) return false;
	*/



	return true;
}

bool ClusterWriter::close(){
	//file.close();
	

	ttree->Print();
	clustered_tfile->cd();
	ttree->Write();
	ttree.release();
	//clustered_tfile->Write();
	//clustered_tfile->Close();
	return true;
}


bool ClusterWriter::write_event(const long event_number){


	//if (mode != std::ios_base::out) return false;
	//if (!file.is_open()) return false;
	//if (!file) return false;


	// Set tree branches
	// Get Grid from Clustering
	// Loop over Grid cells
	// Match cells to Clustering tagged cells



	// Hold on, move the set branch etc to another function that
	// does it once, then fill event

	// Damn, I want this in another function...




	clustered_tfile->cd(); // I think I need this

	// ADD SOME SET ADDRESS??
	// https://root.cern.ch/doc/master/classTTree.html#addingacolumnofstl
	//ttree->Branch(TTreeClustered::x_branch.c_str(), &x_pos);




	// Write event header
	//write_event_header(event_number);

	// Write event
	//for (auto it = cells.begin(); it != cells.end(); it++){
	auto cells = clustering.get_tagged_cells();
	for (auto it = cells->begin(); it != cells->end(); it++){
		//write_event_line(it->first->get_x(), it->first->get_y(), it->first->get_value(), it->second);
		
		x_pos.push_back(it->first->get_x());
		y_pos.push_back(it->first->get_y());
		value.push_back(it->first->get_value());
		class_label.push_back(1);
		cluster.push_back(it->second);
	}

	ttree->Fill();
	clear_containers();
	//ttree->Write();
	//clustered_tfile->Write();

	return true;
}


bool ClusterWriter::write_event_line(double x, double y, double _value, std::string _cluster){
	if (mode != std::ios_base::out) return false;
	if (!file) return false;
	file << x << DELIM << y << DELIM << _value << DELIM << _cluster << EOL;
	return true;
}


bool ClusterWriter::write_event_header(const long event_number){
	if (mode != std::ios_base::out) return false;
	if (!file) return false;
	file << EVENT_HEADER_START << event_number << EOL;
	return true;
}

bool ClusterWriter::set_ttree_branches(){

	ttree->Branch(TTreeClustered::x_branch.c_str(), &x_pos);
	ttree->Branch(TTreeClustered::y_branch.c_str(), &y_pos);
	ttree->Branch(TTreeClustered::value_branch.c_str(), &value);
	ttree->Branch(TTreeClustered::class_branch.c_str(), &class_label);
	ttree->Branch(TTreeClustered::cluster_branch.c_str(), &cluster);

	return true;
}


bool ClusterWriter::clear_containers(){


	x_pos.clear();
	y_pos.clear();
	value.clear();
	class_label.clear();
	cluster.clear();
	return true;
}

bool ClusterWriter::read_event(Event &event, long &event_number){
	if (mode != std::ios_base::in) return false;
	if (!file.is_open()) return false;
	if (!file) return false;

	// Read entire line
	std::string line;
	std::getline(file, line);

	// Check if event header
	if (!read_event_header(line, event_number)) return false;

	EventLine el;
	int file_pos = file.tellg();

	while (std::getline(file, line)){
		if (is_event_header(line)){
			file.seekg(file_pos, std::ios_base::beg);
			break;
		}

		// Fill event
		read_event_line(line, el);
		event.x_vec.push_back(el.x);
		event.y_vec.push_back(el.y);
		event.val_vec.push_back(el.val);
		event.cluster_id_vec.push_back(el.cluster_id);

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

