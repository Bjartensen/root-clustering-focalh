#include "ext_libraries.h"
#include <TFile.h>
#include <TTree.h>


int main(int argc, char* argv[]){

	const std::string focalsim_analysis_dir = "../data/focalsim_analysis_files/";
	const std::string focalsim_digits_dir = "../data/focalsim_digits/";

	const std::string tree_name = "T";


	const char* filename;

	if (argc == 2)
		filename = argv[1];

	std::cout << "Extract digits from focalsim file " << filename << std::endl;
	std::string out_file_path = focalsim_digits_dir + "adsf2.root";


	std::unique_ptr<TFile> analysis_file = std::make_unique<TFile>(filename, "READ");
	auto tree = static_cast<TTree*>(analysis_file->Get(tree_name.c_str()));


	std::unique_ptr<TFile> out_file = std::make_unique<TFile>(out_file_path.c_str(), "RECREATE");
	TTree* out_tree = tree->CloneTree();
	analysis_file->Close();
	out_file->Write();
	out_file->Close();




	//

	return 0;
}
