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

	wxBitmap GetWxBitmap();
	int GetW();
	int GetH();

	void applyGreyTransform();
	void applyHorTransform();
	void applyVerTransform();
	void applyQuantTranform(int n_tones);
};

#endif /* WXFPI_IMAGE_H */