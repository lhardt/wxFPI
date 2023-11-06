#ifndef WXFPI_IMAGE_H
#define WXFPI_IMAGE_H

#include <string>
#include "wx.h"
#include <opencv2/opencv.hpp>


class Image {
private:
	cv::Mat matrix;
	wxBitmap bitmap;
	int w, h;

	void makeWxView();
public:
	Image(std::string filename);
	Image(const Image& other);

	void SaveAs(std::string filename);

	wxBitmap GetWxBitmap();
	int GetW();
	int GetH();

	void applyInvertTransform();
	void applyGreyTransform();
	void applyHorTransform();
	void applyVerTransform();
	void applyQuantTranform(int n_tones);
};


double get_luminance(int r, int g, int b);

#endif /* WXFPI_IMAGE_H */