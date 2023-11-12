#include "clustering.h"
#include <iostream>

Clustering::Clustering(Grid *_g){
	g = _g;
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





/////////////////////////////
/////////////////////////////
/////////////////////////////
/////////////////////////////
/////////////////////////////
void CellularAutomata::tag(){
	// Always go through all cells and check:
	//	Do they have neighbors?
	//		Do the neighbors have tags?
	//			Add tags.

}

bool CellularAutomata::find_maxima(){
	std::vector<Cell*> c;
	for (auto &v : *get_grid()->get_cells()){
		c.push_back(v.get());
	}
	std::sort(c.begin(), c.end(), greater);

	// can compare pointers
	Cell* max = c.at(0);
	for (auto v : c){
		if (is_maximum(v))
			std::cout << "Maximum at " << v->get_id() << ": " << v->get_value() << std::endl;
	}


	return true;
}

bool CellularAutomata::is_maximum(Cell *c){
	for (auto v : *c->get_neighbors())
		if (v->get_value() > c->get_value()) // 4095 may be next to each other...
			return false;
	return true;
}



///////////////////////////////
///////////////////////////////
///////////////////////////////
///////////////////////////////
///////////////////////////////
///////////////////////////////
///////////////////////////////
void ModifiedAggregation::tag(){
	Cell* c = nullptr;

	unsigned int tag_num = 1;
	bool more_clusters = true;
	while (more_clusters){
		more_clusters = false;
		c = find_maximum_untagged();
		if (c){
			add_tag(std::to_string(tag_num), c);
			spread(c);
			more_clusters = true;
			tag_num++;
		}
	}
}


bool ModifiedAggregation::spread(Cell *c){
	std::string cluster_tag = get_tag(c);


	bool more_spread = true;
	while (more_spread){
		more_spread = false;

		std::vector<Cell*> tagged_cells_local;
		for (auto &m : *get_tagged_cells()){
			if (m.second == cluster_tag){
				tagged_cells_local.push_back(m.first);
			}
		}

		for (auto &tc : tagged_cells_local){
			for (auto &v : *tc->get_neighbors()){
				if (is_tagged(v))
					continue;
				if (v->get_value() < aggregation_threshold)
					continue;
				if (v->get_value() <= tc->get_value()){
					add_tag(cluster_tag, v);
					more_spread = true;
				}
			}
		}


	}



	// While loop to:
	//	find neighbors that qualify for spread
	return true;
}

Cell* ModifiedAggregation::find_maximum_untagged(){
	// check for tagged
	
	auto max_it = get_untagged_cells()->begin();
	if (max_it == get_untagged_cells()->end())
		return nullptr;
	Cell* max = *max_it;
	for (const auto& v : *get_untagged_cells()){
		if (is_tagged(v)){
			continue;
		}
		if (v->get_value() < seed_threshold)
			continue;
		if (v->get_value() > max->get_value()){
			max = v;
		}
	}

	// CASE1: No other max and first is untagged: return true.
	// CASE2: No other max and first is tagged: return false.
	// CAse3: Other max found and has been checked for untagged. Return true.

	if (is_tagged(max))
		return nullptr;
	if (max->get_value() < seed_threshold)
		return nullptr;
	return max;
}
