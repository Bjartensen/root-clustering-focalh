#include "cluster_writer.h"
#include "definitions.h"


#include <iostream>

bool ClusterWriter::open(){

	clustered_tfile = std::make_unique<TFile>(filename.c_str(), "RECREATE");
	ttree = std::make_unique<TTree>(TFileGeneric::TreeName.c_str(), TFileGeneric::TreeTitle.c_str());
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


bool ClusterWriter::write_event(TFileGeneric::EventPtr &ptr){

	clustered_tfile->cd(); // I think I need this

	// Write event header

	clear_containers();
	auto cells = clustering.get_tagged_cells();
  Grid *g = clustering.get_grid();



  *event.x = *ptr.x;
  *event.y = *ptr.y;
  *event.value = *ptr.value;
  *event.energies = *ptr.energies;

  *event.labels = *ptr.labels;
  *event.label_idx = *ptr.label_idx;
  *event.fractions = *ptr.fractions;
  


  for (int i = 0; i < ptr.x->size(); i++){

    Cell* c = g->get_cell(ptr.x->at(i), ptr.y->at(i));
		auto it = cells->find(c);
    if (it != cells->end()) {
      // TO-DO: Clusters should just be integers...
      event.clusters->push_back(std::stoi(it->second));
      event.cluster_idx->push_back(i);
    }else{
      event.clusters->push_back(0);
      event.cluster_idx->push_back(i);
    }
  }


	ttree->Fill();
	//ttree->Write();
	//clustered_tfile->Write();

	return true;
}



bool ClusterWriter::set_ttree_branches(){

	ttree->Branch(TFileGeneric::x_branch.c_str(), &event.x);
	ttree->Branch(TFileGeneric::y_branch.c_str(), &event.y);
	ttree->Branch(TFileGeneric::value_branch.c_str(), &event.value);
	ttree->Branch(TFileGeneric::energies_branch.c_str(), &event.energies);

	ttree->Branch(TFileGeneric::label_branch.c_str(), &event.labels);
	ttree->Branch(TFileGeneric::label_idx_branch.c_str(), &event.label_idx);
	ttree->Branch(TFileGeneric::fractions_branch.c_str(), &event.fractions);

	ttree->Branch(TFileGeneric::clusters_branch.c_str(), &event.clusters);
	ttree->Branch(TFileGeneric::cluster_idx_branch.c_str(), &event.cluster_idx);

	return true;
}


bool ClusterWriter::clear_containers(){

  // event.x->...

  event.x->clear();
	event.y->clear();
	event.value->clear();
	event.labels->clear();
	event.clusters->clear();
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
