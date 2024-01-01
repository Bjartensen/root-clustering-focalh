//#include "clustering.h"
#include "modified_aggregation.h"
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
#include <TF1.h>

const double SATURATION_VALUE = 4095;
//const double SATURATION_VALUE = 12000;
const std::string IMAGE_EXTENSION = ".png";
//const std::string IMAGE_EXTENSION = ".pdf";
std::string ANALYSIS_TAG = "";

bool focal_h_grid(Grid& g);
bool fill_grid_ttree_entry(TTree& t, Grid& g, int e, bool override_values = false);
bool focal_h_grid_add_neighbors_cell(Grid& g, std::string cell, std::initializer_list<std::string> neighbors);
bool focal_h_grid_add_neighbors_all(Grid &g);
bool calc_neighbors(Grid &g, Cell *c);
bool calc_neighbors_all(Grid &g);
std::unique_ptr<TGraphErrors> plot_points(std::vector<std::pair<int, std::vector<double>>> data);
std::unique_ptr<TH1D> plot_1dhist(std::vector<double> &y, unsigned int bins = -1);
bool MA_clustering(std::string folder, double MA_seed_threshold, double MA_aggregation_threshold,
	std::vector<std::pair<int, std::vector<double>>> &cluster_count,
	std::vector<std::pair<int, std::vector<double>>> &fraction_of_energy,
	std::vector<std::pair<int, std::vector<double>>> &leftover_max_adc,
	std::vector<std::pair<int, std::vector<std::vector<std::pair<double, double>>>>> &cluster_center_of_mass
);
double mean(std::vector<double> &vec);
double standard_error(std::vector<double> &samples);
void save_point_plot(std::vector<std::unique_ptr<TGraphErrors>> &point_plots, std::string x_label, std::string y_label, std::string legend_title, std::string filename);
void save_1dhist(std::unique_ptr<TH1D> &hist, std::string x_label, std::string y_label, std::string legend_title, std::string filename);
void save_heatmap(Grid &g, std::string filename, std::string title);
std::vector<std::string> get_files(std::string folder);
void analysis();
bool MA_reconstructed_energies(std::string folder, double seed_threshold, double aggregation_threshold);

int main(int argc, char* argv[]){




	//Grid g;
	//if (!focal_h_grid(g)) return false;
	//if (!focal_h_grid_add_neighbors_all(g)) return false;
	//calc_neighbors_all(g);


	//std::string folder = "../data/testbeam/";
	////std::string file = "../data/testbeam/Run_3132_monocluster.root";
	//std::string file = "../data/testbeam/201_100_tb.root";
	//std::string filename_fig = "../data/testbeam/test4.png";
	//std::string filename_fig = "../data/testbeam/test2.png";


	//std::unique_ptr<TFile> f = std::make_unique<TFile>(file.c_str(), "READ");
	//auto t = static_cast<TTree*>(f->Get("T"));


	//fill_grid_ttree_entry(*t, g, 0, true);
	//save_heatmap(g, filename_fig, "adsf");




	std::string folder = "../data/focalsim/pi_plus_1000e_deg0/";
	//std::string folder = "../data/focalsim/pi_plus_100e_deg0/";
	MA_reconstructed_energies(folder, 800.0, 10);
	
	//analysis();




	return 0;
}


bool MA_reconstructed_energies(std::string folder, double seed_threshold, double aggregation_threshold){

	std::vector<std::string> files = get_files(folder);

	Grid g;
	if (!focal_h_grid(g)) return false;
	if (!focal_h_grid_add_neighbors_all(g)) return false;
	calc_neighbors_all(g);

	std::vector<std::pair<int, std::vector<double>>> cluster_adc_sums_energy;
	std::vector<std::pair<int, std::vector<double>>> grid_adc_sums_energy;

	for (int i = 0; i < files.size(); i++){
		std::unique_ptr<TFile> f = std::make_unique<TFile>(files.at(i).c_str(), "READ");
		auto t = static_cast<TTree*>(f->Get("T"));
		int energy_temp = static_cast<TParameter<int>*>(f->Get("energy"))->GetVal();

		std::vector<double> cluster_adc_sums;
		std::vector<double> grid_adc_sums;
		// For each event
		for (int e = 0; e < t->GetEntries(); e++){

			fill_grid_ttree_entry(*t, g, e, true);

			ModifiedAggregation ma(&g, seed_threshold, aggregation_threshold);
			
			// Any clusters found
			if (ma.tag()){
				std::string main_cluster = ma.get_largest_cluster();
				cluster_adc_sums.push_back(ma.get_cluster_sum(main_cluster));

			// No clusters found
			}else{
				cluster_adc_sums.push_back(0.0);
			}
			grid_adc_sums.push_back(g.get_grid_sum());
		}

		cluster_adc_sums_energy.push_back(std::make_pair(energy_temp, cluster_adc_sums));
		grid_adc_sums_energy.push_back(std::make_pair(energy_temp, grid_adc_sums));
	}



	std::vector<std::tuple<int, double, double>> grid_fits;
	std::vector<std::tuple<int, double, double>> cluster_fits;


	// Grid fits and plots
	std::unique_ptr<TCanvas> grid_canvas = std::make_unique<TCanvas>("c", "c", 1);
	grid_canvas->cd();
	for (int i = 0; i < files.size(); i++){
		std::cout << std::to_string(grid_adc_sums_energy.at(i).first) << std::endl;
		//std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 100, 0, 150000);
		std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 50, 0, 150000);
		std::unique_ptr<TF1> fit = std::make_unique<TF1>("fit", "gaus");
		for (auto &v : grid_adc_sums_energy.at(i).second)
			hist->Fill(v);
		fit->SetParameter(0, hist->GetMaximum());
		fit->SetParameter(1, hist->GetMean());
		fit->SetParameter(2, hist->GetRMS());
		std::cout << "Max: " << hist->GetMaximum() << " at: " << hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin()) << std::endl;
		hist->SetTitle(std::to_string(grid_adc_sums_energy.at(i).first).c_str());
		hist->SetFillColor(0);
		hist->SetLineColor(i+1);
		hist->SetStats(0);
		hist->Draw("same");
		hist->Fit("fit", "Q0");
		grid_fits.push_back(std::make_tuple(grid_adc_sums_energy.at(i).first, fit->GetParameter(1), fit->GetParError(1)));
		hist.release();
	}

	std::string grid_hist_filename = "grid_hist.png";
	//std::string title = ";" + x_label + ";" + y_label;

	
	TLegend *leg = grid_canvas->BuildLegend();
	//leg->SetHeader(legend_title.c_str(), "C");

	//c->SetTopMargin(0.2);
	grid_canvas->SetRightMargin(0.1);
	grid_canvas->SetLeftMargin(0.15);
	grid_canvas->Update();
	grid_canvas->SaveAs(grid_hist_filename.c_str());


	// Cluster fits and plots
	for (int i = 0; i < files.size(); i++){
		std::cout << std::to_string(cluster_adc_sums_energy.at(i).first) << std::endl;
		//std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 100, 0, 150000);
		std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 50, 0, 150000);
		std::unique_ptr<TF1> fit = std::make_unique<TF1>("fit", "gaus");
		for (auto &v : cluster_adc_sums_energy.at(i).second)
			hist->Fill(v);
		fit->SetParameter(0, hist->GetMaximum());
		fit->SetParameter(1, hist->GetMean());
		fit->SetParameter(2, hist->GetRMS());
		std::cout << "Max: " << hist->GetMaximum() << " at: " << hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin()) << std::endl;
		hist->SetTitle(std::to_string(cluster_adc_sums_energy.at(i).first).c_str());
		hist->SetFillColor(0);
		hist->SetLineColor(i+1);
		hist->SetStats(0);
		hist->Draw("same");
		hist->Fit("fit", "Q0");
		cluster_fits.push_back(std::make_tuple(cluster_adc_sums_energy.at(i).first, fit->GetParameter(1), fit->GetParError(1)));
		hist.release();
	}




	std::vector<std::unique_ptr<TGraphErrors>> lin_plots;
	std::unique_ptr<TGraphErrors> grid_points = std::make_unique<TGraphErrors>();
	std::unique_ptr<TGraphErrors> cluster_points = std::make_unique<TGraphErrors>();
	grid_points->SetName("Grid points");
	grid_points->SetMarkerStyle(21);
	grid_points->SetMarkerColor(kBlue);
	cluster_points->SetName("Cluster points");
	cluster_points->SetMarkerStyle(22);
	cluster_points->SetMarkerColor(kRed);

	for (int i = 0; i < grid_fits.size(); i++){
		grid_points->SetPoint(i, std::get<0>(grid_fits.at(i)), std::get<1>(grid_fits.at(i)));
		grid_points->SetPointError(i, 0, std::get<2>(grid_fits.at(i)));

		cluster_points->SetPoint(i, std::get<0>(cluster_fits.at(i)), std::get<1>(cluster_fits.at(i)));
		cluster_points->SetPointError(i, 0, std::get<2>(cluster_fits.at(i)));

		//grid_points->SetMinimum(0);
		//cluster_points->SetMinimum(0);
	}

	std::unique_ptr<TF1> grid_lin_fit = std::make_unique<TF1>("grid_lin_fit", "pol1", 0, 400);
	std::unique_ptr<TF1> cluster_lin_fit = std::make_unique<TF1>("cluster_lin_fit", "pol1", 0, 400);
	grid_lin_fit->SetLineColor(kBlue);
	grid_lin_fit->SetLineStyle(2);
	cluster_lin_fit->SetLineColor(kRed);
	cluster_lin_fit->SetLineStyle(2);
	grid_points->Fit("grid_lin_fit", "R");
	cluster_points->Fit("cluster_lin_fit", "R");
	lin_plots.push_back(std::move(cluster_points));
	lin_plots.push_back(std::move(grid_points));
	std::string lin_plot_x_label = "Energy [GeV]";
	std::string lin_plot_y_label = "ADC Counts";
	std::string lin_plot_legend_title = "Linearization";
	std::string lin_plot_filename = "lin_plot" + IMAGE_EXTENSION;
	save_point_plot(lin_plots, lin_plot_x_label, lin_plot_y_label, lin_plot_legend_title, lin_plot_filename);



	for (auto &v : grid_fits){
		std::cout << std::get<0>(v) << ",";
	}
	std::cout << std::endl;

	for (auto &v : cluster_fits){
		std::cout << std::get<0>(v) << ",";
	}
	std::cout << std::endl;




	return true;
}





void analysis(){
	std::vector<std::pair<double, double>> MA_params;
	ANALYSIS_TAG = "_MISC";
	//MA_params.push_back(std::make_pair(100, 10));
	//MA_params.push_back(std::make_pair(300, 10));
	//MA_params.push_back(std::make_pair(500, 10));
	MA_params.push_back(std::make_pair(800, 10));
	//MA_params.push_back(std::make_pair(1000, 10));

	//ANALYSIS_TAG = "_DIFFAGG";
	//MA_params.push_back(std::make_pair(800, 10));
	//MA_params.push_back(std::make_pair(800, 100));
	//MA_params.push_back(std::make_pair(800, 300));
	//MA_params.push_back(std::make_pair(800, 400));
	//MA_params.push_back(std::make_pair(800, 500));

	std::string folder = "../data/focalsim/pi_plus_1000e_deg0/";
	//std::string folder = "../data/focalsim/pi_plus_100e_deg0/";
	//std::string folder = "../data/focalsim/pi_plus_deg0/";

	std::vector<std::unique_ptr<TGraphErrors>> cluster_count_points_vec;
	std::vector<std::unique_ptr<TGraphErrors>> fraction_of_energy_points_vec;
	std::vector<std::unique_ptr<TGraphErrors>> leftover_max_adc_points_vec;
	std::vector<std::unique_ptr<TH1D>> cluster_count_hist_vec;
	std::vector<std::unique_ptr<TH1D>> cluster_center_of_mass_hist_vec;

	for (int i = 0; i < MA_params.size(); i++){
		std::vector<std::pair<int, std::vector<double>>> cluster_count;
		std::vector<std::pair<int, std::vector<double>>> fraction_of_energy;
		std::vector<std::pair<int, std::vector<double>>> leftover_max_adc;
		std::vector<std::pair<int, std::vector<std::vector<std::pair<double, double>>>>> cluster_center_of_mass;

		double MA_seed_threshold = MA_params.at(i).first;
		double MA_aggregation_threshold = MA_params.at(i).second;
		std::string MA_params_desc = "Seed thld: " + std::to_string(int(MA_seed_threshold)) + ", agg thld: " + std::to_string(int(MA_aggregation_threshold));
		std::string MA_params_short = "MA_"+std::to_string(int(MA_seed_threshold))+"_"+std::to_string(int(MA_aggregation_threshold));

		MA_clustering(folder, MA_seed_threshold, MA_aggregation_threshold,
					cluster_count,
					fraction_of_energy,
					leftover_max_adc,
					cluster_center_of_mass
				);

		// Cluster count
		std::unique_ptr<TGraphErrors> cluster_count_points = plot_points(cluster_count);
		cluster_count_points->SetMarkerStyle(21+i);
		cluster_count_points->SetMarkerColor(1+i);
		cluster_count_points->SetName(MA_params_desc.c_str());
		cluster_count_points_vec.push_back(std::move(cluster_count_points));

		// Fraction of total
		std::unique_ptr<TGraphErrors> fraction_of_energy_points = plot_points(fraction_of_energy);
		fraction_of_energy_points->SetMarkerStyle(21+i);
		fraction_of_energy_points->SetMarkerColor(1+i);
		fraction_of_energy_points->SetName(MA_params_desc.c_str());
		fraction_of_energy_points_vec.push_back(std::move(fraction_of_energy_points));

		// Leftover max ADC
		std::unique_ptr<TGraphErrors> leftover_max_adc_points = plot_points(leftover_max_adc);
		leftover_max_adc_points->SetMarkerStyle(21+i);
		leftover_max_adc_points->SetMarkerColor(1+i);
		leftover_max_adc_points->SetName(MA_params_desc.c_str());
		leftover_max_adc_points_vec.push_back(std::move(leftover_max_adc_points));

		// Number of clusters hist
		for (int i = 0 ; i < cluster_count.size(); i++){
			std::unique_ptr<TH1D> cluster_count_hist = plot_1dhist(cluster_count.at(i).second);
			cluster_count_hist->SetDefaultBufferSize();
			cluster_count_hist->SetFillColor(kBlue-5);
			std::string temp_filename = std::to_string(int(cluster_count.at(i).first));
			cluster_count_hist->SetTitle(temp_filename.c_str());
			cluster_count_hist_vec.push_back(std::move(cluster_count_hist));
		}



		std::vector<double> two_cluster_center_of_mass_distance;
		for (const auto &f : cluster_center_of_mass){ // file/energy
			for (const auto &e : f.second){ // event
				if (e.size() == 2){
					double x_dist = e.at(0).first - e.at(1).first;
					double y_dist = e.at(0).second - e.at(1).second;
					two_cluster_center_of_mass_distance.push_back(std::sqrt(std::pow(x_dist,2) + std::pow(y_dist,2)));
				}
			}
		}

		std::unique_ptr<TH1D> cluster_center_of_mass_hist = plot_1dhist(two_cluster_center_of_mass_distance, 21);
		cluster_center_of_mass_hist->SetDefaultBufferSize();
		cluster_center_of_mass_hist->SetFillColor(kBlue-5);
		std::string temp_filename = "com_test";//std::to_string(int(cluster_count.at(i).first));
		std::string temp_title = "Center of mass when 2 clusters;Distance [cm];Events";
		cluster_center_of_mass_hist->SetTitle(temp_title.c_str());
		cluster_center_of_mass_hist_vec.push_back(std::move(cluster_center_of_mass_hist));





	}

	std::string cluster_count_filename = folder + "cluster_count" + ANALYSIS_TAG + IMAGE_EXTENSION;
	save_point_plot(cluster_count_points_vec, "Energy [GeV]", "Clusters", "Clusters per particle", cluster_count_filename);

	std::string fraction_of_energy_filename = folder + "fraction_of_energy" + ANALYSIS_TAG + IMAGE_EXTENSION;
	save_point_plot(fraction_of_energy_points_vec, "Energy [GeV]", "Fraction of total ADC", "Fraction of total ADC", fraction_of_energy_filename);

	std::string leftover_max_adc_filename = folder + "leftover_max_adc" + ANALYSIS_TAG + IMAGE_EXTENSION;
	save_point_plot(leftover_max_adc_points_vec, "Energy [GeV]", "Leftover max ADC", "Leftover max ADC", leftover_max_adc_filename);

	for (int i = 0; i < cluster_count_hist_vec.size(); i++){
		std::string temp_extension = cluster_count_hist_vec.at(i)->GetTitle();
		std::string cluster_count_hist_filename = folder + "number_of_clusters_" + temp_extension + ANALYSIS_TAG + IMAGE_EXTENSION;
		save_1dhist(cluster_count_hist_vec.at(i), "Count", "Clusters", "Clusters", cluster_count_hist_filename);
	}

	for (int i = 0; i < cluster_center_of_mass_hist_vec.size(); i++){
		std::string temp_extension = cluster_center_of_mass_hist_vec.at(i)->GetTitle();
		std::string cluster_center_of_mass_hist_filename = folder + "CoM_" + temp_extension + ANALYSIS_TAG + IMAGE_EXTENSION;
		save_1dhist(cluster_center_of_mass_hist_vec.at(i), "Distance", "Events", "Center of mass", cluster_center_of_mass_hist_filename);
	}


}


bool MA_clustering(std::string folder, double MA_seed_threshold, double MA_aggregation_threshold,
	std::vector<std::pair<int, std::vector<double>>> &cluster_count,
	std::vector<std::pair<int, std::vector<double>>> &fraction_of_energy,
	std::vector<std::pair<int, std::vector<double>>> &leftover_max_adc,
	std::vector<std::pair<int, std::vector<std::vector<std::pair<double, double>>>>> &cluster_center_of_mass
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

		std::vector<double> cluster_count_temp;
		std::vector<double> fraction_of_energy_temp;
		std::vector<double> leftover_max_adc_temp;
		std::vector<std::vector<std::pair<double, double>>> cluster_center_of_mass_temp;
		for (int e = 0; e < t->GetEntries(); e++){

			fill_grid_ttree_entry(*t, g, e, true);

			ModifiedAggregation ma(&g, MA_seed_threshold, MA_aggregation_threshold);
			
			// Any clusters found
			if (ma.tag()){
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

				fraction_of_energy_temp.push_back(ma.get_cluster_sum(main_cluster)/ma.get_sum());
				cluster_count_temp.push_back(ma.get_cluster_count());
				//number_of_clusters.push_back(ma.get_cluster_count());
				ma.cluster_center_of_mass(main_cluster);
				std::vector<std::pair<double, double>> event_cluster_center_of_mass_temp;
				for (const auto &t : ma.get_clusters())
					event_cluster_center_of_mass_temp.push_back(ma.cluster_center_of_mass(t));
				cluster_center_of_mass_temp.push_back(event_cluster_center_of_mass_temp);

			// No clusters found
			}else{
				cluster_count_temp.push_back(0);
				//number_of_clusters.push_back(0);
				fraction_of_energy_temp.push_back(0);
				leftover_max_adc_temp.push_back(0);
			}
			

			//Plotting heatmap
			//std::string fig_name = files.at(i) + std::to_string(i) + "_" + std::to_string(e) + ".png";
			std::string fig_name = folder + "heatmap/" + std::to_string(energy_temp) + "_" + std::to_string(e) + ".png";
			std::string title = std::to_string(energy_temp);
			//save_heatmap(g, fig_name, title);

		}
		cluster_count.push_back(std::make_pair(energy_temp, cluster_count_temp));

		fraction_of_energy.push_back(std::make_pair(energy_temp, fraction_of_energy_temp));

		leftover_max_adc.push_back(std::make_pair(energy_temp, leftover_max_adc_temp));

		cluster_center_of_mass.push_back(std::make_pair(energy_temp, cluster_center_of_mass_temp));
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
	//std::vector<Double_t>* particle = nullptr;


	t.SetBranchAddress("x_pos", &x_pos);
	t.SetBranchAddress("y_pos", &y_pos);
	t.SetBranchAddress("value", &value);
	//t.SetBranchAddress("particle", &particle);

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
	if (SATURATION_VALUE > 0)
		hist->SetMaximum(SATURATION_VALUE);
	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 2400, 2400);
	c->cd();
	gStyle->SetOptStat(0);
	hist->SetTitle(title.c_str());
	hist->Draw();
	c->Update();
	c->SaveAs(filename.c_str());
}


void save_point_plot(std::vector<std::unique_ptr<TGraphErrors>> &point_plots, std::string x_label, std::string y_label, std::string legend_title, std::string filename){

	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("save_point_plot", "save_point_plot", 1);
	std::unique_ptr<TMultiGraph> multi = std::make_unique<TMultiGraph>();
	c->cd();

	for (auto &v : point_plots){
		multi->Add(v.get());
		v.release(); // Have to relinquish ownership to the root objects
	}
	std::string title = ";" + x_label + ";" + y_label;
	multi->SetTitle(title.c_str());
	multi->Draw("AP");
	multi->SetMinimum(0);
	multi->GetXaxis()->SetLimits(0, 400);
	
	
	TLegend *leg = c->BuildLegend();
	//leg->SetBorderSize(0);
	leg->SetHeader(legend_title.c_str(), "C");

	//c->SetTopMargin(0.2);
	c->SetRightMargin(0.1);
	c->SetLeftMargin(0.15);
	c->Update();
	c->SaveAs(filename.c_str());
}

std::unique_ptr<TGraphErrors> plot_points(std::vector<std::pair<int, std::vector<double>>> data){

	std::unique_ptr<TGraphErrors> graph = std::make_unique<TGraphErrors>();

	for (int i = 0; i < data.size(); i++){
		graph->SetPoint(i, data.at(i).first, mean(data.at(i).second));
		graph->SetPointError(i, 0, standard_error(data.at(i).second));
	}
	graph->SetName("graph");
	graph->SetMarkerStyle(21);
	graph->SetMarkerColor(kBlue);
	//graph->GetXaxis()->SetTitle(x_label.c_str());
	//graph->GetYaxis()->SetTitle(y_label.c_str());

	return std::move(graph);
}



std::unique_ptr<TH1D> plot_1dhist(std::vector<double> &y, unsigned int bins){
	std::set<double> unq;
	for (const auto &v : y)
		unq.insert(v);

	if (bins == -1)
		bins = unq.size();

	std::unique_ptr<TH1D> ptr = std::make_unique<TH1D>("hist", "Number of clusters", bins, 0, 0);
	for (const auto &v : y){
		ptr->Fill(v);
	}
	return std::move(ptr);
}


void save_1dhist(std::unique_ptr<TH1D> &hist, std::string x_label, std::string y_label, std::string legend_title, std::string filename){
	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 1);
	c->cd();
	std::string title = ";" + x_label + ";" + y_label;

	hist->Draw();
	hist.release();
	
	TLegend *leg = c->BuildLegend();
	leg->SetHeader(legend_title.c_str(), "C");

	//c->SetTopMargin(0.2);
	c->SetRightMargin(0.1);
	c->SetLeftMargin(0.15);
	c->Update();
	c->SaveAs(filename.c_str());
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
