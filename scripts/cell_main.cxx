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
#include <THStack.h>
#include "focalh.h"


#include "cluster_events.h"

const double SATURATION_VALUE = 4096;
//const double SATURATION_VALUE = 32000;
//const std::string IMAGE_EXTENSION = ".png";
const std::string IMAGE_EXTENSION = ".pdf";
std::string ANALYSIS_TAG = "";

// Move to class





std::unique_ptr<TGraphErrors> plot_points(std::vector<std::pair<int, std::vector<double>>> data);
std::unique_ptr<TH1D> plot_1dhist(std::vector<double> &y, unsigned int bins = -1);
bool MA_clustering(std::string folder, double MA_seed_threshold, double MA_aggregation_threshold,
	std::vector<std::pair<int, std::vector<double>>> &cluster_count,
	std::vector<std::pair<int, std::vector<double>>> &fraction_of_energy,
	std::vector<std::pair<int, std::vector<double>>> &leftover_max_adc,
	std::vector<std::pair<int, std::vector<std::vector<std::pair<double, double>>>>> &cluster_center_of_mass
);




// Move to some util header or find better STL ways of doing it
double mean(std::vector<double> &vec);
double standard_error(std::vector<double> &samples);


void test_cluster_writer(std::string file);


void save_point_plot(std::vector<std::unique_ptr<TGraphErrors>> &point_plots, std::string x_label, std::string y_label, std::string legend_title, std::string filename);
void save_1dhist(std::unique_ptr<TH1D> &hist, std::string x_label, std::string y_label, std::string legend_title, std::string filename);
void save_heatmap(Grid &g, std::string filename, std::string title);
void save_neighborhood(Grid &g, Cell* cell, std::string filename, std::string title);
std::vector<std::string> get_files(std::string folder);
void analysis(std::string folder);
bool MA_reconstructed_energies(std::string folder, double seed_threshold, double aggregation_threshold);

int main(int argc, char* argv[]){


	FoCalH focal;
	Grid &g = focal.get_grid();

	/*
	Grid g;
	if (!focal_h_grid(g)) return false;
	calc_neighbors_all(g);
	std::cout << g.get_cells()->size() << std::endl;
	std::cout << g.get_cells()->at(0)->get_width() << std::endl;
	*/

	//std::string folder = "../data/testbeam/";
	//std::string file = "../data/testbeam/Run_3226_monocluster.root";
	//std::string file = "../data/focalsim/pi_plus_10000e_deg0/250_10000_small.root";
	std::string file = "../data/focalsim/pi_plus_1000e_deg0/250_1000_analysis.root";
	//std::string file = "../data/focalsim/misc/250_1_analysis.root";
	//std::string file = "../data/tstbeam/201_100_tb.root";
	//std::string filename_fig = "../data/testbeam/OLD.png";
	//std::string filename_fig = "../data/testbeam/NEW.png";


	std::unique_ptr<TFile> f = std::make_unique<TFile>(file.c_str(), "READ");
	auto t = static_cast<TTree*>(f->Get("T"));


	for (int i = 0; i < 10; i++){
		g.fill_grid_ttree_entry(*t, i, true);
		std::string temp_filename_fig = "../data/focalsim/pi_plus_1000e_deg0/" + std::to_string(i) + IMAGE_EXTENSION;
		save_heatmap(g, temp_filename_fig, "250 GeV, pi+");
	}


	//for (const auto &v : *g.get_cells()){
	//	std::string temp_filename_fig = "../data/testbeam/neighboords" + v->get_id() + IMAGE_EXTENSION;
	//	save_neighborhood(g, v.get(), temp_filename_fig, "");
	//	std::cout << v->get_id() << ",";
	//}
	//std::cout << std::endl;



	//std::string folder = "../data/focalsim/pi_plus_10000e_deg0/";
	//std::string folder = "../data/focalsim/pi_plus_1000e_deg0/";
	std::string folder = "../data/focalsim/pi_plus_1000e_deg0/";
	//std::string folder = "../data/testbeam/";



	//MA_reconstructed_energies(folder, 800.0, 10);
	//analysis(folder);
	test_cluster_writer(file);




	return 0;
}


void test_cluster_writer(std::string file){
	FoCalH focal;
	Grid &g = focal.get_grid();

	std::string folder;
	std::string filename = "test";

	std::unique_ptr<TFile> f = std::make_unique<TFile>(file.c_str(), "READ");
	std::cout << "Opening file: " << file << std::endl;
	auto t = static_cast<TTree*>(f->Get("T"));
	int energy_temp = static_cast<TParameter<int>*>(f->Get("energy"))->GetVal();

	std::vector<double> cluster_adc_sums;
	std::vector<double> second_cluster_adc_sums;
	std::vector<double> grid_adc_sums;
	std::vector<double> leftover_grid_adc_sums;

	ClusterWriter writer(filename, std::ios_base::out);
	writer.open();

	// For each event
	for (int e = 38; e < 40; e++){

		std::cout << "Clustering event " << e << std::endl;

		g.fill_grid_ttree_entry(*t, e, true);

		double seed_threshold = 0;
		double aggregation_threshold = 0;
		//ModifiedAggregation ma(g, seed_threshold, aggregation_threshold);
		ModifiedAggregation ma(seed_threshold, aggregation_threshold);
		ma.set_grid(g);
		
		// Any clusters found
		if (ma.tag()){
			std::string main_cluster = ma.get_largest_cluster();
			cluster_adc_sums.push_back(ma.get_cluster_sum(main_cluster));
			leftover_grid_adc_sums.push_back(g.get_grid_sum() - ma.get_cluster_sum(main_cluster));


		// No clusters found
		}else{
			cluster_adc_sums.push_back(0.0);
			leftover_grid_adc_sums.push_back(g.get_grid_sum());
		}
		grid_adc_sums.push_back(g.get_grid_sum());

		// 3 + 3*7 = 0,0
		int idx = 3 + 3*7;
		//int idx = 3 + 3*7*5;
		Cell &gc = *ma.get_grid()->get_cells()->at(idx).get();
		auto ccm = ma.get_tagged_cells()->at(&gc);
		std::cout << gc.get_x() << "," << gc.get_y() << "," << gc.get_value() << ",tag:" << ccm << std::endl;



		if (!writer.write_event(*ma.get_tagged_cells(), e))
			std::cout << "Couldn't write" << std::endl;


	}

	writer.close();



	// Try reading in the file now

	ClusterWriter reader(filename, std::ios_base::in);
	reader.open();

	ClusterWriter::Event ev;
	long evnum;

	if (reader.read_event(ev, evnum)){
		std::cout << "Read event! Event number: " << evnum << std::endl;
		for (int i = 0; i < ev.x_vec.size(); i++){
			std::cout << ev.x_vec.at(i) << "," << ev.y_vec.at(i) << "," << ev.val_vec.at(i) << ","  << ev.cluster_id_vec.at(i) << std::endl;

		}
	}

	reader.close();


}


bool MA_reconstructed_energies(std::string folder, double seed_threshold, double aggregation_threshold){

	std::cout << "Plotting histograms, fitting gaussians and linearity..." << std::endl;

	std::vector<std::string> files = get_files(folder);


	bool sim = false;
	std::string lin_plot_legend_title;
	std::string leftover_lin_plot_legend_title;
	std::string mc_tb_affix;
	std::string lin_plot_filename;
	std::string leftover_lin_plot_filename;
	int grid_hist_max;
	int cluster_hist_max;
	int leftover_grid_hist_max;
	int grid_bins;
	int cluster_bins;
	int leftover_grid_bins;
	int grid_xmax;
	int cluster_xmax;
	int leftover_grid_xmax;
	if (!sim){ // tb, 10000 events
		lin_plot_legend_title = "Testbeam SEP2023 linearity, hadrons"; // tb
		leftover_lin_plot_legend_title = "Testbeam SEP2023 linearity, hadrons"; // tb
		mc_tb_affix = "tb"; //tb
		lin_plot_filename = folder + mc_tb_affix + "_lin_plot_" + IMAGE_EXTENSION;
		leftover_lin_plot_filename = folder + mc_tb_affix + "_leftover_lin_plot_" + IMAGE_EXTENSION;
		grid_hist_max = 550; // tb
		cluster_hist_max = 400; // tb
		leftover_grid_hist_max = 500;
		grid_bins = 300; // tb
		cluster_bins = 300; // tb
		leftover_grid_bins = 300;
		grid_xmax = 290000; // tb
		cluster_xmax = 200000; // tb
		leftover_grid_xmax = 120000;

	//}else{ // sim, 1000 events
	//	lin_plot_legend_title = "Monte Carlo linearity, pions"; // tb
	//	mc_tb_affix = "mc"; //tb
	//	lin_plot_filename = "lin_plot_" + mc_tb_affix + IMAGE_EXTENSION;
	//	grid_hist_max = 100; // tb
	//	cluster_hist_max = 70; // tb
	//	grid_bins = 250; // tb
	//	cluster_bins = 250; // tb
	//	grid_xmax = 150000; // tb
	//	cluster_xmax = 130000; // tb
	//}

	}else{ // sim, 1000 events
		lin_plot_legend_title = "Simulation linearity, pions"; // tb
		leftover_lin_plot_legend_title = "Simulation linearity, pions"; // tb
		mc_tb_affix = "mc"; //tb
		lin_plot_filename = folder + mc_tb_affix + "_lin_plot_"  + IMAGE_EXTENSION;
		leftover_lin_plot_filename = folder + mc_tb_affix + "_leftover_lin_plot_"  + IMAGE_EXTENSION;
		grid_hist_max = 820; // tb
		cluster_hist_max = 500; // tb
		leftover_grid_hist_max = 820;
		grid_bins = 250; // tb
		cluster_bins = 200; // tb
		leftover_grid_bins = 250;
		grid_xmax = 150000; // tb
		cluster_xmax = 130000; // tb
		leftover_grid_xmax = 60000;
	}


	FoCalH focal;
	Grid &g = focal.get_grid();

	std::vector<std::pair<int, std::vector<double>>> cluster_adc_sums_energy;
	std::vector<std::pair<int, std::vector<double>>> second_cluster_adc_sums_energy;

	// Add another vector for second largest cluster (if exists) to test for linearity
	std::vector<std::pair<int, std::vector<double>>> grid_adc_sums_energy;
	std::vector<std::pair<int, std::vector<double>>> leftover_grid_adc_sums_energy;

	for (int i = 0; i < files.size(); i++){
		std::unique_ptr<TFile> f = std::make_unique<TFile>(files.at(i).c_str(), "READ");
		std::cout << "Opening file: " << files.at(i) << std::endl;
		auto t = static_cast<TTree*>(f->Get("T"));
		int energy_temp = static_cast<TParameter<int>*>(f->Get("energy"))->GetVal();

		std::vector<double> cluster_adc_sums;
		std::vector<double> second_cluster_adc_sums;
		std::vector<double> grid_adc_sums;
		std::vector<double> leftover_grid_adc_sums;
		// For each event
		for (int e = 0; e < t->GetEntries(); e++){

			g.fill_grid_ttree_entry(*t, e, true);

			//ModifiedAggregation ma(g, seed_threshold, aggregation_threshold);
			ModifiedAggregation ma(seed_threshold, aggregation_threshold);
			ma.set_grid(g);
			
			// Any clusters found
			if (ma.tag()){
				std::string main_cluster = ma.get_largest_cluster();
				cluster_adc_sums.push_back(ma.get_cluster_sum(main_cluster));
				leftover_grid_adc_sums.push_back(g.get_grid_sum() - ma.get_cluster_sum(main_cluster));


				//main_cluster_vec.push_back(main_cluster);
				//std::vector<std::string> remaining_clusters = ma.get_remaining_clusters(main_cluster_vec);
				//if (remaining_clusters.size() > 0){
				//	std::string max = remaining_clusters.at(0);
				//	for (auto &v : remaining_clusters){
				//		if (ma.get_cluster_max_adc(v) > ma.get_cluster_max_adc(max))
				//			max = v;
				//	}
				//	leftover_max_adc_temp.push_back(ma.get_cluster_max_adc(max));
				//}else{
				//	leftover_max_adc_temp.push_back(0);
				//}



			// No clusters found
			}else{
				cluster_adc_sums.push_back(0.0);
				leftover_grid_adc_sums.push_back(g.get_grid_sum());
			}
			grid_adc_sums.push_back(g.get_grid_sum());
		}

		cluster_adc_sums_energy.push_back(std::make_pair(energy_temp, cluster_adc_sums));
		grid_adc_sums_energy.push_back(std::make_pair(energy_temp, grid_adc_sums));
		leftover_grid_adc_sums_energy.push_back(std::make_pair(energy_temp, leftover_grid_adc_sums));
	}



	std::vector<std::tuple<int, double, double>> grid_fits;
	std::vector<std::tuple<int, double, double>> cluster_fits;
	std::vector<std::tuple<int, double, double>> leftover_grid_fits;

	int grid_max = 0;


	std::unique_ptr<THStack> grid_stack = std::make_unique<THStack>("grid_stack", "");

	// GRID HISTOGRAMS 

	std::unique_ptr<TCanvas> grid_canvas = std::make_unique<TCanvas>("grid", "grid", 1);
	grid_canvas->cd();
	std::cout << "Total signal Gaussian fits" << std::endl;
	for (int i = 0; i < files.size(); i++){
		std::cout << "Energy: " << std::to_string(grid_adc_sums_energy.at(i).first) << std::endl;
		//std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 100, 0, 150000);
		std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", grid_bins, 0, grid_xmax);
		std::unique_ptr<TF1> fit = std::make_unique<TF1>("fit", "gaus");
		for (auto &v : grid_adc_sums_energy.at(i).second)
			hist->Fill(v);
		fit->SetParameter(0, hist->GetMaximum());
		fit->SetParameter(1, hist->GetMean());
		fit->SetParameter(2, hist->GetRMS());
		//std::cout << "Max: " << hist->GetMaximum() << " at: " << hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin()) << std::endl;
		std::string temp_title = std::to_string(grid_adc_sums_energy.at(i).first) + " GeV";
		hist->SetTitle(temp_title.c_str());
		hist->SetFillColor(0);
		hist->SetLineColor(i+1);
		hist->SetStats(0);
		hist->GetXaxis()->SetTitle("ADC sum");
		hist->GetYaxis()->SetTitle("Count");

		fit->SetLineStyle(2);
		fit->SetLineColor(i+1);

		hist->SetMaximum(grid_hist_max);

		hist->Draw("same");
		hist->Fit("fit", "0");
		grid_fits.push_back(std::make_tuple(grid_adc_sums_energy.at(i).first, fit->GetParameter(1), fit->GetParError(1)));
		//grid_stack->Add(hist.get());




		hist.release();
		std::cout << std::endl << std::endl;
	}

	//grid_stack->GetXaxis()->SetTitle("ADC sum");
	//grid_stack->GetYaxis()->SetTitle("Count");
	//grid_stack->Draw("nostack");

	std::string grid_hist_filename = folder + mc_tb_affix + "_grid_hist" + IMAGE_EXTENSION;
	//std::string title = ";" + x_label + ";" + y_label;

	
	TLegend *grid_leg = grid_canvas->BuildLegend();
	grid_leg->SetHeader("ADC sum histogram", "C");
	grid_leg->SetTextSize(0.023);

	//c->SetTopMargin(0.2);
	gStyle->SetOptTitle(0);
	grid_canvas->SetRightMargin(0.1);
	grid_canvas->SetLeftMargin(0.15);
	grid_canvas->Update();
	grid_canvas->SaveAs(grid_hist_filename.c_str());




	// CLUSTER HISTOGRAMS

	std::unique_ptr<THStack> cluster_stack = std::make_unique<THStack>("grid_stack", "");
	// Cluster fits and plots
	std::unique_ptr<TCanvas> cluster_canvas = std::make_unique<TCanvas>("cluster", "cluster", 1);
	cluster_canvas->cd();
	std::cout << "Clustered signal Gaussian fits" << std::endl;
	for (int i = 0; i < files.size(); i++){
		std::cout << "Energy: " << std::to_string(cluster_adc_sums_energy.at(i).first) << std::endl;
		//std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 100, 0, 150000);
		std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", cluster_bins, 0, cluster_xmax);
		std::unique_ptr<TF1> fit = std::make_unique<TF1>("fit", "gaus");
		for (auto &v : cluster_adc_sums_energy.at(i).second)
			hist->Fill(v);
		fit->SetParameter(0, hist->GetMaximum());
		fit->SetParameter(1, hist->GetMean());
		fit->SetParameter(2, hist->GetRMS());
		//std::cout << "Max: " << hist->GetMaximum() << " at: " << hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin()) << std::endl;
		std::string temp_title = std::to_string(cluster_adc_sums_energy.at(i).first) + " GeV";
		hist->SetTitle(temp_title.c_str());
		hist->SetFillColor(0);
		hist->SetLineColor(i+1);
		hist->SetStats(0);
		hist->GetXaxis()->SetTitle("ADC sum");
		hist->GetYaxis()->SetTitle("Count");
		hist->SetMaximum(cluster_hist_max);

		hist->Draw("same");
		hist->Fit("fit", "0");
		cluster_fits.push_back(std::make_tuple(cluster_adc_sums_energy.at(i).first, fit->GetParameter(1), fit->GetParError(1)));

		//cluster_stack->Add(hist.get());

		hist.release();
		std::cout << std::endl << std::endl;
	}

	//cluster_stack->GetXaxis()->SetTitle("ADC sum");
	//cluster_stack->GetYaxis()->SetTitle("Count");
	//cluster_stack->Draw("nostack");

	std::string cluster_hist_filename = folder + mc_tb_affix + "_cluster_hist" + IMAGE_EXTENSION;
	//std::string title = ";" + x_label + ";" + y_label;

	
	TLegend *cluster_leg = cluster_canvas->BuildLegend();
	cluster_leg->SetHeader("Cluster histogram", "C");
	cluster_leg->SetTextSize(0.023);

	//c->SetTopMargin(0.2);
	gStyle->SetOptTitle(0);
	cluster_canvas->SetRightMargin(0.1);
	cluster_canvas->SetLeftMargin(0.15);
	cluster_canvas->Update();
	cluster_canvas->SaveAs(cluster_hist_filename.c_str());




	// LEFTOVER GRID HISTOGRAMS
	std::unique_ptr<TCanvas> leftover_grid_canvas = std::make_unique<TCanvas>("leftover_grid", "leftover_grid", 1);
	leftover_grid_canvas->cd();
	std::cout << "Leftover signal Gaussian fits" << std::endl;
	for (int i = 0; i < files.size(); i++){
		std::cout << "Energy: " << std::to_string(leftover_grid_adc_sums_energy.at(i).first) << std::endl;
		//std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", 100, 0, 150000);
		std::unique_ptr<TH1D> hist = std::make_unique<TH1D>("hist", "", leftover_grid_bins, 0, leftover_grid_xmax);
		std::unique_ptr<TF1> fit = std::make_unique<TF1>("fit", "gaus");
		for (auto &v : leftover_grid_adc_sums_energy.at(i).second)
			hist->Fill(v);
		fit->SetParameter(0, hist->GetMaximum());
		fit->SetParameter(1, hist->GetMean());
		fit->SetParameter(2, hist->GetRMS());
		//std::cout << "Max: " << hist->GetMaximum() << " at: " << hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin()) << std::endl;
		std::string temp_title = std::to_string(leftover_grid_adc_sums_energy.at(i).first) + "GeV";
		hist->SetTitle(temp_title.c_str());
		hist->SetFillColor(0);
		hist->SetLineColor(i+1);
		hist->SetStats(0);
		hist->GetXaxis()->SetTitle("ADC sum");
		hist->GetYaxis()->SetTitle("Count");


		hist->SetMaximum(leftover_grid_hist_max);

		hist->Draw("same");
		hist->Fit("fit", "0");
		leftover_grid_fits.push_back(std::make_tuple(leftover_grid_adc_sums_energy.at(i).first, fit->GetParameter(1), fit->GetParError(1)));

		hist.release();
		std::cout << std::endl << std::endl;
	}


	std::string leftover_grid_hist_filename = folder + mc_tb_affix + "_leftover_grid_hist" + IMAGE_EXTENSION;
	//std::string title = ";" + x_label + ";" + y_label;

	
	TLegend *leftover_grid_leg = leftover_grid_canvas->BuildLegend();
	leftover_grid_leg->SetHeader("Leftover ADC sum", "C");
	leftover_grid_leg->SetTextSize(0.023);

	//c->SetTopMargin(0.2);
	gStyle->SetOptTitle(0);
	leftover_grid_canvas->SetRightMargin(0.1);
	leftover_grid_canvas->SetLeftMargin(0.15);
	leftover_grid_canvas->Update();
	leftover_grid_canvas->SaveAs(leftover_grid_hist_filename.c_str());



	// LINEARIZATION

	std::vector<std::unique_ptr<TGraphErrors>> lin_plots;
	std::unique_ptr<TGraphErrors> grid_points = std::make_unique<TGraphErrors>();
	std::unique_ptr<TGraphErrors> cluster_points = std::make_unique<TGraphErrors>();
	grid_points->SetName("Total ADC");
	grid_points->SetMarkerStyle(21);
	grid_points->SetMarkerColor(kBlue);
	cluster_points->SetName("Dominant cluster");
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
	lin_plots.push_back(std::move(grid_points));
	lin_plots.push_back(std::move(cluster_points));
	std::string lin_plot_x_label = "Energy [GeV]";
	std::string lin_plot_y_label = "ADC Counts";
	save_point_plot(lin_plots, lin_plot_x_label, lin_plot_y_label, lin_plot_legend_title, lin_plot_filename);



	// LINEARIZATION LEFTOVER GRID

	std::vector<std::unique_ptr<TGraphErrors>> leftover_lin_plots;
	std::unique_ptr<TGraphErrors> full_grid_points = std::make_unique<TGraphErrors>();
	std::unique_ptr<TGraphErrors> cluster_points2 = std::make_unique<TGraphErrors>();
	std::unique_ptr<TGraphErrors> leftover_grid_points = std::make_unique<TGraphErrors>();
	full_grid_points->SetName("Total ADC");
	full_grid_points->SetMarkerStyle(21);
	full_grid_points->SetMarkerColor(kBlue);
	cluster_points2->SetName("Dominant cluster");
	cluster_points2->SetMarkerStyle(22);
	cluster_points2->SetMarkerColor(kRed);
	leftover_grid_points->SetName("Leftover ADC after removing largest cluster");
	leftover_grid_points->SetMarkerStyle(23);
	leftover_grid_points->SetMarkerColor(kGreen);

	for (int i = 0; i < grid_fits.size(); i++){
		full_grid_points->SetPoint(i, std::get<0>(grid_fits.at(i)), std::get<1>(grid_fits.at(i)));
		full_grid_points->SetPointError(i, 0, std::get<2>(grid_fits.at(i)));

		cluster_points2->SetPoint(i, std::get<0>(cluster_fits.at(i)), std::get<1>(cluster_fits.at(i)));
		cluster_points2->SetPointError(i, 0, std::get<2>(cluster_fits.at(i)));

		leftover_grid_points->SetPoint(i, std::get<0>(leftover_grid_fits.at(i)), std::get<1>(leftover_grid_fits.at(i)));
		leftover_grid_points->SetPointError(i, 0, std::get<2>(leftover_grid_fits.at(i)));

	}

	std::unique_ptr<TF1> full_grid_lin_fit = std::make_unique<TF1>("grid_lin_fit", "pol1", 0, 400);
	std::unique_ptr<TF1> cluster_lin_fit2 = std::make_unique<TF1>("cluster_lin_fit2", "pol1", 0, 400);
	std::unique_ptr<TF1> leftover_grid_lin_fit = std::make_unique<TF1>("leftover_grid_lin_fit", "pol1", 0, 400);
	full_grid_lin_fit->SetLineColor(kBlue);
	full_grid_lin_fit->SetLineStyle(2);
	cluster_lin_fit2->SetLineColor(kRed);
	cluster_lin_fit2->SetLineStyle(2);
	leftover_grid_lin_fit->SetLineColor(kGreen);
	leftover_grid_lin_fit->SetLineStyle(2);
	full_grid_points->Fit("grid_lin_fit", "R");
	cluster_points2->Fit("cluster_lin_fit2", "R");
	leftover_grid_points->Fit("leftover_grid_lin_fit", "R");
	leftover_lin_plots.push_back(std::move(full_grid_points));
	leftover_lin_plots.push_back(std::move(cluster_points2));
	leftover_lin_plots.push_back(std::move(leftover_grid_points));
	std::string leftover_lin_plot_x_label = "Energy [GeV]";
	std::string leftover_lin_plot_y_label = "ADC Counts";
	save_point_plot(leftover_lin_plots, leftover_lin_plot_x_label, leftover_lin_plot_y_label, leftover_lin_plot_legend_title, leftover_lin_plot_filename);




	return true;
}





void analysis(std::string folder){


	std::cout << "Clustering and looking at cluster quality..." << std::endl;


	std::vector<std::pair<double, double>> MA_params;
	ANALYSIS_TAG = "_DIFFSEED";
	MA_params.push_back(std::make_pair(100, 10));
	MA_params.push_back(std::make_pair(300, 10));
	MA_params.push_back(std::make_pair(500, 10));
	MA_params.push_back(std::make_pair(800, 10));
	MA_params.push_back(std::make_pair(1000, 10));

	//ANALYSIS_TAG = "_DIFFAGG";
	//MA_params.push_back(std::make_pair(800, 10));
	//MA_params.push_back(std::make_pair(800, 100));
	//MA_params.push_back(std::make_pair(800, 300));
	//MA_params.push_back(std::make_pair(800, 400));
	//MA_params.push_back(std::make_pair(800, 500));

	//std::string folder = "../data/focalsim/pi_plus_1000e_deg0/";
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


	/*
	Grid g;
	if (!focal_h_grid(g)) return false;
	calc_neighbors_all(g);
	*/

	FoCalH focal;
	Grid &g = focal.get_grid();

	for (int i = 0; i < files.size(); i++){
		std::unique_ptr<TFile> f = std::make_unique<TFile>(files.at(i).c_str(), "READ");
		auto t = static_cast<TTree*>(f->Get("T"));
		int energy_temp = static_cast<TParameter<int>*>(f->Get("energy"))->GetVal();

		std::vector<double> cluster_count_temp;
		std::vector<double> fraction_of_energy_temp;
		std::vector<double> leftover_max_adc_temp;
		std::vector<std::vector<std::pair<double, double>>> cluster_center_of_mass_temp;
		for (int e = 0; e < t->GetEntries(); e++){

			g.fill_grid_ttree_entry(*t, e, true);

			ModifiedAggregation ma(g, MA_seed_threshold, MA_aggregation_threshold);
			
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
	//std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 2400, 2400);
	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 2);
	c->cd();
	//gStyle->SetOptStat(0);
	hist->GetXaxis()->SetTitle("x [cm]");
	hist->GetYaxis()->SetTitle("y [cm]");
	hist->SetStats(0);
	hist->SetTitle(title.c_str());
	hist->Draw();
	c->Update();
	c->SaveAs(filename.c_str());
}

void save_neighborhood(Grid &g, Cell* cell, std::string filename, std::string title){
	std::unique_ptr<TH2Poly> hist = g.plot_cell_neighbors(cell);
	hist->SetMaximum(2);
	//std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 2400, 2400);
	std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>("c", "c", 2);
	c->cd();
	//gStyle->SetOptStat(0);
	hist->GetXaxis()->SetTitle("x [cm]");
	hist->GetYaxis()->SetTitle("y [cm]");
	hist->SetStats(0);
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















// Move to class
// Class focal_geometry subclass of Grid
//

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
    std::regex reg(".root$",
            std::regex_constants::ECMAScript | std::regex_constants::icase);

    for (const auto & entry : std::filesystem::directory_iterator(folder))
		if (std::regex_search(entry.path().u8string(), reg)){
			files.push_back(entry.path().u8string());
		}
	return files;
}



