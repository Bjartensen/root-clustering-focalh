#include "grid.h"
#include <iostream>
#include "TStyle.h"

void Grid::make_cell(double _x, double _y, double _val, double _w, double _h){
	std::unique_ptr<Cell> c_ptr = std::make_unique<Cell>(_x, _y, _val, _w, _h);
	cells.push_back(std::move(c_ptr));
}

void Grid::make_cell(double _x, double _y, double _val, double _w, double _h, std::string _id){
	std::unique_ptr<Cell> c_ptr = std::make_unique<Cell>(_x, _y, _val, _w, _h, _id);
	cells.push_back(std::move(c_ptr));
}

Cell* Grid::get_cell(std::string _id){
	for (int i = 0; i < cells.size(); i++){
		if (cells.at(i)->get_id() == _id){
			return cells.at(i).get();
		}
	}
	return nullptr;
}

Cell* Grid::get_cell(General::float_type x, General::float_type y){
	for (auto &v : cells)
		if (v->hit(x, y)){
			return v.get();
		}
	return nullptr;
}

std::unique_ptr<TH2Poly> Grid::plot_grid(){
	std::unique_ptr<TH2Poly> ptr = std::make_unique<TH2Poly>();


	const double margin = 0.02; // temporary
	const unsigned short vertices = 4;

	std::vector<double> x;
	std::vector<double> y;

	for (auto &v : cells){
		// Quadrant I
		x.push_back(v->get_x() + (v->get_width()/2 - margin));
		y.push_back(v->get_y() + (v->get_height()/2 - margin));

		// Quadrant II
		x.push_back(v->get_x() - (v->get_width()/2 - margin));
		y.push_back(v->get_y() + (v->get_height()/2 - margin));


		// Quadrant III
		x.push_back(v->get_x() - (v->get_width()/2 - margin));
		y.push_back(v->get_y() - (v->get_height()/2 - margin));

		// Quadrant IV
		x.push_back(v->get_x() + (v->get_width()/2 - margin));
		y.push_back(v->get_y() - (v->get_height()/2 - margin));

		ptr->AddBin(vertices, &x[0], &y[0]);
		ptr->Fill(v->get_x(), v->get_y(), v->get_value());

		x.clear();
		y.clear();

	}

	//ptr->GetXaxis()->SetTitle("x [cm]");
	//ptr->GetYaxis()->SetTitle("y [cm]");
	//ptr->SetStats(0);
	//ptr->SetOptTitle("adsf");
	return std::move(ptr);
}


std::unique_ptr<TH2Poly> Grid::plot_cell_neighbors(Cell* c){
	std::unique_ptr<TH2Poly> ptr = std::make_unique<TH2Poly>();

	const double margin = 0.02; // temporary
	const unsigned short vertices = 4;

	std::vector<double> x;
	std::vector<double> y;

	for (auto &v : cells){
		// Quadrant I
		x.push_back(v->get_x() + (v->get_width()/2 - margin));
		y.push_back(v->get_y() + (v->get_height()/2 - margin));

		// Quadrant II
		x.push_back(v->get_x() - (v->get_width()/2 - margin));
		y.push_back(v->get_y() + (v->get_height()/2 - margin));


		// Quadrant III
		x.push_back(v->get_x() - (v->get_width()/2 - margin));
		y.push_back(v->get_y() - (v->get_height()/2 - margin));

		// Quadrant IV
		x.push_back(v->get_x() + (v->get_width()/2 - margin));
		y.push_back(v->get_y() - (v->get_height()/2 - margin));

		ptr->AddBin(vertices, &x[0], &y[0]);

		x.clear();
		y.clear();

	}

	//ptr->GetXaxis()->SetTitle("x [cm]");
	//ptr->GetYaxis()->SetTitle("y [cm]");
	ptr->Fill(c->get_x(), c->get_y(), 1);
	for (auto n : *c->get_neighbors())
		ptr->Fill(n->get_x(), n->get_y(), 2);

	return std::move(ptr);
}

bool Grid::SetCellValues(double _x, double _y, double _val){
	for (auto &v : cells)
		if (v->hit(_x, _y)){
			v->set_value(_val);
			return true;
		}
	return false;
}

bool Grid::Fill(double _x, double _y, double _val){
	for (auto &v : cells)
		if (v->hit(_x, _y)){
			v->set_value(v->get_value()+_val);
			return true;
		}
	return false;
}


double Grid::get_grid_sum(){
	double sum = 0;
	for (const auto &v : cells){
		sum += v->get_value();
	}
	return sum;
}



// TO-DO: Delete in favor of event_ptr version
bool Grid::fill_grid_ttree_entry(TTree& t, int e, bool override_values){

	std::vector<Double_t>* x_pos = nullptr;
	std::vector<Double_t>* y_pos = nullptr;
	std::vector<Double_t>* value = nullptr;
	//std::vector<Double_t>* particle = nullptr;


	t.SetBranchAddress(TFileGeneric::x_branch.c_str(), &x_pos);
	t.SetBranchAddress(TFileGeneric::y_branch.c_str(), &y_pos);
	t.SetBranchAddress(TFileGeneric::value_branch.c_str(), &value);
	//t.SetBranchAddress("particle", &particle);

	t.GetEntry(e);

	for (int i = 0; i < x_pos->size(); i++){
		if (override_values)
			SetCellValues(x_pos->at(i), y_pos->at(i), value->at(i));
		else
			Fill(x_pos->at(i), y_pos->at(i), value->at(i));
	}

	return true;
}



bool Grid::fill_grid_event_ptr(TFileGeneric::EventPtr &ptr, bool override_values){
	for (int i = 0; i < ptr.x->size(); i++){
		if (override_values){
			SetCellValues(ptr.x->at(i), ptr.y->at(i), ptr.value->at(i));
    }
		else{
			Fill(ptr.x->at(i), ptr.y->at(i), ptr.value->at(i));
    }
	}
	return true;
}
