#ifndef CELL_H
#define CELL_H

#include <vector>
#include <string>


/*
 * class Cell
 * x
 * y
 * value
 * References to adjecent cells
 *
 */

//class Cell;

class Cell{
	double x_position, y_position, value, width, height;
	// consider vectors with polygonal coordinates
	std::vector<Cell*> neighbors;
	std::string tag;
	std::string id; // make code checking uniqueness, hash table?

public:
	Cell() : x_position(0), y_position(0), value(0) , width(1), height(1) {}
	//Cell(double _x, double _y) : x_position(_x), y_position(_y), value(0) {}
	//Cell(double _x, double _y, double value) : x_position(_x), y_position(_y), value(0) {}
	Cell(double _x, double _y, double _val, double _w, double _h)
		: x_position(_x), y_position(_y), value(_val), width(_w), height(_h) {}

	Cell(double _x, double _y, double _val, double _w, double _h, std::string _id)
		: x_position(_x), y_position(_y), value(_val), width(_w), height(_h), id(_id) {}
	
	
	// Setters
	void set_x(double _x) {x_position=_x;}
	void set_y(double _y) {y_position=_y;}
	void set_value(double _val) {value=_val;}
	void set_width(double _w) {width=_w;}
	void set_height(double _h) {height=_h;}
	void set_tag(std::string _t) {tag=_t;}

	// Getters
	double get_x() {return x_position;}
	double get_y() {return y_position;}
	double get_value() {return value;}
	double get_width() {return width;}
	double get_height() {return height;}
	std::vector<Cell*> * get_neighbors() {return &neighbors;}
	std::string get_tag() {return tag;}
	std::string get_id() {return id;}
	

	bool add_neighbor(Cell *n);
	void squawk();

	bool hit(double _x, double _y);
};

#endif // CELL_H
