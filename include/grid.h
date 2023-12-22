#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include <memory>
#include <TH2Poly.h>


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
	// setup
	// something that fills
	bool Fill(double _x, double _y, double _val);
	bool SetCellValues(double _x, double _y, double _val);
	double get_grid_sum();

	// TH2Poly plotting
	std::unique_ptr<TH2Poly> plot_grid();
	std::unique_ptr<TH2Poly> plot_cell_neighbors(Cell* c);

	std::vector<std::unique_ptr<Cell>>* get_cells() {return &cells;}

};

#endif // GRID_H
