#include "clustering.h"
#include <iostream>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include "TStyle.h"
#include "TColor.h"
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TParameter.h>
#include <TLegend.h>
#include <filesystem>
#include <regex>

const double SATURATION_VALUE = 4095;

bool focal_h_grid(Grid& g);
bool fill_grid_ttree_entry(TTree& t, Grid& g, int e, bool override_values = false);
bool focal_h_grid_add_neighbors_cell(Grid& g, std::string cell, std::initializer_list<std::string> neighbors);
bool focal_h_grid_add_neighbors_all(Grid &g);
bool calc_neighbors(Grid &g, Cell *c);
bool calc_neighbors_all(Grid &g);
std::unique_ptr<TGraphErrors> plot_points(std::vector<double> &x, std::vector<double> &y, std::vector<double> &y_err);
bool MA_analysis(std::string folder, double MA_seed_threshold, double MA_aggregation_threshold,
	std::vector<double> &energy, 
	std::vector<double> &cluster_count,
	std::vector<double> &fraction_of_energy,
	std::vector<double> &leftover_max_adc,
	std::vector<double> &cluster_count_err,
	std::vector<double> &fraction_of_energy_err,
	std::vector<double> &leftover_max_adc_err
);
double mean(std::vector<double> &vec);
double standard_error(std::vector<double> &samples);
void save_point_plot(std::vector<std::unique_ptr<TGraphErrors>> &point_plots, std::string x_label, std::string y_label, std::string legend_title, std::string filename);
void save_heatmap(Grid &g, std::string filename, std::string title);
std::vector<std::string> get_files(std::string folder);

int main(int argc, char* argv[]){

	std::vector<std::pair<double, double>> MA_params;
	MA_params.push_back(std::make_pair(100, 10));
	MA_params.push_back(std::make_pair(300, 10));
	MA_params.push_back(std::make_pair(500, 10));
	MA_params.push_back(std::make_pair(800, 10));
	MA_params.push_back(std::make_pair(1000, 10));

	std::string folder = "../data/focalsim/pi_plus_100e_deg0/";

	//std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 800, 800);
	//std::unique_ptr<TMultiGraph> multi = std::make_unique<TMultiGraph>();
	std::vector<std::unique_ptr<TGraphErrors>> cluster_count_points_vec;
	std::vector<std::unique_ptr<TGraphErrors>> fraction_of_energy_points_vec;
	std::vector<std::unique_ptr<TGraphErrors>> leftover_max_adc_points_vec;
	//c->cd();

	for (int i = 0; i < MA_params.size(); i++){
		std::vector<double> energy;
		std::vector<double> cluster_count;
		std::vector<double> fraction_of_energy;
		std::vector<double> leftover_max_adc;

		std::vector<double> cluster_count_err;
		std::vector<double> fraction_of_energy_err;
		std::vector<double> leftover_max_adc_err;

		double MA_seed_threshold = MA_params.at(i).first;
		double MA_aggregation_threshold = MA_params.at(i).second;
		std::string MA_params_desc = "Seed thld: " + std::to_string(int(MA_seed_threshold)) + ", agg thld: " + std::to_string(int(MA_aggregation_threshold));
		std::string MA_params_short = "MA_"+std::to_string(int(MA_seed_threshold))+"_"+std::to_string(int(MA_aggregation_threshold));

		MA_analysis(folder, MA_seed_threshold, MA_aggregation_threshold,
					energy,
					cluster_count,
					fraction_of_energy,
					leftover_max_adc,
					cluster_count_err,
					fraction_of_energy_err,
					leftover_max_adc_err
				);

		// Cluster count
		std::unique_ptr<TGraphErrors> cluster_count_points = plot_points(energy, cluster_count, cluster_count_err);
		cluster_count_points->SetMarkerStyle(21+i);
		cluster_count_points->SetMarkerColor(1+i);
		cluster_count_points->SetName(MA_params_desc.c_str());
		cluster_count_points_vec.push_back(std::move(cluster_count_points));

		// Fraction of total
		std::unique_ptr<TGraphErrors> fraction_of_energy_points = plot_points(energy, fraction_of_energy, fraction_of_energy_err);
		fraction_of_energy_points->SetMarkerStyle(21+i);
		fraction_of_energy_points->SetMarkerColor(1+i);
		fraction_of_energy_points->SetName(MA_params_desc.c_str());
		fraction_of_energy_points_vec.push_back(std::move(fraction_of_energy_points));

		// Leftover max ADC
		std::unique_ptr<TGraphErrors> leftover_max_adc_points = plot_points(energy, leftover_max_adc, leftover_max_adc_err);
		leftover_max_adc_points->SetMarkerStyle(21+i);
		leftover_max_adc_points->SetMarkerColor(1+i);
		leftover_max_adc_points->SetName(MA_params_desc.c_str());
		leftover_max_adc_points_vec.push_back(std::move(leftover_max_adc_points));
	}

	std::string cluster_count_filename = folder + "cluster_count.png";
	save_point_plot(cluster_count_points_vec, "Energy [GeV]", "Clusters", "Clusters per particle", cluster_count_filename);

	std::string fraction_of_energy_filename = folder + "fraction_of_energy.png";
	save_point_plot(fraction_of_energy_points_vec, "Energy [GeV]", "Fraction of total ADC", "Fraction of total ADC", fraction_of_energy_filename);

	std::string leftover_max_adc_filename = folder + "leftover_max_adc.png";
	save_point_plot(leftover_max_adc_points_vec, "Energy [GeV]", "Leftover max ADC", "Leftover max ADC", leftover_max_adc_filename);

	return 0;
}




bool MA_analysis(std::string folder, double MA_seed_threshold, double MA_aggregation_threshold,
	std::vector<double> &energy, 
	std::vector<double> &cluster_count,
	std::vector<double> &fraction_of_energy,
	std::vector<double> &leftover_max_adc,
	std::vector<double> &cluster_count_err,
	std::vector<double> &fraction_of_energy_err,
	std::vector<double> &leftover_max_adc_err
){


	std::vector<std::string> files = get_files(folder);
	//files.push_back("250_1_analysis.root");


	Grid g;
	if (!focal_h_grid(g)) return false;
	if (!focal_h_grid_add_neighbors_all(g)) return false;
	calc_neighbors_all(g);


	for (int i = 0; i < files.size(); i++){
		std::unique_ptr<TFile> f = std::make_unique<TFile>(files.at(i).c_str(), "READ");
		auto t = static_cast<TTree*>(f->Get("T"));
		int energy_temp = static_cast<TParameter<int>*>(f->Get("energy"))->GetVal();
		energy.push_back(double(energy_temp));

		std::vector<double> cluster_count_temp;
		std::vector<double> fraction_of_energy_temp;
		std::vector<double> leftover_max_adc_temp;

		for (int e = 0; e < t->GetEntries(); e++){

			fill_grid_ttree_entry(*t, g, e, true);

			ModifiedAggregation ma(&g, MA_seed_threshold, MA_aggregation_threshold);
			ma.tag();
			std::vector<std::string> main_cluster_vec;
			std::string main_cluster = ma.get_largest_cluster();
			main_cluster_vec.push_back(main_cluster);
			std::vector<std::string> remaining_clusters = ma.get_remaining_clusters(main_cluster_vec);
			if (remaining_clusters.size() > 0){
				std::string max = remaining_clusters.at(0);
				for (auto &v : remaining_clusters){
					if (ma.get_cluster_max_adc(v) > ma.get_cluster_max_adc(max))
						max = v;
				}
				leftover_max_adc_temp.push_back(ma.get_cluster_max_adc(max));
			}else{
				leftover_max_adc_temp.push_back(0);
			}

			cluster_count_temp.push_back(ma.get_cluster_count());
			fraction_of_energy_temp.push_back(ma.get_cluster_sum(main_cluster)/ma.get_sum());

			//Plotting heatmap
			//std::string fig_name = files.at(i) + std::to_string(i) + "_" + std::to_string(e) + ".png";
			std::string fig_name = folder + "heatmap/" + std::to_string(energy_temp) + "_" + std::to_string(e) + ".png";
			std::string title = std::to_string(energy_temp);
			//save_heatmap(g, fig_name, title);

		}
		cluster_count.push_back(mean(cluster_count_temp));
		cluster_count_err.push_back(standard_error(cluster_count_temp));

		fraction_of_energy.push_back(mean(fraction_of_energy_temp));
		fraction_of_energy_err.push_back(standard_error(fraction_of_energy_temp));

		leftover_max_adc.push_back(mean(leftover_max_adc_temp));
		leftover_max_adc_err.push_back(standard_error(leftover_max_adc_temp));
	}

	return true;
}


// Create grid according to FocalH geometry. Move later to a file format.

bool focal_h_grid(Grid& g){

	// The sides of the modules are 1.5mm copper plates.
	// The modules have a 3mm margin.
	// Modules are 65x65mm^2 with plate

	const double detector_width = 19.5; // cm
	const double detector_height = 19.5; // cm

	const double module_width = detector_width/3;
	const double module_height = detector_height/3;

	const unsigned int center_module_rows = 7;
	const unsigned int center_module_cols = 7;

	const unsigned int outer_module_rows = 5;
	const unsigned int outer_module_cols = 5;


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
			g.make_cell(x,y,0,dx,dy,center_cell_temp_name);
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
				g.make_cell(x,y,0,dx,dy,outer_cell_temp_name);
			}
	}


	return true;
}

bool fill_grid_ttree_entry(TTree& t, Grid& g, int e, bool override_values){

	std::vector<Double_t>* x_pos = nullptr;
	std::vector<Double_t>* y_pos = nullptr;
	std::vector<Double_t>* value = nullptr;
	std::vector<Double_t>* particle = nullptr;


	t.SetBranchAddress("x_pos", &x_pos);
	t.SetBranchAddress("y_pos", &y_pos);
	t.SetBranchAddress("value", &value);
	t.SetBranchAddress("particle", &particle);

	t.GetEntry(e);

	for (int i = 0; i < x_pos->size(); i++){
		if (override_values)
			g.SetCellValues(x_pos->at(i), y_pos->at(i), value->at(i));
		else
			g.Fill(x_pos->at(i), y_pos->at(i), value->at(i));
	}

	return true;
}

bool focal_h_grid_add_neighbors_all(Grid& g){
	//if (!focal_h_grid_add_neighbors_cell(g, "B11C00", {"B11C01", "B11C10", "B11C11","B21C40","B20C44", "B10C04"})) return false;
	//if (!focal_h_grid_add_neighbors_cell(g, "B11C01", {"B11C11", "B11C12", "B11C02","B21C41","B21C40", "B11C00", "B11C10"})) return false;




	return true;
}



bool focal_h_grid_add_neighbors_cell(Grid& g, std::string cell, std::initializer_list<std::string> neighbors){

	auto c = g.get_cell(cell); if (!c) return false;
	for (auto n : neighbors){
		if(!c->add_neighbor(&*g.get_cell(n))) return false;
	}

	return true;
}



bool calc_neighbors_all(Grid &g){

	for (auto &v : *g.get_cells())
		if (!calc_neighbors(g, &*v)) return false;
	return true;
}

bool calc_neighbors(Grid &g, Cell *c){

	const double tol = 0.000001;

	std::pair<double, double> E = {c->get_x_position() + c->get_width()/2, c->get_y_position()};
	std::pair<double, double> NE = {c->get_x_position() + c->get_width()/2, c->get_y_position() + c->get_height()/2};
	std::pair<double, double> N = {c->get_x_position(), c->get_y_position() + c->get_height()/2};
	std::pair<double, double> NW = {c->get_x_position() - c->get_width()/2, c->get_y_position() + c->get_height()/2};
	std::pair<double, double> W = {c->get_x_position() - c->get_width()/2, c->get_y_position()};
	std::pair<double, double> SW = {c->get_x_position() - c->get_width()/2, c->get_y_position() - c->get_height()/2};
	std::pair<double, double> S = {c->get_x_position(), c->get_y_position() - c->get_height()/2};
	std::pair<double, double> SE = {c->get_x_position() + c->get_width()/2, c->get_y_position() - c->get_height()/2};


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
			c->add_neighbor(v.get());

	return true;
}



double mean(std::vector<double> &vec){
	double sum = 0;
	for (auto &v : vec)
		sum += v;
	return sum/vec.size();
}

void save_heatmap(Grid &g, std::string filename, std::string title){
	std::unique_ptr<TH2Poly> hist = g.plot_grid();
	hist->SetMaximum(SATURATION_VALUE);
	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 600, 600);
	c->cd();
	gStyle->SetOptStat(0);
	hist->SetTitle(title.c_str());
	hist->Draw();
	c->Update();
	c->SaveAs(filename.c_str());
}


void save_point_plot(std::vector<std::unique_ptr<TGraphErrors>> &point_plots, std::string x_label, std::string y_label, std::string legend_title, std::string filename){

	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 800, 800);
	std::unique_ptr<TMultiGraph> multi = std::make_unique<TMultiGraph>();
	c->cd();

	for (auto &v : point_plots){
		multi->Add(v.get());
		v.release(); // Have to relinquish ownership to the root objects
	}
	std::string title = ";" + x_label + ";" + y_label;
	multi->SetTitle(title.c_str());
	multi->Draw("AP");
	
	
	TLegend *leg = c->BuildLegend();
	//leg->SetBorderSize(0);
	leg->SetHeader(legend_title.c_str(), "C");

	c->SetTopMargin(0.2);
	c->SetRightMargin(0.1);
	c->SetLeftMargin(0.15);
	c->Update();
	c->SaveAs(filename.c_str());
}

std::unique_ptr<TGraphErrors> plot_points(std::vector<double> &x, std::vector<double> &y, std::vector<double> &y_err){

	std::unique_ptr<TGraphErrors> graph = std::make_unique<TGraphErrors>();
	for (int i = 0; i < x.size(); i++){
		graph->SetPoint(i, x.at(i), y.at(i));
		graph->SetPointError(i, 0, y_err.at(i));
	}
	graph->SetName("graph");
	graph->SetMarkerStyle(21);
	graph->SetMarkerColor(kBlue);
	//graph->GetXaxis()->SetTitle(x_label.c_str());
	//graph->GetYaxis()->SetTitle(y_label.c_str());

	return std::move(graph);
}


double standard_error(std::vector<Double_t> &samples){
	double mean = 0.0;
	double std_dev = 0.0;
	double standard_error = 0.0;

	for (auto &v : samples) mean += v;
	mean /= samples.size();

	for (auto &v : samples) std_dev += std::pow((v - mean), 2);
	//std_dev /= samples->size() - 1;
	std_dev /= samples.size(); // Surely I want sample deviation and not population deviation??
	std_dev = std::sqrt(std_dev);
	
	standard_error = std_dev/std::sqrt(samples.size());
	return standard_error;
}

std::vector<std::string> get_files(std::string folder){
	std::vector<std::string> files;
    std::regex reg("analysis.root$",
            std::regex_constants::ECMAScript | std::regex_constants::icase);

    for (const auto & entry : std::filesystem::directory_iterator(folder))
		if (std::regex_search(entry.path().u8string(), reg)){
			files.push_back(entry.path().u8string());
		}
	return files;
}
