#include "cluster_events.h"

bool ClusterEvents::run_clustering(vec_cl_ptr &clustering_vec, Grid &g, unsigned long start, unsigned long end){

	if (clustering_vec.size() == 0) return false;
	for (auto &v : clustering_vec) v->set_grid(g);

	std::vector<std::unique_ptr<ClusterWriter>> writers;
	setup_writers(writers, clustering_vec);

	// Get tree and check for bounds
	std::unique_ptr<TTree> ttree(events_file->Get<TTree>(TREE_NAME.c_str()));
	if (end == 0) end = ttree->GetEntries();
	if (end - start <= 0) return false;

	// For each event, loop through clustering_vec and perform clustering
	for (long e = start; e < end; e++){
		g.fill_grid_ttree_entry(*ttree, e, true);
		cluster_event(clustering_vec);
		write_event(writers, e);
	}

	// Clean up
	for (auto &v : writers) v->close();
	return true;
}

bool ClusterEvents::cluster_event(vec_cl_ptr &clustering_vec){
	for (auto &v : clustering_vec) v->tag();
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
		w = std::make_unique<ClusterWriter>(*v, v->name(), std::ios_base::out);
		w->open();
		writers.push_back(std::move(w));
	}
}

bool ClusterEvents::open(){
	events_file = std::make_unique<TFile>(filename.c_str(), tfile_options.c_str());
	if (!events_file) return false;
	if (events_file->IsZombie()) return false;
	return true;
}

bool ClusterEvents::close(){

	// Close?

	return true;
}
