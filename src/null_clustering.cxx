#include "null_clustering.h"

bool NullClustering::tag(){
  std::vector<Cell*> *cells;
  std::map<Cell*, std::string> *tagged_cells;
  cells = get_untagged_cells();
	tagged_cells = get_tagged_cells();
  while (cells->size() > 0){
    Cell *c = cells->at(0);
    tagged_cells->insert(std::pair(c,"1"));
    auto it = std::find(cells->begin(), cells->end(), c);
    if (it != cells->end()){
      cells->erase(it);
    }
  }
  return true;
}

std::string NullClustering::name(){
	const char DELIM = '_';
	std::string algo = "null";
	return algo;
}
