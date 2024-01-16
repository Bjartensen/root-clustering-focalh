#include "ext_libraries.h"
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>


int main(int argc, char* argv[]){

	//const std::string focalsim_analysis_dir = "../data/focalsim_analysis_files/";
	const std::string focalsim_digits_dir = "../data/focalsim_digits/";

	const std::string tree_name = "T";


	std::string filename;
	std::string outfilename;

	if (argc == 3){
		filename = std::string(argv[1]);
		outfilename = std::string(argv[2]);
	}else{
		std::cout << "Expects two file names." << std::endl;
		return 0;
	}




	//const std::string focalsim_analysis_filename = "";

	std::cout << "Extract digits from focalsim file " << filename << " into " << outfilename << std::endl;
	//std::string out_file_path = focalsim_digits_dir + outfilename;


	std::unique_ptr<TFile> analysis_file = std::make_unique<TFile>(filename.c_str(), "READ");
	auto tree = static_cast<TTree*>(analysis_file->Get(tree_name.c_str()));
	int energy = static_cast<TParameter<int>*>(analysis_file->Get("energy"))->GetVal();
	TParameter<int> energy_par("energy", energy);

	std::unique_ptr<TFile> out_file = std::make_unique<TFile>(outfilename.c_str(), "RECREATE");
	out_file->cd();
	TTree* out_tree = tree->CloneTree();
	out_tree->Write();
	energy_par.Write();
	analysis_file->Close();
	out_file->Write();
	out_file->Close();




	//

	return 0;
}
