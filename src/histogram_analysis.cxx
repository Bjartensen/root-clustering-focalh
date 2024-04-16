#include "histogram_analysis.h"

#include <iostream>

void HistogramAnalysis::calculate_sums(){

	// Get files
	// open files with reader
	// read events and add sums of grid and primary cluster

	//std::vector<std::string> files = get_files();
	for (const auto &v : get_files()){
		ClusterReader reader(v);
		reader.open();
		TTreeClustered::EventPtr event;
		std::cout << v << std::endl;
		unsigned long adc_sum = 0;
		while (reader.read_event(event)){
			for (const auto &u : *event.value) adc_sum+=u; // not for all..
		}
		std::cout << adc_sum << std::endl;
		reader.close();
		//
	}

	//ClusterReader reader;
}
