#include "cellular_automata.h"

void CellularAutomata::tag(){
	// Always go through all cells and check:
	//	Do they have neighbors?
	//		Do the neighbors have tags?
	//			Add tags.

}

bool CellularAutomata::find_maxima(){
	std::vector<Cell*> c;
	for (auto &v : *get_grid()->get_cells()){
		c.push_back(v.get());
	}
	std::sort(c.begin(), c.end(), greater);

	// can compare pointers
	Cell* max = c.at(0);
	for (auto v : c){
		if (is_maximum(v))
			std::cout << "Maximum at " << v->get_id() << ": " << v->get_value() << std::endl;
	}


	return true;
}

bool CellularAutomata::is_maximum(Cell *c){
	for (auto v : *c->get_neighbors())
		if (v->get_value() > c->get_value()) // 4095 may be next to each other...
			return false;
	return true;
}
