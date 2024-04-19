#include "histogram_analysis.h"

#include <iostream>

void HistogramAnalysis::calculate_sums(){

	// Get files
	// open files with reader
	// read events and add sums of grid and primary cluster

	for (const auto &v : get_files()){
		ClusterReader reader(v);
		reader.open();

		reader.read_events_header();
		TTreeClustered::EventPtr event;
		std::cout << v << std::endl;
		std::vector<General::float_type> sums;
		while (reader.read_event(event)){
			unsigned long adc_sum = 0;
			for (const auto &u : *event.value){
				adc_sum+=u;
			}
			sums.push_back(adc_sum);
		}
		grid_sums.push_back(std::make_pair(reader.get_events_header().Energy, sums));
		reader.close();
	}
}



void HistogramAnalysis::squawk(){
	std::cout << "Grid sums:";
	std::cout << std::endl;
	for (const auto &v : grid_sums){
		std::cout << '\t';
		std::cout << v.first << ", " << v.second.size() << std::endl;
	}

}


void HistogramAnalysis::plot(std::string name, std::string title){
  PlotHistogram hist;
  hist.create_tcanvas(name, title);
  hist.set_x_max(calculate_hist_x_max());
  hist.set_bins(calculate_hist_bins());
  hist.set_x_axis_label("Sum");
  hist.set_y_axis_label("Count");
  for (int i = 0; i < grid_sums.size(); i++){
    hist.create_histogram(grid_sums.at(i).second, name, title);
  }
  hist.save_to_file("test");

}


double HistogramAnalysis::calculate_hist_x_max(){
  double max = 0;
  for (const auto &energy : grid_sums)
    for (const auto &v : energy.second)
      if (v > max) max = v;
  return max;
}

double HistogramAnalysis::calculate_hist_y_max(){
  return 0;
}

int HistogramAnalysis::calculate_hist_bins(){
  return (int)(std::sqrt(grid_sums.at(0).second.size()) * 1.6); // ??
}

