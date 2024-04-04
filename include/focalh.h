#ifndef FOCALH
#define FOCALH

#include "geometry.h"

class FoCalH : public Geometry{
private:
	const double detector_width = 19.5;
	const double detector_height = 19.5;
	const unsigned int module_columns = 3;
	const unsigned int module_rows = 3;

	const unsigned int center_module_rows = 7;
	const unsigned int center_module_cols = 7;
	const unsigned int outer_module_rows = 5;
	const unsigned int outer_module_cols = 5;

public:
	FoCalH();
	bool construct_geometry();

	// Constructs the geometry of FoCalH Prototype-2. Uses the size and number of
	// modules and cells in modules to construct the cells.
	bool make_cells();


	// Computes the neighbors for a cell and assigns the neighbors.
	bool calc_neighbors_for_cell(Grid &g, Cell &c);


	// Loops through all cells and calculates and sets neighbors.
	bool calc_neighbors(Grid &g);

};







#endif // FOCALH
