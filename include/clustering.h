#ifndef CLUSTERING_H
#define CLUSTERING_H

#include "grid.h"
#include <memory>
#include <map>
#include <set>

/*
 *	Abstract class for clustering algorithms.
 *	This class will contain evaluation of performance of clustering.
 *
 *	Derived classes must implement tag().
 */
class Clustering{
private:
	Grid* g;
	//std::map<std::string, Cell*> tagged_cells;
	std::vector<Cell*> untagged_cells;
	std::map<Cell*, std::string> tagged_cells;

public:
	Clustering(Grid *_g);
	virtual void tag() = 0;

	Grid* get_grid(){return g;}

	void add_tag(std::string tag, Cell* c);

	bool is_tagged(Cell* c);


	Cell* find_maximum_untagged();


	std::vector<Cell*>* get_untagged_cells(){return &untagged_cells;}
	std::map<Cell*, std::string>* get_tagged_cells(){return &tagged_cells;}
	std::string get_tag(Cell* c){return tagged_cells[c];}

	void print_tagged();
	void print_untagged();


	double get_sum();
	int get_cluster_count();
	double get_cluster_sum(std::string tag);
	double get_cluster_max_adc(std::string tag);
	std::vector<std::string> get_clusters();
	std::string get_largest_cluster();
	std::vector<std::string> get_remaining_clusters(std::vector<std::string> &exclude_clusters);

};


#endif // CLUSTERING_H

#ifndef CELLULAR_AUTOMATA_H
#define CELLULAR_AUTOMATA_H

// Move to own file at some point
class CellularAutomata : public Clustering{

public:
	CellularAutomata(Grid *_g) : Clustering(_g) {}

	void tag();
	bool find_maxima();
	bool is_maximum(Cell *c);
	static bool greater(Cell* a, Cell* b){return (a->get_value() > b->get_value());}
};

#endif // CELLULAR_AUTOMATA_H


#ifndef MODIFIED_AGGREGATION_H
#define MODIFIED_AGGREGATION_H

// Move to own file at some point
class ModifiedAggregation : public Clustering{
	double seed_threshold;
	double aggregation_threshold;
public:
	ModifiedAggregation(Grid *_g, double _seed, double _agg) : Clustering(_g), seed_threshold(_seed), aggregation_threshold(_agg) {}

	void tag();
	//bool find_maximum(std::vector<Cell*> *pool);
	//bool is_maximum(Cell *c);
	static bool greater(Cell* a, Cell* b){return (a->get_value() > b->get_value());}
	bool spread(Cell* c);

	Cell* find_maximum_untagged();

};

#endif // MODIFIED_AGGREGATION_H
