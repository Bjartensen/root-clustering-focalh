#include <iostream>
#include "cell.h"


void Cell::squawk(){
	std::cout << id << " => ";
	std::cout << "x:" << x_position << " y:" << y_position;
	std::cout << " val:" << value << std::endl;

	for (auto v : neighbors){
		std::cout << "\t";
		v->squawk();
	}
}


bool Cell::add_neighbor(Cell *n){
	if (!n) return false;
	neighbors.push_back(n);
	return true;
}


bool Cell::hit(double _x, double _y){
	if (std::abs(_x - x_position) > width/2)
		return false;

	if (std::abs(_y - y_position) > height/2)
		return false;

	return true;
}

