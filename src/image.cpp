#include "image.h"

Image::Image(std::string filename)  {
	auto img = cv::imread(filename, cv::IMREAD_COLOR);
	if (img.empty()) {
		wxLogError("Could not open image!");
		return;
	}

	this->matrix = img;
	cv::Size sz = matrix.size();
	this->w = sz.width;
	this->h = sz.height;
	makeWxView();
}

void Image::makeWxView() {
	cv::cvtColor(matrix, matrix, cv::COLOR_RGB2BGR);
	wxImage Image(w, h, matrix.data, TRUE);
	bitmap = wxBitmap(Image, -1);
}

int Image::GetH() {
	return h;
}

int Image::GetW() {
	return w;
}

wxBitmap Image::GetWxBitmap() {
	return bitmap;
}

//wxBitmap * get