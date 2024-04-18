# * See https://root.cern/manual/integrate_root_into_my_cmake_project/ for more info
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
project(clustering_util)

find_package(ROOT REQUIRED COMPONENTS 
    Core 
	Tree
    Hist
    Gpad
    Graf3d
    Thread
    RIO
    RooFitCore
    RooFit
    Graf
    # for TTreadData
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(clustering_util SHARED 
	#src/extract_digits.cxx
	#src/easylogging++.cc
	include/ext_libraries.h
	src/cell.cxx
	src/grid.cxx
	src/clustering.cxx
	src/modified_aggregation.cxx
	src/focal_coordinates.cxx
	src/geometry.cxx
	src/focalh.cxx
	src/cluster_events.cxx
	src/cluster_writer.cxx
	src/cluster_reader.cxx
	src/cluster_analysis.cxx
	src/histogram_analysis.cxx
	src/plot_gaussian.cxx
	include/cell.h
	include/grid.h
	include/clustering.h
	include/modified_aggregation.h
	include/focal_coordinates.h
	include/geometry.h
	include/focalh.h
	include/cluster_events.h
	include/cluster_writer.h
	include/cluster_reader.h
	include/cluster_analysis.h
	include/histogram_analysis.h
	include/plot_gaussian.h
	include/definitions.h
)

target_link_libraries(clustering_util PUBLIC 
    ROOT::Core
    ROOT::Tree
    ROOT::Hist
    ROOT::Gpad
    ROOT::Graf3d
    ROOT::RooFitCore
    ROOT::RooFit
    # for TFile
    ROOT::RIO
    ROOT::Thread
    # for TScatter
    ROOT::Graf
)

# * Change script name here to run different scripts
add_executable(extract_digits_main                  ./scripts/extract_digits.cxx)
add_executable(hist									./scripts/hist.cxx)
add_executable(cell_main									./scripts/cell_main.cxx)
add_executable(testbeam_reconstruct									./scripts/testbeam_reconstruct.cxx)

target_link_libraries(extract_digits_main                     clustering_util)
target_link_libraries(hist                     clustering_util)
target_link_libraries(cell_main                     clustering_util)
target_link_libraries(testbeam_reconstruct                     clustering_util)

#ROOT_GENERATE_DICTIONARY(myapp_dict myapp.h MODULE myapp LINKDEF LinkDef.h)
