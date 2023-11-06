#include "image.h"

Image::Image(std::string filename)  {
	auto img = cv::imread(filename, cv::IMREAD_COLOR);
	if (img.empty()) {
		w = 0, h = 0;
		wxLogError("Could not open image!");
		return;
	}

	this->matrix = img;
	cv::Size sz = matrix.size();
	this->w = sz.width;
	this->h = sz.height;
	makeWxView();
}

Image::Image(const Image& other) : matrix(other.matrix), w(other.w), h(other.h) {
	makeWxView();
}

void Image::makeWxView() {
	cv::Mat matrix_bgr;
	cv::cvtColor(matrix, matrix_bgr, cv::COLOR_RGB2BGR);
	wxImage Image(w, h, matrix_bgr.data, TRUE);
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


void Image::applyGreyTransform() {
	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {

			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);
			
			double lum = get_luminance(p[0], p[1], p[2]);

			p[0] = p[1] = p[2] = (uchar) lum;
		}
	}
	makeWxView();
}

void Image::applyHorTransform() {
	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; 2 * i_col < matrix.cols; i_col++) {
			int j_col = matrix.cols - 1 - i_col;
			
			cv::Vec3b &p1 = matrix.at<cv::Vec3b>(i_row, i_col);
			cv::Vec3b &p2 = matrix.at<cv::Vec3b>(i_row, j_col);

			swap(p1, p2);
		}
	}
	makeWxView();
}

void Image::applyVerTransform() {
	for (int i_row = 0; 2 * i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			int j_row = matrix.rows - 1 - i_row;

			cv::Vec3b& p1 = matrix.at<cv::Vec3b>(i_row, i_col);
			cv::Vec3b& p2 = matrix.at<cv::Vec3b>(j_row, i_col);

			swap(p1, p2);
		}
	}
	makeWxView();
}

void Image::applyQuantTranform(int n_quant) {

	makeWxView();
}

double get_luminance(int r, int g, int b) {
	// NTSC formula for pixel intensity;
	return 0.299 * r + 0.587 * g + 0.114 * b;
}

//wxBitmap * get