#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include <memory>
#include <TH2Poly.h>
#include <TTree.h>
#include "definitions.h"


/*
 * class Grid
 * Cells
 */

class Grid{
	std::vector<std::unique_ptr<Cell>> cells;
	// std::unordered_map for storing the unique cell names
	
public:

	Grid() {}

	void make_cell(double _x, double _y, double _val, double _w, double _h);
	void make_cell(double _x, double _y, double _val, double _w, double _h, std::string _id);

	Cell* get_cell(std::string _id);
  Cell* get_cell(General::float_type x, General::float_type y);
	// setup
	// something that fills
	bool Fill(double _x, double _y, double _val);
	bool SetCellValues(double _x, double _y, double _val);
	double get_grid_sum();

	// TH2Poly plotting
	std::unique_ptr<TH2Poly> plot_grid();
	std::unique_ptr<TH2Poly> plot_cell_neighbors(Cell* c);

	std::vector<std::unique_ptr<Cell>>* get_cells() {return &cells;}


	//bool fill_grid_ttree_entry(TTree& t, Grid& g, int e, bool override_values);
	bool fill_grid_ttree_entry(TTree& t, int e, bool override_values);
	// This needs to be cleaned up. It's because the already-created testbeam
	// and simulation files have different branch addresses than the one I have
	// now defined in the definitions.h
	bool fill_grid_ttree_entry2(TTree& t, int e, bool override_values);
  bool fill_grid_event_ptr(TFileGeneric::EventPtr &ptr, bool override_values);
};

#endif // GRID_H
