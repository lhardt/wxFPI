#include "image.h"

#include <bitset>

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

Image::Image(const Image& other) : matrix(other.matrix.size(), other.matrix.type()), w(other.w), h(other.h) {
	other.matrix.copyTo(matrix);
	makeWxView();
}

void Image::SaveAs(std::string filename) {
	cv::imwrite(filename, this->matrix);
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

void Image::applyInvertTransform() {
	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);

			p[0] = 255 - p[0];
			p[1] = 255 - p[1];
			p[2] = 255 - p[2];
		}
	}
	makeWxView();
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

void Image::applyQuantTranform(int n_bins) {
	std::bitset<256> has_shade;
	has_shade.reset();
	int n_shades = 0;

	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);
			
			if (!has_shade[p[0]]) {
				has_shade[p[0]] = true;
				n_shades++;
			}
		}
	}	

	wxLogInfo("quant: The current image has %d shades!", n_shades);
	wxLogInfo("quant: and we will compress it to %d !", n_bins);
	// This prevents redundant work and edge case high==low.
	if (n_shades <= n_bins) {
		wxLogInfo("quant: Skipping! Less shades than bins. ", n_bins);
		makeWxView();
		return;
	}

	int low = 0, high = 255; 
	while (!has_shade[low] && low < 255) ++low;
	while (!has_shade[high] && high > 0) --high;

	int bin_size = (high - low + 1) / n_bins;
	double bin_size_d= (high - low + 1) / (double)n_bins;
	wxLogInfo("quant: bin size will be %lf !", bin_size_d);


	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);

			int pv = p[0];
			// pv in [low .. low + bin_size - 1] goes to bin 0 
			int pbin = (pv - low)/ bin_size;

			//                     int new_lum = ((l_low-1)+bin_size*(2*bin+1))/2;

			double pintensity = low + pbin * bin_size_d;
			if (pintensity > 255) pintensity = 255;

			p[0] = p[1] = p[2] = (int) pintensity;
		}
	}
	makeWxView();
}

double get_luminance(int r, int g, int b) {
	// NTSC formula for pixel intensity;
	return 0.299 * r + 0.587 * g + 0.114 * b;
}

//wxBitmap * get