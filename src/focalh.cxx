#include "focalh.h"


FoCalH::FoCalH(){
	construct_geometry();
	calc_neighbors(get_grid());
}



bool FoCalH::construct_geometry(){
	// Constructs the cells
	if (!make_cells()) return false;

	// Assigns the neighborhoods



	return true;
}

bool FoCalH::make_cells(){

	// The sides of the modules are 1.5mm copper plates.
	// The modules have a 3mm margin.
	// Modules are 65x65mm^2 with plate

	const double module_width = detector_width/module_columns;
	const double module_height = detector_height/module_rows;

	const std::pair<double, double> center(0.0, 0.0);
	const std::pair<double, double> module00(center.first - module_width, center.second + module_height);
	const std::pair<double, double> module01(center.first, center.second + module_height);
	const std::pair<double, double> module02(center.first + module_width, center.second + module_height);
	const std::pair<double, double> module10(center.first - module_width, center.second);
	const std::pair<double, double> module12(center.first + module_width, center.second);
	const std::pair<double, double> module20(center.first - module_width, center.second - module_height);
	const std::pair<double, double> module21(center.first, center.second - module_height);
	const std::pair<double, double> module22(center.first + module_width, center.second - module_height);

	std::vector<std::pair<double, double>> outer_modules;
	outer_modules.push_back(module00);
	outer_modules.push_back(module01);
	outer_modules.push_back(module02);
	outer_modules.push_back(module10);
	outer_modules.push_back(module12);
	outer_modules.push_back(module20);
	outer_modules.push_back(module21);
	outer_modules.push_back(module22);
	std::vector<std::string> outer_board_names = {"B00", "B01", "B02", "B10", "B12", "B20", "B21", "B22"};

	std::string center_board_name = "B11";
	std::string center_cell_temp_name = "";
	// Center module
	for (int r = 0; r < center_module_rows; r++)
		for (int c = 0; c < center_module_cols; c++){
			double dx = module_width/center_module_rows;
			double x = center.first + double(c-3)*dx;

			double dy = module_height/center_module_cols;
			double y = center.second + double(r-3)*dy;

			center_cell_temp_name = center_board_name+"C"+std::to_string(r)+std::to_string(c);
			get_grid().make_cell(x,y,0,dx,dy,center_cell_temp_name);
		}


	std::string outer_cell_temp_name = "";
	for (int i = 0; i < outer_modules.size(); i++){
		for (int r = 0; r < outer_module_rows; r++)
			for (int c = 0; c < outer_module_cols; c++){
				double dx = module_width/outer_module_rows;
				double x = outer_modules.at(i).first + double(c-2)*dx;

				double dy = module_height/outer_module_cols;
				double y = outer_modules.at(i).second + double(r-2)*dy;
				outer_cell_temp_name = outer_board_names.at(i)+"C"+std::to_string(r)+std::to_string(c);
				get_grid().make_cell(x,y,0,dx,dy,outer_cell_temp_name);
			}
	}


	return true;

	
	//

}


// Computes neighbors of cell by going a small distance in 8 directions
// Reason for doing this is that center module and outer module cells
// have different sizes, so going a cell width to the left does not
// necessarily bring you to the center of a cell.
bool FoCalH::calc_neighbors_for_cell(Grid &g, Cell &c){

	const double tol = 0.000001;

	// Cardinal directions
	std::pair<double, double> E = {c.get_x() + c.get_width()/2, c.get_y()};
	std::pair<double, double> NE = {c.get_x() + c.get_width()/2, c.get_y() + c.get_height()/2};
	std::pair<double, double> N = {c.get_x(), c.get_y() + c.get_height()/2};
	std::pair<double, double> NW = {c.get_x() - c.get_width()/2, c.get_y() + c.get_height()/2};
	std::pair<double, double> W = {c.get_x() - c.get_width()/2, c.get_y()};
	std::pair<double, double> SW = {c.get_x() - c.get_width()/2, c.get_y() - c.get_height()/2};
	std::pair<double, double> S = {c.get_x(), c.get_y() - c.get_height()/2};
	std::pair<double, double> SE = {c.get_x() + c.get_width()/2, c.get_y() - c.get_height()/2};


	for (auto &v : *g.get_cells())
		if (
			v->hit(E.first+tol,E.second)
			|| v->hit(NE.first+tol,NE.second+tol)
			|| v->hit(N.first,N.second+tol)
			|| v->hit(NW.first-tol,NW.second+tol)
			|| v->hit(W.first-tol,W.second)
			|| v->hit(SW.first-tol,SW.second-tol)
			|| v->hit(S.first,S.second-tol)
			|| v->hit(SE.first+tol,SE.second-tol)
		   )
			c.add_neighbor(v.get());


	return true;
}


bool FoCalH::calc_neighbors(Grid &g){
	for (auto &v : *g.get_cells())
		if (!calc_neighbors_for_cell(g, *v)) return false;
	return true;
}

