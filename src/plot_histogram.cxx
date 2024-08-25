#include "plot_histogram.h"


#include <iostream>

void PlotHistogram::create_tcanvas(std::string name, std::string title){
  canvas = std::make_unique<TCanvas>("histogram", "histogram", 5);
  canvas->SetTickx();
  canvas->SetTicky();
  canvas->SetLeftMargin(0.13); // Must be adjusted depending on y-axis label
  canvas->SetRightMargin(0.1);
}


void PlotHistogram::create_histogram(std::vector<General::float_type> data, std::string name, std::string title){

  std::unique_ptr<TH1D> hist = std::make_unique<TH1D>(name.c_str(), title.c_str(), bins, 0, x_max);

  for (const auto &v : data) hist->Fill(v);
  histograms.push_back(std::move(hist));
}

void PlotHistogram::draw_hists(){
  canvas->cd();
  update_height();
  auto_set_graphics();
  for (auto &v : histograms){
    v->Draw("same");
  }
  make_tlegend();
  for (auto &v : histograms) v.release();

}

void PlotHistogram::save_to_file(std::string filename){
  canvas->cd();
  draw_hists();
  make_header();

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


void PlotHistogram::make_tlegend(){
  //TLegend *leg = canvas->BuildLegend();
  //leg->SetHeader("test", "C");


  std::unique_ptr<TLegend> leg;
  leg = std::make_unique<TLegend>(0.5, 0.6, 0.88, 0.88);

  leg->SetHeader(tlegend_title.c_str(), "C");
  leg->SetBorderSize(0);

  leg->SetNColumns(3);

  for (auto &v : histograms){
    leg->AddEntry(v.get(), v->GetName(), "f");
  }


  //leg->AddEntry(histograms.at(0).get(), "test", "l");
  leg->Draw();
  leg.release();

}


void PlotHistogram::make_header(){

  canvas->cd();
  // Top left
  std::unique_ptr<TPaveText> top_left;
  top_left = std::make_unique<TPaveText>(0.14,0.91,0.5,0.95,"NDC");
  top_left->AddText(canvas_header_left.c_str());
  top_left->SetBorderSize(0);
  top_left->SetFillColor(0);
  top_left->SetMargin(0);
  top_left->SetTextAlign(11);

  // Top right
  std::unique_ptr<TPaveText> top_right;
  top_right = std::make_unique<TPaveText>(0.5,0.91,0.88,0.95,"NDC");
  top_right->AddText(canvas_header_right.c_str());
  top_right->SetBorderSize(0);
  top_right->SetFillColor(0);
  top_right->SetMargin(0);
  top_right->SetTextAlign(31);

  top_left->Draw();
  top_left.release();
  top_right->Draw();
  top_right.release();
}


