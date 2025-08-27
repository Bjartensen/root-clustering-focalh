#include "null_clustering.h"
#include <iostream>
#include "focalh.h"
#include "cluster_events.h"

int main(int argc, char* argv[]){


  if (argc != 3){
    std::cout << "Requires one input file and one output file name." << std::endl;
    return 0;
  }

	FoCalH focal;
	Grid &g = focal.get_grid();

	std::cout << "Null clustering" << std::endl;


	// Prepare Clustering objects
	std::vector<std::unique_ptr<Clustering>> clustering_vec;
	std::unique_ptr<Clustering> null;
	null = std::make_unique<NullClustering>();
	clustering_vec.push_back(std::move(null));

  std::cout << "Opening ..." << std::endl;
	ClusterEvents clusterizer(argv[1], argv[2]);
	clusterizer.open();
  std::cout << "Opened." << std::endl;
	clusterizer.set_events_header_source("mc");
	clusterizer.set_events_header_description("Geant4 Monte-Carlo");
  std::cout << "Running clustering..." << std::endl;
	clusterizer.run_clustering(clustering_vec, g, 0, 0);
  std::cout << "Clustered." << std::endl;
	clusterizer.close();

  std::cout << "Closing ..." << std::endl;

  return 0;
}
