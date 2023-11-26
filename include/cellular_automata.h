#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

#include "clustering.h"

class CellularAutomata : public Clustering{

public:
	CellularAutomata(Grid *_g) : Clustering(_g) {}

	void tag();
	bool find_maxima();
	bool is_maximum(Cell *c);
	static bool greater(Cell* a, Cell* b){return (a->get_value() > b->get_value());}
};

#endif // CELLULAR_AUTOMATA_H
