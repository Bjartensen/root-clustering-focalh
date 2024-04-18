#ifndef PLOT_GAUSSIAN_H
#define PLOT_GAUSSIAN_H

#include <TCanvas.h>
#include <TH1D.h>
#include <vector>
#include <memory>

class PlotGaussian{
private:
	std::vector<std::unique_ptr<TH1D>> gaussians;
	std::unique_ptr<TCanvas> canvas;
	
public:
	/*
	create_tcanvas();
	create_gaussian();
	add_gaussian();
	get_tcanvas();

	set_height(TH1D &h);
	set_...
	...

	auto_height(TH1D &h);
	auto_...
	...

	set_height_all();
	set_...
	...
	*/
	
};

#endif // PLOT_GAUSSIAN_H
