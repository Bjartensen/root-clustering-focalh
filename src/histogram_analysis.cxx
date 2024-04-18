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
