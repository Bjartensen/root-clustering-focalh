#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>


namespace Folders{
	const std::string DataFolder = "../data";
	const std::string ClusteredFolder = "../clustered";
	const std::string AnalysisFolder = "../analysis";
}

// Input TTree from focalsim or testbeam
namespace TTreeData{
	const std::string TreeName = "T";
	
}

// Clustered events TTree
namespace TTreeClustered{
	// Names and strings
	const std::string TreeName = "clustered_events";
	const std::string x_branch = "x_coordinate";
	const std::string y_branch = "y_coordinate";
	const std::string value_branch = "value";
	const std::string class_branch = "class";
	const std::string cluster_branch = "cluster";

	// Types
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
