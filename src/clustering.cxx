#include "clustering.h"
#include <iostream>

Clustering::Clustering(Grid &_g){
	set_grid(_g);
}


void Clustering::set_grid(Grid &_g){
	g = &_g;
	for (auto &v : *g->get_cells()){
		untagged_cells.push_back(v.get());
	}
}


void Clustering::print_untagged(){
	std::cout << "Size: " << untagged_cells.size() << std::endl;
	for (auto &v : untagged_cells)
		std::cout << v->get_id() << " x:" << v->get_x_position() << " y:" << v->get_y_position() << " val:" << v->get_value() << std::endl;
}


void Clustering::print_tagged(){
	std::cout << "Size: " << tagged_cells.size() << std::endl;
	for (auto &v : tagged_cells)
		std::cout << v.first->get_id() << " x:" << v.first->get_x_position() << " y:" << v.first->get_y_position() << " val:" << v.first->get_value() << " tag:" << v.second << std::endl;
}

void Clustering::add_tag(std::string tag, Cell* c){
	tagged_cells[c] = tag;
	auto it = std::find(untagged_cells.begin(), untagged_cells.end(), c);
	if (it != untagged_cells.end())
		untagged_cells.erase(it);

}

bool Clustering::is_tagged(Cell* c){
	if (auto search = tagged_cells.find(c); search != tagged_cells.end()){
		return true;
	}
	return false;
}

Cell* Clustering::find_maximum_untagged(){
	// check for tagged
	
	Cell* max = untagged_cells.at(0);
	for (const auto& v : untagged_cells){
		if (is_tagged(v)){
			continue;
		}
		if (v->get_value() > max->get_value()){
			max = v;
		}
	}

	if (is_tagged(max))
		return nullptr;
	return max;
}


double Clustering::get_sum(){
	double sum = 0;
	for (const auto &v : *g->get_cells())
		sum += v->get_value();
	return sum;
}

int Clustering::get_cluster_count(){
	std::set<std::string> unique;
	for (auto &v : tagged_cells)
		unique.insert(v.second).second;
	return unique.size();
}

double Clustering::get_cluster_max_adc(std::string tag){
	std::vector<double> adc_values;
	for (auto v : tagged_cells)
		if (v.second == tag)
			adc_values.push_back(v.first->get_value());

	auto max_elem = std::max_element(adc_values.begin(), adc_values.end());
	if (max_elem == adc_values.end())
		return -1;
	return *max_elem;
}


double Clustering::get_cluster_sum(std::string tag){
	double sum = 0;
	for (auto v : tagged_cells)
		if (v.second == tag)
			sum += v.first->get_value();
	return sum;
}

std::vector<std::string> Clustering::get_clusters(){
	std::set<std::string> unique;
	std::vector<std::string> tags;
	for (auto v : tagged_cells)
		unique.insert(v.second).second;

	for (auto v : unique)
		tags.push_back(v);

	return tags;
}

std::string Clustering::get_largest_cluster(){
	std::vector<std::string> clusters = get_clusters();
	auto max_it = clusters.begin();
	if (max_it == clusters.end())
		return "N/A";
	std::string max = *max_it;
	for (auto &v : clusters)
		if (get_cluster_sum(v) > get_cluster_sum(max))
			max = v;
	return max;
}


std::vector<std::string> Clustering::get_remaining_clusters(std::vector<std::string> &exclude_clusters){

	std::vector<std::string> remaining_clusters;
	for (auto v : get_clusters())
		for (auto excl : exclude_clusters)
			if (v != excl)
				remaining_clusters.push_back(v);

	return remaining_clusters;
}

std::pair<double, double> Clustering::cluster_center_of_mass(std::string tag){
	// sum coordinates times value
	// divide by sum
	
	double x_mean = 0.0;
	double y_mean = 0.0;
	for (const auto &v : *get_tagged_cells())
		if (v.second == tag){
			x_mean += v.first->get_x_position() * v.first->get_value();
			y_mean += v.first->get_y_position() * v.first->get_value();
		}
	x_mean /= get_cluster_sum(tag);
	y_mean /= get_cluster_sum(tag);
	
	return std::make_pair(x_mean, y_mean);

}




bool Clustering::cluster_events(TTree &tree, unsigned long start, unsigned long end){

	// Check bounds
	if (start < 0) return false;
	if (end > tree.GetEntries()) return false;


	for (int e = start; e < end; e++){
		// Cluster
		g->fill_grid_ttree_entry(tree, e, true);


		// Save to file
	}

	return false;

}
