#include "focal_coordinates.h"
#include <iostream>
#include <TFile.h>
#include <memory>
#include <TTree.h>
#include <TParameter.h>


int main(int argc, char* argv[]){

	const unsigned short number_of_boards = 4;
	const unsigned long MAX_ENTRIES = 25*number_of_boards;

	unsigned int energy;

	std::string coordinates = "module_row_col_coordinates.csv";
	std::string channel_mapping = "board_channel_module_row_col_mapping.csv";

	std::string shihai_frames_file;

	if (argc == 3){
		shihai_frames_file = argv[1];
		energy = std::stoi(argv[2]);
	}
	else{
		std::cout << "ERROR: Specify frames root file and energy." << std::endl;
		return 0;
	}

	std::cout << "Attempting to reconstruct events from file: " << shihai_frames_file << std::endl;
	std::cout << "Specified energy: " << energy << std::endl;


	FocalCoordinates f(coordinates, channel_mapping);
	//f.generate_module_coordinates(file);


	std::unique_ptr<TFile> frames_file = std::make_unique<TFile>(shihai_frames_file.c_str(), "READ");


	auto frames_tree = static_cast<TTree*>(frames_file->Get("frames"));
	
    auto n_entries = frames_tree->GetEntries();


	

    Short_t _board_num;
    Double_t _timestamp;
    Int_t _trigID;
    std::vector<Short_t> *_CH_ptr = nullptr;
    std::vector<Short_t> *_HG_charge_ptr = nullptr;
    std::vector<Short_t> *_LG_charge_ptr = nullptr;
    std::vector<Short_t> *_TS_ptr = nullptr;
    std::vector<Short_t> *_ToT_ptr = nullptr;


    frames_tree->SetBranchAddress("board_num", &_board_num);
    frames_tree->SetBranchAddress("trigID", &_trigID);
    frames_tree->SetBranchAddress("CH",        &_CH_ptr);
    frames_tree->SetBranchAddress("HG_charge", &_HG_charge_ptr);
    frames_tree->SetBranchAddress("LG_charge", &_LG_charge_ptr);
	

	std::vector<std::vector<std::tuple<unsigned short, unsigned short, unsigned short>>> merged_frames;
	std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> temp_vec;

	Int_t temp_trigID = -1;
    for (auto i = 0; i < n_entries; i++){
		if (i > MAX_ENTRIES)
			break;
		frames_tree->GetEntry(i);
		if (temp_trigID != _trigID){
			if (temp_trigID != -1)
				merged_frames.push_back(temp_vec);
			temp_vec.clear();
		}

		for (int j = 0; j < _CH_ptr->size(); j++){
			temp_vec.push_back(std::make_tuple(_board_num, _CH_ptr->at(j), _HG_charge_ptr->at(j)));
		}

		
		temp_trigID = _trigID;
	}




	TParameter<int> a("energy", energy);
	std::string digit_file_name = std::to_string(energy) + "_" + std::to_string(MAX_ENTRIES) + "_tb.root";
	std::unique_ptr<TFile> digit_file = std::make_unique<TFile>(digit_file_name.c_str(), "RECREATE");
	std::unique_ptr<TTree> digit_tree = std::make_unique<TTree>("T", "digits");


	std::vector<double> x_pos;
	std::vector<double> y_pos;
	std::vector<double> value;
	digit_tree->Branch("x_pos", &x_pos);
	digit_tree->Branch("y_pos", &y_pos);
	digit_tree->Branch("value", &value);
	

	a.Write();


	std::cout << "Events: " << merged_frames.size() << std::endl;

	for (int m = 0; m < merged_frames.size(); m++){
		// for each merged frame
		x_pos.clear();
		y_pos.clear();
		value.clear();

		double temp_x;
		double temp_y;
		for (int i = 0; i < merged_frames.at(m).size(); i++){
			auto temp_board = std::get<0>(merged_frames.at(m).at(i));
			auto temp_channel = std::get<1>(merged_frames.at(m).at(i));

			if(f.board_channel_coordinates(temp_board, temp_channel, temp_x, temp_y)){
				x_pos.push_back(temp_x);
				y_pos.push_back(temp_y);
				value.push_back(std::get<2>(merged_frames.at(m).at(i)));
			}

		}
		digit_tree->Fill();
	}

	digit_tree->Write();



	return 0;
}
