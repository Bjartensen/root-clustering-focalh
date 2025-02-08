#include "gaussian_analysis.h"

// TO-DO: The file was half-finished and didn't compile. I have made some changes just to make it compile, but it is NOT finished.

unsigned int vector_sum(TTreeClustered::value_type &vec);

void GaussianAnalysis::squawk(){
	std::cout << "Gaussian anlysis:";
	std::cout << std::endl;
	for (const auto &v : get_files()){
    std::cout << v;
    std::cout << std::endl;
  }
}

void GaussianAnalysis::begin_analysis(int clusters){
  // Loop through files
  for (const auto &v : get_files()){
    std::cout << "Analysing file " << v << std::endl;


    // For each file, loop through events
    ClusterReader reader(v);
    reader.open();
    reader.read_events_header();
    TTreeClustered::EventPtr event;
    std::vector<Cluster_Vec> events_vec;

    // For each event, loop through clusters
    while (reader.read_event(event)){
      // also just fill in the All tag with the sum

      Cluster_Map inner_map; // keeping track of sums for one event
      Cluster_Vec inner_vec;
      // vector so I can sort by largest?

      for (int i = 0; i < event.cluster->size(); i++){
        fill_map(inner_map, event.cluster->at(i), event.value->at(i));
      }

      // I want it as sorted vectors
      std::copy(inner_map.begin(), inner_map.end(), std::back_inserter(inner_vec));
      std::sort(inner_vec.begin(), inner_vec.end(), [](auto &left, auto &right){
          return vector_sum(left.second) > vector_sum(right.second);
          });

      events_vec.push_back(inner_vec);
    }


    for (auto &v : events_vec){
      // take top N clusters and sum and fit to histogram
      // call fit_gaussian?
      for (auto &c : v){
        std::cout << c.first << ", " << vector_sum(c.second) << std::endl;
      }
      break;
    }




    // For each cluster record sum and tag
    // All of this should be broken up into functions

  }
}


void GaussianAnalysis::fit_gaussian(std::vector<Cluster_Vec> &events, int min, int max, int clusters){
  std::unique_ptr<TH1D> hist = std::make_unique<TH1D>();

  for (auto &v : events){
    unsigned int sum = 0;
    for (int i = 0; i < clusters; i++){
      // TO-DO: Temporarily set to zero
      sum += 0;//vector_sum(c.second);
    }
    hist->Fill(sum);
  }


  if (min >= max || min == max){
    // full range
  }


  // if min >= max, do full range
  //
}



// map of string and vector?
// function that adds cluster tag and value
// if the tag exists, just adds to the vector
// if it doesn't exist add new entry to map
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


bool GaussianAnalysis::open(){

	clustered_tfile = std::make_unique<TFile>(filename.c_str(), "RECREATE");
	//ttree = std::make_unique<TTree>(TTreeClustered::TreeName.c_str(), TTreeClustered::TreeTitle.c_str());
	ttree = clustered_tfile->Get<TTree>(TTreeClustered::TreeName.c_str());
	//clustered_tfile->cd();

	//if (!set_ttree_branches()) return false;

	if (!clustered_tfile) return false;
	if (clustered_tfile->IsZombie()) return false;


	return true;
}

bool GaussianAnalysis::close(){
	clustered_tfile->cd();
	ttree->Write();
	//ttree->release();
	//clustered_tfile->Close();
	return true;
}
