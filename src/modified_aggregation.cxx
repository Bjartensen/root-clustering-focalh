#include "modified_aggregation.h"

bool ModifiedAggregation::tag(){
	Cell* c = nullptr;
	bool clustered = false;

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
			clustered = true;
		}
	}
	return clustered;
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
          // According to the paper, if this is false (encountering greater cell)
          // it might stop spreading altogether if this is encountered.
          // Don't know if that is correctly interpreted.
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



std::string ModifiedAggregation::name(){
	const char DELIM = '_';
	std::string algo = "ma";
	std::string pars = "";
	pars += std::to_string(int(seed_threshold));
	pars += DELIM;
	pars += std::to_string(int(aggregation_threshold));
	return algo+DELIM+pars;
}
