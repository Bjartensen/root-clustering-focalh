#include "cluster_events.h"

#include <iostream>

bool ClusterEvents::open(){
	events_file = std::make_unique<TFile>(filename.c_str(), tfile_options.c_str());
	if (!events_file) return false;
	if (events_file->IsZombie()) return false;
	return true;
}

bool ClusterEvents::close(){
	events_file->Close();
	return true; // Meaningless...
}

bool ClusterEvents::run_clustering(vec_cl_ptr &clustering_vec, Grid &g, unsigned long start, unsigned long end){

	if (clustering_vec.size() == 0) return false;

	set_events_header_energy(get_ttree_energy(*events_file));
	std::vector<std::unique_ptr<ClusterWriter>> writers;
	setup_writers(writers, clustering_vec);


	// Get tree and check for bounds
	std::unique_ptr<TTree> ttree(events_file->Get<TTree>(TTreeData::TreeName.c_str()));
	if (end == 0) end = ttree->GetEntries();
	if (end - start <= 0) return false;

	// For each event, loop through clustering_vec and perform clustering
	for (long e = start; e < end; e++){
		for (auto &v : clustering_vec) v->set_grid(g); // Performance hit
		g.fill_grid_ttree_entry(*ttree, e, true);
		cluster_event(clustering_vec);
		write_event(writers, e);
	}

	// Clean up
	for (auto &v : writers) v->close();

	return true;
}

bool ClusterEvents::cluster_event(vec_cl_ptr &clustering_vec){
	for (auto &v : clustering_vec){
		v->tag();
	}
	return true;
}


bool ClusterEvents::write_event(std::vector<std::unique_ptr<ClusterWriter>> &writers, long event_number){
	for (auto &v : writers) v->write_event(event_number);
	return true;
}


void ClusterEvents::setup_writers(
		std::vector<std::unique_ptr<ClusterWriter>> &writers,
		vec_cl_ptr &clustering_vec){

	for (auto &v : clustering_vec){
		std::unique_ptr<ClusterWriter> w;
		std::string filename = std::to_string(get_ttree_energy(*events_file));
		filename += "_"+v->name();
		filename = get_full_path(filename);
		w = std::make_unique<ClusterWriter>(*v, filename, std::ios_base::out);
		w->open();
		w->set_events_header(header);
		w->write_events_header();
		writers.push_back(std::move(w));
	}
}


std::string ClusterEvents::get_full_path(std::string algo_pars){
	return Folders::ClusteredFolder+"/"+algo_pars+General::RootExtension;
}




void ClusterEvents::set_events_header_energy(General::energy_type energy){
	header.Energy = energy;
}

void ClusterEvents::set_events_header_source(std::string source){
	header.Source = source;
}

void ClusterEvents::set_events_header_description(std::string description){
	header.Description = description;
}

General::energy_type ClusterEvents::get_ttree_energy(TFile &f){
	auto tparameter = static_cast<TParameter<int>*>(f.Get(TTreeData::Energy.c_str()));
	return tparameter->GetVal();
}
