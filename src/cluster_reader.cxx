#include "cluster_reader.h"


#include <iostream>

bool ClusterReader::open(){
	clustered_tfile = std::make_unique<TFile>(filename.c_str(), "READ");
	if (clustered_tfile->IsZombie()) return false;
	ttree = clustered_tfile->Get<TTree>(TTreeClustered::TreeName.c_str());
	if (!ttree) return false;
	set_ttree_branches();

	return true;
}

bool ClusterReader::close(){
	clustered_tfile->Close();

	return true;
}

bool ClusterReader::read_event(TTreeClustered::EventPtr &ev){
	if (entry >= ttree->GetEntries()) return false;
	ttree->GetEntry(entry);
	ev = event;
	entry++;
	return true;
}

bool ClusterReader::read_event(TTreeClustered::EventPtr &ev, unsigned long en){
	if (en >= ttree->GetEntries()) return false;
	ttree->GetEntry(en);
	ev = event;
	return true;
}

void ClusterReader::set_ttree_branches(){
	ttree->SetBranchAddress(TTreeClustered::x_branch.c_str(), &event.x);
	ttree->SetBranchAddress(TTreeClustered::y_branch.c_str(), &event.y);
	ttree->SetBranchAddress(TTreeClustered::value_branch.c_str(), &event.value);
	ttree->SetBranchAddress(TTreeClustered::class_branch.c_str(), &event.class_label);
	ttree->SetBranchAddress(TTreeClustered::cluster_branch.c_str(), &event.cluster);
}


TTree* ClusterReader::get_ttree(){
	return ttree;
}


void ClusterReader::read_events_header(){

	// Read objects
	
	// Get TParameter for energy
	// Get source and use name
	// Get description and use name


	//TObject &energy = *clustered_tfile->Get(General::energy_tparameter.c_str());
	TObject &source = *clustered_tfile->Get(General::source_tobj.c_str());
	TObject &description = *clustered_tfile->Get(General::description_tobj.c_str());

	//header.Energy = static_cast<TParameter<int>*>(energy).GetVal();
	header.Energy = static_cast<TParameter<int>*>(clustered_tfile->Get(General::energy_tparameter.c_str()))->GetVal();
	header.Source = source.GetName();
	header.Description = description.GetName();


}


General::EventsHeader& ClusterReader::get_events_header(){
	return header;
}
