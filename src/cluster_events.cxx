#include "cluster_events.h"

#include <iostream>

bool ClusterEvents::open(){
	events_file = std::make_unique<TFile>(in_filename.c_str(), tfile_options.c_str());
	if (!events_file) return false;
	if (events_file->IsZombie()) return false;
  in_ttree = events_file->Get<TTree>(TFileGeneric::TreeName.c_str());
  if (!in_ttree) return false;
	return true;
}

bool ClusterEvents::close(){
	events_file->Close();
	return true; // Meaningless...
}

bool ClusterEvents::run_clustering(vec_cl_ptr &clustering_vec, Grid &g, unsigned long start, unsigned long end){


	if (clustering_vec.size() == 0) return false;


  set_in_ttree_branches();

	std::vector<std::unique_ptr<ClusterWriter>> writers;
	setup_writers(writers, clustering_vec, out_filename);


	// Get tree and check for bounds
	if (end == 0) end = in_ttree->GetEntries();
	if (end - start <= 0) return false;

	// For each event, loop through clustering_vec and perform clustering
	for (long e = start; e < end; e++){
    in_ttree->GetEntry(e);
    g.fill_grid_event_ptr(in_event, true);
		for (auto &v : clustering_vec) v->set_grid(g); // TO-DO: Performance hit?
		cluster_event(clustering_vec);
		write_event(writers, in_event); // TO-DO: Here I want to supply EventPtr?
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


bool ClusterEvents::write_event(std::vector<std::unique_ptr<ClusterWriter>> &writers, TFileGeneric::EventPtr &ptr){
	for (auto &v : writers) v->write_event(ptr);
	return true;
}


void ClusterEvents::setup_writers(
		std::vector<std::unique_ptr<ClusterWriter>> &writers,
		vec_cl_ptr &clustering_vec, std::string filename){

	for (auto &v : clustering_vec){
		std::unique_ptr<ClusterWriter> w;
    std::string temp_filename = filename;
		temp_filename += "_"+v->name();
		temp_filename = get_full_path(temp_filename);
		w = std::make_unique<ClusterWriter>(*v, temp_filename, std::ios_base::out);
		w->open();
		w->set_events_header(header);
		w->write_events_header();
		writers.push_back(std::move(w));
	}
}


std::string ClusterEvents::get_full_path(std::string algo_pars){
	return Folders::ClusteredFolder+"/"+algo_pars+General::RootFileExtension;
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


void ClusterEvents::set_in_ttree_branches(){
  in_ttree->SetBranchAddress(TFileGeneric::x_branch.c_str(), &in_event.x);
  in_ttree->SetBranchAddress(TFileGeneric::y_branch.c_str(), &in_event.y);
  in_ttree->SetBranchAddress(TFileGeneric::value_branch.c_str(), &in_event.value);
  in_ttree->SetBranchAddress(TFileGeneric::label_branch.c_str(), &in_event.labels);
  in_ttree->SetBranchAddress(TFileGeneric::label_idx_branch.c_str(), &in_event.label_idx);
  in_ttree->SetBranchAddress(TFileGeneric::fractions_branch.c_str(), &in_event.fractions);
  in_ttree->SetBranchAddress(TFileGeneric::energies_branch.c_str(), &in_event.energies);
}
