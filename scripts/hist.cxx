#include "ext_libraries.h"
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <cmath>
#include <TGraphErrors.h>
//#include <TAttMarker.h>
//#include <TPaveText.h>
#include <TLegend.h>
#include <iostream>
#include <fstream>


std::pair<Double_t, Double_t> center_of_mass(std::vector<Double_t> *x_pos, std::vector<Double_t>* y_pos, std::vector<Double_t> * values);
std::pair<Double_t, Double_t> standard_deviation(std::vector<Double_t> *x_pos, std::vector<Double_t>* y_pos, std::vector<Double_t> * values);

Double_t standard_error(std::vector<Double_t> *samples);

TGraphErrors* get_graph(std::vector<Double_t> *x_vec, std::vector<Double_t> *y_vec, std::vector<Double_t> *y_vec_err, std::string x_title, std::string y_title);

int main(int argc, char* argv[]){

	std::string focalsim_analysis_dir = "../data/focalsim/pi_plus/";

	if (argc == 2)
		focalsim_analysis_dir = argv[1];
	else
		focalsim_analysis_dir = "../data/focalsim/pi_plus/";

	const std::string readme_filename = focalsim_analysis_dir + "README.txt";

	const std::string out_dir = "../data/misc/";
	const std::string tree_name = "T";

	const long unsigned int arr_len = 16;
	std::array<std::string, arr_len> files = {	"20_10_analysis.root"
												, "40_10_analysis.root"
												, "60_10_analysis.root"
												, "100_10_analysis.root"
												, "120_10_analysis.root"
												, "140_10_analysis.root"
												, "160_10_analysis.root"
												, "180_10_analysis.root"
												, "200_10_analysis.root"
												, "220_10_analysis.root"
												, "240_10_analysis.root"
												, "260_10_analysis.root"
												, "280_10_analysis.root"
												, "300_10_analysis.root"
												, "320_10_analysis.root"
												, "350_10_analysis.root"};

	std::array<std::string, arr_len> energy = {	"20", "40", "60", "100"
												, "120", "140", "160", "180"
												, "200", "220", "240", "260"
												, "280", "300", "320", "350"};

	std::vector<Double_t> energy_num = {	20, 40, 60, 100
												, 120, 140, 160, 180
												, 200, 220, 240, 260
												, 280, 300, 320, 350};
	//std::array<std::string, arr_len> files = {"350_10_analysis.root"};


	std::vector<Double_t> max_adc;
	std::vector<Double_t> max_adc_err;
	std::vector<Double_t> mean_x;
	std::vector<Double_t> mean_x_err;
	std::vector<Double_t> mean_y;
	std::vector<Double_t> mean_y_err;
	std::vector<Double_t> sigma_x;
	std::vector<Double_t> sigma_x_err;
	std::vector<Double_t> sigma_y;
	std::vector<Double_t> sigma_y_err;

	std::string readme_particle;
	std::string readme_angle;
	std::ifstream README;
	README.open(readme_filename);
	if (README.is_open()){
		std::getline(README, readme_particle);
		std::getline(README, readme_angle);
		std::cout << readme_particle << std::endl;
	}else{
		std::cout << "No README.txt found." << std::endl;
		readme_particle = "";
		readme_angle = "";
	}
	README.close();

	std::string out_file_path = focalsim_analysis_dir + "analysis.root";
	std::unique_ptr<TFile> out_file = std::make_unique<TFile>(out_file_path.c_str(), "RECREATE");

	for (int fi = 0; fi < arr_len; fi++){
		std::string t = focalsim_analysis_dir + files[fi];
		const char* filename = t.c_str();
		std::unique_ptr<TFile> analysis_file = std::make_unique<TFile>(filename, "READ");
		auto tree = static_cast<TTree*>(analysis_file->Get(tree_name.c_str()));

		std::cout << "File: " << filename << std::endl;

		std::vector<Double_t>* x_pos = nullptr;
		std::vector<Double_t>* y_pos = nullptr;
		std::vector<Double_t>* values = nullptr;
		std::vector<Double_t>* particle = nullptr;
		tree->SetBranchAddress("x_pos", &x_pos);
		tree->SetBranchAddress("y_pos", &y_pos);
		tree->SetBranchAddress("value", &values);
		tree->SetBranchAddress("particle", &particle);


		std::vector<Double_t> temp_max_adc;
		std::vector<Double_t> temp_mean_x;
		std::vector<Double_t> temp_mean_y;
		std::vector<Double_t> temp_sigma_x;
		std::vector<Double_t> temp_sigma_y;


		for (int entry = 0; tree->LoadTree(entry) >= 0; entry++){

			analysis_file->cd(); // Not sure if needed
			tree->GetEntry(entry);



			/*

			double x[] = {1.0, 2.0, 2.25, 2.5, 2.75, 4.0, 6.0};
			double y[] = {1.0, 2.0, 2.25, 2.5, 2.75, 4.0, 6.0};
			double vals[] = {1, 1, 1, 1, 1, 1, 1};
			const double x_low[] = { 0.0, 2.0, 2.25, 2.5, 2.75, 4.0, 6.0};
			const double y_low[] = { 0.0, 2.0, 2.25, 2.5, 2.75, 4.0, 6.0};
			//auto h = new TH2D("h", "Histogram", 6, x_low, 6, y_low);

			out_file->cd(); // Seems to be needed to correctly write

			auto h = new TH2D("h", "Histogram", 16, -10, 10, 16, -10, 10);
	//		for (int i = 0; i < 6; i++){
	//			h->Fill(x[i], y[i], vals[i]);
	//		}

			for (int i = 0; i < x_pos->size(); i++){
				h->Fill(x_pos->at(i), y_pos->at(i), values->at(i));
			}

			//for (int i = 0; i < x_pos->size(); i++){
			//	std::cout << x_pos->at(i) << "," << y_pos->at(i) << std::endl;
			//}

			//for (int i = 0; i<4; i++){
			//	for (int j = 0; j<4; j++){
			//		h->Fill(x[i], y[j], vals[i+j]);
			//	}
			//}

			std::string canvas_name = "hist_"+energy[fi];
			TCanvas *c1 = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 200, 10, 700, 500);
			h->Draw("colz");
			c1->Update();
			c1->Write();

			delete c1;
			delete h;
			*/

			
			std::pair CoM = center_of_mass(x_pos, y_pos, values);
			std::pair std_xy = standard_deviation(x_pos, y_pos, values);
			temp_max_adc.push_back(*std::max_element(values->begin(), values->end()));
			temp_mean_x.push_back(CoM.first);
			temp_mean_y.push_back(CoM.second);
			temp_sigma_x.push_back(std_xy.first);
			temp_sigma_y.push_back(std_xy.second);



		}

		analysis_file->Close();
		out_file->Write();


		Double_t max_adc_avg = 0;
		Double_t mean_x_avg = 0;
		Double_t mean_y_avg = 0;
		Double_t sigma_x_avg = 0;
		Double_t sigma_y_avg = 0;

		Double_t max_adc_standard_error = standard_error(&temp_max_adc);
		Double_t mean_x_standard_error = standard_error(&temp_mean_x);
		Double_t mean_y_standard_error = standard_error(&temp_mean_y);
		Double_t sigma_x_standard_error = standard_error(&temp_sigma_x);
		Double_t sigma_y_standard_error = standard_error(&temp_sigma_y);


		for (int i = 0; i < temp_max_adc.size(); i++){
			max_adc_avg += temp_max_adc.at(i);
			mean_x_avg += temp_mean_x.at(i);
			mean_y_avg += temp_mean_y.at(i);
			sigma_x_avg += temp_sigma_x.at(i);
			sigma_y_avg += temp_sigma_y.at(i);
		}

		max_adc_avg /= temp_max_adc.size();
		mean_x_avg /= temp_mean_x.size();
		mean_y_avg /= temp_mean_y.size();
		sigma_x_avg /= temp_sigma_x.size();
		sigma_y_avg /= temp_sigma_y.size();


		max_adc.push_back(max_adc_avg);
		mean_x.push_back(mean_x_avg);
		mean_y.push_back(mean_y_avg);
		sigma_x.push_back(sigma_x_avg);
		sigma_y.push_back(sigma_y_avg);
		
		max_adc_err.push_back(max_adc_standard_error);
		mean_x_err.push_back(mean_x_standard_error);
		mean_y_err.push_back(mean_y_standard_error);
		sigma_x_err.push_back(sigma_x_standard_error);
		sigma_y_err.push_back(sigma_y_standard_error);
		

	}


	for (int i = 0; i < max_adc.size(); i++){
		std::cout << "E: " << energy[i] << ", max_adc: " << max_adc.at(i) << ", mean_x: " << mean_x.at(i) << ", mean_y: " << mean_y.at(i) << ", sigma_x: " << sigma_x.at(i) << ", sigma_y: " << sigma_y.at(i) << std::endl;
	}


	out_file->cd();

	//gROOT->SetStyle("Bold"); // Set the current style to "Plain"

	// TO-DO: Maybe move means and sigmas into single graphs
	// Max ADC
	TCanvas *c = new TCanvas("Characterization", "", 200, 10, 1100, 1500);
	c->Divide(2,3,0.005,0.005);
	c->cd(1);
	auto graph1 = get_graph(&energy_num, &max_adc, &max_adc_err, "Energy", "Max ADC");

	// Mean x
	c->cd(3);
	auto graph2 = get_graph(&energy_num, &mean_x, &mean_x_err, "Energy", "Mean x");

	// Mean y
	c->cd(4);
	auto graph3 = get_graph(&energy_num, &mean_y, &mean_y_err, "Energy", "Mean y");

	// Sigma x
	c->cd(5);
	auto graph4 = get_graph(&energy_num, &sigma_x, &sigma_x_err, "Energy", "Sigma x");

	// Sigma y
	c->cd(6);
	auto graph5 = get_graph(&energy_num, &sigma_y, &sigma_y_err, "Energy", "Sigma y");

	

	c->cd(2);

	TLegend *leg = new TLegend(0.1, 0.5, 0.9, 0.9);
	//leg->SetHeader("Shower characterization", "c");
	leg->AddEntry((TObject*)0, "Focalsim showers", "");
	leg->AddEntry((TObject*)0, readme_particle.c_str(), "");
	leg->AddEntry((TObject*)0, readme_angle.c_str(), "");
	//leg->AddEntry(graph1, "adsf", "");
	leg->Draw();

	c->cd();

	c->Update();
	c->Write();
	delete c;
	delete graph1;
	delete graph2;
	delete graph3;
	delete graph4;
	delete graph5;

	out_file->Close();

	return 0;
}



std::pair<Double_t, Double_t> center_of_mass(std::vector<Double_t> *x_pos, std::vector<Double_t>* y_pos, std::vector<Double_t> * values){
	Double_t x_mean = 0.0;
	Double_t y_mean = 0.0;
	Double_t adc_total = 0.0;
	for (int v : *values) adc_total += v;

	for (int i = 0; i < x_pos->size(); i++) x_mean += x_pos->at(i) * values->at(i);
	x_mean /= adc_total;

	for (int i = 0; i < y_pos->size(); i++) y_mean += y_pos->at(i) * values->at(i);
	y_mean /= adc_total;

	return {x_mean, y_mean};
}


std::pair<Double_t, Double_t> standard_deviation(std::vector<Double_t> *x_pos, std::vector<Double_t>* y_pos, std::vector<Double_t> * values){
	Double_t x_std = 0.0;
	Double_t y_std = 0.0;
	std::pair CoM = center_of_mass(x_pos, y_pos, values);
	Double_t adc_total = 0.0;
	for (int v : *values) adc_total += v;

	Double_t var_x = 0;
	for (int i = 0; i < x_pos->size(); i++){
		var_x += values->at(i) * std::pow((x_pos->at(i) - CoM.first), 2);
	}
	var_x /=adc_total;
	x_std = std::sqrt(var_x);

	Double_t var_y = 0;
	for (int i = 0; i < y_pos->size(); i++){
		var_y += values->at(i) * std::pow((y_pos->at(i) - CoM.second), 2);
	}
	var_y /=adc_total;
	y_std = std::sqrt(var_y);

	return {x_std, y_std};
}


Double_t standard_error(std::vector<Double_t> *samples){
	Double_t mean = 0.0;
	Double_t std_dev = 0.0;
	Double_t standard_error = 0.0;

	for (auto v : *samples) mean += v;
	mean /= samples->size();

	for (auto v : *samples) std_dev += std::pow((v - mean), 2);
	//std_dev /= samples->size() - 1;
	std_dev /= samples->size(); // Surely I want sample deviation and not population deviation??
	std_dev = std::sqrt(std_dev);
	
	standard_error = std_dev/std::sqrt(samples->size());
	return standard_error;
}

TGraphErrors* get_graph(std::vector<Double_t> *x_vec, std::vector<Double_t> *y_vec, std::vector<Double_t> *y_vec_err, std::string x_title, std::string y_title){

	const float size = 0.05;
	TGraphErrors *ret_graph = new TGraphErrors();

	ret_graph->GetXaxis()->SetTitle(x_title.c_str());
	ret_graph->GetYaxis()->SetTitle(y_title.c_str());

	ret_graph->GetXaxis()->SetTitleSize(size);
	ret_graph->GetYaxis()->SetTitleSize(size);

	ret_graph->GetXaxis()->SetLabelSize(size);
	ret_graph->GetYaxis()->SetLabelSize(size);


	for (int i = 0; i < y_vec->size(); i++){
		ret_graph->SetPoint(i, x_vec->at(i), y_vec->at(i));
		ret_graph->SetPointError(i, 0, y_vec_err->at(i));
	}

	//ret_graph->SetMarkerStyle(2);
	ret_graph->Draw("A*");

	return ret_graph;

}

