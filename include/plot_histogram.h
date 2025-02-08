#ifndef PLOT_HISTOGRAM_H
#define PLOT_HISTOGRAM_H

#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TLatex.h>
#include <TH1D.h>
#include <TF1.h>
#include <vector>
#include <memory>
#include "definitions.h"

class PlotHistogram{
private:
	std::vector<std::unique_ptr<TH1D>> histograms;
	std::unique_ptr<TCanvas> canvas;
  std::unique_ptr<TF1> fits;

  std::string x_axis_label = "";
  std::string y_axis_label = "";

  std::string tlegend_title = "";
  std::string canvas_header_left = "";
  std::string canvas_header_right = "";

  int bins = 0;
  double x_max = 0;
	
public:
	void create_tcanvas(std::string name, std::string title);
	void create_histogram(std::vector<General::float_type> data, std::string name, std::string title);
	void add_histogram();

  void draw_hists();
  void save_to_file(std::string filename);


  // Do fits
  // std::unique_ptr<TF1> fit = std::make_unique<TF1>("fit", "gaus");
  //fit->SetParameter(0, hist->GetMaximum());
  //fit->SetParameter(1, hist->GetMean());
  //fit->SetParameter(2, hist->GetRMS());
  //hist->Draw("same");
  //hist->Fit("fit", "0");

  void auto_set_graphics();
  void make_tlegend();
  void set_tlegend_title(std::string _title){tlegend_title = _title;}
  void make_header(); // Eventually move to separate class


  void set_x_axis_label(std::string _x_axis_label){x_axis_label = _x_axis_label;}
  void set_y_axis_label(std::string _y_axis_label){y_axis_label = _y_axis_label;}
  void set_canvas_header_left(std::string _left){canvas_header_left = _left;}
  void set_canvas_header_right(std::string _right){canvas_header_right = _right;}
  void set_x_max(double _x_max){x_max = _x_max;}
  void set_bins(int _bins){bins = _bins;}

  int find_height();
  void update_height();

  double get_x_max(){return x_max;}
  int get_bins(){return bins;}
	
};

#endif // PLOT_HISTOGRAM_H
