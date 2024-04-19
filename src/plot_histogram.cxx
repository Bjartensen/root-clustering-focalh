#include "plot_histogram.h"


#include <iostream>

void PlotHistogram::create_tcanvas(std::string name, std::string title){
  canvas = std::make_unique<TCanvas>("histogram", "histogram", 1);
}


void PlotHistogram::create_histogram(std::vector<General::float_type> data, std::string name, std::string title){

  std::unique_ptr<TH1D> hist = std::make_unique<TH1D>(name.c_str(), title.c_str(), bins, 0, x_max);

  for (const auto &v : data) hist->Fill(v);
  histograms.push_back(std::move(hist));
}

void PlotHistogram::draw_hists(){
  update_height();
  auto_set_graphics();
  for (auto &v : histograms){
    v->Draw("same");
    v.release();
  }
}

void PlotHistogram::save_to_file(std::string filename){
  canvas->cd();
  draw_hists();

  std::string full_filename = Folders::AnalysisFolder; // needs something from definitions.h
  full_filename += "/";
  full_filename += filename;
  full_filename += Plotting::ImageExtension;
  canvas->Update();
  canvas->SaveAs(full_filename.c_str());
}


int PlotHistogram::find_height(){
  int height = 0;
  for (const auto &v : histograms){
    // If zero bin content is largest, find maximum smaller than it
    int zero_bin_max = v->GetBinContent(1);
    if (v->GetMaximumBin() == 1){
      if (v->GetMaximum(zero_bin_max) > height) height = v->GetMaximum(zero_bin_max);
    }else{
      if (v->GetMaximum() > height) height = v->GetMaximum();
    }
  }
  return height;
}


void PlotHistogram::update_height(){
  for (auto &v : histograms) v->SetMaximum(find_height());
}

void PlotHistogram::auto_set_graphics(){
  for (int i = 0; i < histograms.size(); i++){
    // Set colors
    histograms.at(i)->SetFillColorAlpha(i+1, 0.7);
    histograms.at(i)->SetFillStyle(3145);
    histograms.at(i)->SetLineColor(1);

    
    histograms.at(i)->SetStats(0);
    histograms.at(i)->GetXaxis()->SetTitle(x_axis_label.c_str());
    histograms.at(i)->GetYaxis()->SetTitle(y_axis_label.c_str());
  }
}
