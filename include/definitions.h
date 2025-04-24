#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>
#include <vector>


namespace Plotting{
  const std::string ImageExtension = ".pdf";
}

namespace General{
	const std::string RootFileExtension = ".root";
	using float_type = double;
	using adc_type = unsigned int;
	using energy_type = int;

	const std::string energy_tparameter = "energy";
	const std::string source_tobj = "source";
	const std::string description_tobj = "description";
	struct EventsHeader{
		energy_type Energy = 0;
		std::string Source = "";
		std::string Description = "";
	};

  const std::string EnergyUnit = "GeV";
}

namespace Folders{
	const std::string DataFolder = "../data";
	const std::string ClusteredFolder = "../clustered";
	const std::string AnalysisFolder = "../analysis";
}

// Input TTree from focalsim or testbeam
namespace TTreeData{
	const std::string TreeName = "EventsTree";
	const std::string Energy = "Energy";
}


namespace TFileGeneric{
  const std::string TreeName = "EventsTree";
  const std::string TreeTitle = "Events Tree";
  const std::string EnergyName = "Energy";
  const std::string FileNameAppend = "_generic" + General::RootFileExtension;

  // Under a TreeName
  const std::string x_branch = "x";
  const std::string y_branch = "y";
  const std::string value_branch = "value";
  const std::string label_branch = "labels";
  const std::string label_idx_branch = "label_indices";
  const std::string fractions_branch = "fractions";
  const std::string energies_branch = "energies";
  const std::string clusters_branch = "clusters";
  const std::string cluster_idx_branch = "cluster_indices";

  struct EventPtr{
    std::vector<General::float_type> *x = nullptr;
    std::vector<General::float_type> *y = nullptr;
    std::vector<General::float_type> *value = nullptr;
    std::vector<General::float_type> *energies = nullptr;

    // Per cell flattened
    std::vector<unsigned int> *labels = nullptr; // Flattened
    std::vector<unsigned int> *label_idx = nullptr; // Index offsets
    std::vector<General::float_type> *fractions = nullptr; // Flattened

    std::vector<unsigned int> *clusters = nullptr;
    std::vector<unsigned int> *cluster_idx = nullptr;
  };
}


// Clustered events TTree
namespace TTreeClustered{
	// Names and strings
	const std::string TreeName = "clustered_events";
	const std::string TreeTitle = "Clustered Events";
	const std::string x_branch = "x";
	const std::string y_branch = "y";
	const std::string value_branch = "value";
	const std::string class_branch = "labels";
	const std::string cluster_branch = "cluster";

	// Types
	using x_type = std::vector<General::float_type>;
	using y_type = std::vector<General::float_type>;
	using value_type = std::vector<unsigned int>;
	using class_type = std::vector<unsigned int>;
	using cluster_type = std::vector<std::string>;

	struct Event{
		x_type x;
		y_type y;
		value_type value;
		class_type class_label;
		cluster_type cluster;
	};

	struct EventPtr{
		x_type* x = nullptr;
		y_type* y = nullptr;
		value_type* value = nullptr;
		class_type* class_label = nullptr;
		cluster_type* cluster = nullptr;
	};
}

namespace TFileClustered{
	// Names and strings
	const std::string method = "method";
	const std::string parameters = "parameters";

	// Types
	using parameters_type = std::pair<std::string, std::string>;
}

// Analysis TTree
namespace TTreeAnalysis{
	
}



#endif // DEFINITIONS_H
