#ifndef MODIFIED_AGGREGATION_H
#define MODIFIED_AGGREGATION_H

#include "clustering.h"


// Move to own file at some point
class ModifiedAggregation : public Clustering{
	double seed_threshold;
	double aggregation_threshold;
public:
	ModifiedAggregation(Grid &_g, double _seed, double _agg) : Clustering(_g), seed_threshold(_seed), aggregation_threshold(_agg) {}

	ModifiedAggregation(double _seed, double _agg) : Clustering(), seed_threshold(_seed), aggregation_threshold(_agg) {}

	bool tag();
	std::string name();
	//bool find_maximum(std::vector<Cell*> *pool);
	//bool is_maximum(Cell *c);
	static bool greater(Cell* a, Cell* b){return (a->get_value() > b->get_value());}
	bool spread(Cell* c);

	Cell* find_maximum_untagged();

};

#endif // MODIFIED_AGGREGATION_H
