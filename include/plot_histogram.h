#ifndef PLOT_HISTOGRAM_H
#define PLOT_HISTOGRAM_H

#include <TCanvas.h>
#include <TH1D.h>
#include <vector>
#include <memory>
#include "definitions.h"

class PlotHistogram{
private:
	std::vector<std::unique_ptr<TH1D>> histograms;
	std::unique_ptr<TCanvas> canvas;

  std::string x_axis_label = "";
  std::string y_axis_label = "";

  int bins = 0;
  double x_max = 0;
	
public:
	void create_tcanvas(std::string name, std::string title);
	void create_histogram(std::vector<General::float_type> data, std::string name, std::string title);
	void add_histogram();

  void draw_hists();
  void save_to_file(std::string filename);

  void auto_set_graphics();

  void set_x_axis_label(std::string _x_axis_label){x_axis_label = _x_axis_label;}
  void set_y_axis_label(std::string _y_axis_label){y_axis_label = _y_axis_label;}

  void set_x_max(double _x_max){x_max = _x_max;}
  void set_bins(int _bins){bins = _bins;}

  int find_height();
  void update_height();
  
  double get_x_max(){return x_max;}
  int get_bins(){return bins;}
	
};

#endif // PLOT_HISTOGRAM_H
