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


	return true;
}

bool ClusterWriter::close(){
	clustered_tfile->cd();
	ttree->Write();
	ttree.release();
	//clustered_tfile->Close();
	return true;
}


bool ClusterWriter::write_event(const long event_number){


	clustered_tfile->cd(); // I think I need this


	// Write event header
	//write_event_header(event_number);

	// Write event
	//for (auto it = cells.begin(); it != cells.end(); it++){
	

	// I think I should get the Grid and loop over all cells
	// and try and match them with Clustering tagged cells.
	// If they are not found I set the tag to 0 or -1.


	clear_containers();
	auto cells = clustering.get_tagged_cells();
  Grid *g = clustering.get_grid();

  
	for (auto &c : *g->get_cells()){

		x_pos.push_back(c->get_x());
		y_pos.push_back(c->get_y());
		value.push_back(c->get_value());
		class_label.push_back(1);


		auto it = cells->find(c.get());

    if (it != cells->end()) {
      cluster.push_back(it->second);
    }else{
      cluster.push_back("0");

    }

    // Search through clustering cells and find label
    //cluster.push_back(c.second);


  }



  /*
	for (auto it = cells->begin(); it != cells->end(); it++){
	//for (auto it = cells->begin(); it != cells->end(); it++){
		//write_event_line(it->first->get_x(), it->first->get_y(), it->first->get_value(), it->second);
		
		x_pos.push_back(it->first->get_x());
		y_pos.push_back(it->first->get_y());
		value.push_back(it->first->get_value());
		class_label.push_back(1);
		cluster.push_back(it->second);
	}
  */


	ttree->Fill();
	//ttree->Write();
	//clustered_tfile->Write();

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

void ClusterWriter::set_events_header(General::EventsHeader &h){
	header = h;
}

void ClusterWriter::write_events_header(){
	TParameter<General::energy_type> energy(General::energy_tparameter.c_str(), header.Energy);
	energy.Write();
	TObjString source(header.Source.c_str());
	source.Write(General::source_tobj.c_str());
	TObjString description(header.Description.c_str());
	description.Write(General::description_tobj.c_str());
}
