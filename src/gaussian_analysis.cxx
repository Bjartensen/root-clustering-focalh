#include "gaussian_analysis.h"

unsigned int vector_sum(TTreeClustered::value_type &vec);

void GaussianAnalysis::squawk(){
	std::cout << "Gaussian anlysis:";
	std::cout << std::endl;
	for (const auto &v : get_files()){
    std::cout << v;
    std::cout << std::endl;
  }
}

void GaussianAnalysis::begin_analysis(){
  // Loop through files
  for (const auto &v : get_files()){
    std::cout << "Analysing file " << v << std::endl;



    // For each file, loop through events
    ClusterReader reader(v);
    reader.open();
    reader.read_events_header();
    TTreeClustered::EventPtr event;

    // map of string and vector?
    // function that adds cluster tag and value
    // if the tag exists, just adds to the vector
    // if it doesn't exist add new entry to map


    // inner and outer collection? They sort of are equivalent:
    // for the entire file, I want to calculate sums for each cluster
    // but those sums come from looking at each event and calculating a sum
    // for each cluster in the event...

    Cluster_Map outer_map; // keeping track of sums for one file

    // For each event, loop through clusters
    while (reader.read_event(event)){
      // event.cluster is the cluster
      // but it is not ordered
      // I need some function that adds to a collection
      // and when new creates a new

      // also just fill in the All tag with the sum


      Cluster_Map inner_map; // keeping track of sums for one event


      for (int i = 0; i < event.cluster->size(); i++){
        fill_map(inner_map, event.cluster->at(i), event.value->at(i));
      }

      for (auto &c : inner_map){
        fill_map(outer_map, c.first, vector_sum(c.second));
      }
    }

    std::cout << outer_map.size() << std::endl;


    for (auto &c : outer_map){
      std::cout << c.first << ", " << c.second.size() << ", " << vector_sum(c.second) << std::endl;
    }


    // For each cluster record sum and tag
    // All of this should be broken up into functions

  }
}



void GaussianAnalysis::fill_map(Cluster_Map &map, std::string tag, General::adc_type val){
  auto it = map.find(tag);
  if (it == map.end()){
    std::vector<General::adc_type> vec;
    vec.push_back(val);
    map.insert(std::make_pair(tag, vec));
  }else{
    it->second.push_back(val);
  }
}


unsigned int vector_sum(TTreeClustered::value_type &vec){
  unsigned int sum = 0;
  for (const auto &v : vec)
    sum += v;
  return sum;
}


