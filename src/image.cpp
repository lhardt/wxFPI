#include "image.h"

#include <bitset>

Image::Image(int h, int w) : matrix(h, w, CV_8UC3, cv::Scalar(255,255,255)){
	cv::Size sz = matrix.size();
	this->w = sz.width;
	this->h = sz.height;
	makeWxView();
}

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

double Kernel::applyToPixelChannel(const Kernel& kernel,
	uchar p00, uchar p01, uchar p02,
	uchar p10, uchar p11, uchar p12,
	uchar p20, uchar p21, uchar p22) const {

	auto& w = this->weights;

	double result = shift
		+ w[0][0] * p00 + w[0][1] * p01 + w[0][2] * p02
		+ w[1][0] * p10 + w[1][1] * p11 + w[1][2] * p12
		+ w[2][0] * p20 + w[2][1] * p21 + w[2][2] * p22;
	
	if(result > 255) result = 255;
	if (result < 0) result = 0;
	return result;
}

cv::Vec3b Kernel::applyToPixel(const Kernel& kernel,
	const cv::Vec3b& p00, const cv::Vec3b& p01, const cv::Vec3b& p02,
	const cv::Vec3b& p10, const cv::Vec3b& p11, const cv::Vec3b& p12,
	const cv::Vec3b& p20, const cv::Vec3b& p21, const cv::Vec3b& p22) const {
	cv::Vec3b out; 

	for (int ch = 0; ch < 3; ++ch) {
		out[ch] = applyToPixelChannel(kernel, 
			p00[ch], p01[ch], p02[ch], 
			p10[ch], p11[ch], p12[ch], 
			p20[ch], p21[ch], p22[ch]);
	}
	return out;
}


void Image::applyKernel(const Kernel& kernel) {
	cv::Mat temp = this->matrix.clone();

	/* Top line */
	for (int i = 1; i < matrix.cols - 1; ++i) {
		int x = i, y = 0;
		cv::Vec3b& p = temp.at<cv::Vec3b>(y, x);
		p = kernel.applyToPixel(kernel,
			at(y, x - 1), at(y, x), at(y, x + 1),
			at(y, x - 1), at(y, x), at(y, x + 1),
			at(y + 1, x - 1), at(y + 1, x), at(y + 1, x + 1));
	}
	/* Bottom line */
	for (int i = 1; i < matrix.cols - 1; ++i) {
		int x = i, y = matrix.rows - 1;
		cv::Vec3b& p = temp.at<cv::Vec3b>(y, x);
		p = kernel.applyToPixel(kernel,
			at(y - 1, x - 1), at(y - 1, x), at(y - 1, x + 1),
			at(y, x - 1), at(y, x), at(y, x + 1),
			at(y, x - 1), at(y, x), at(y, x + 1));
	}

	/* Left */
	for (int i = 1; i < matrix.rows - 1; ++i) {
		int x = 0, y = i;
		cv::Vec3b& p = temp.at<cv::Vec3b>(y, x);

		p = kernel.applyToPixel(kernel,
			at(y - 1, x), at(y - 1, x), at(y - 1, x + 1),
			at(y, x), at(y, x), at(y, x + 1),
			at(y + 1, x), at(y + 1, x), at(y + 1, x + 1)
		);
	}
	/* Right */
	for (int i = 1; i < matrix.rows - 1; ++i) {
		int x = matrix.cols-1, y = i;
		cv::Vec3b& p = temp.at<cv::Vec3b>(y, x);

		p = kernel.applyToPixel(kernel,
			at(y - 1, x - 1), at(y - 1, x), at(y - 1, x),
			at(y, x - 1), at(y, x), at(y, x),
			at(y + 1, x - 1), at(y + 1, x), at(y + 1, x)
		);
	}

	/* Center of Image */
	for (int x = 1; x < matrix.cols - 1; ++x) {
		for (int y = 1; y < matrix.rows - 1; ++y) {
			cv::Vec3b& p = temp.at<cv::Vec3b>(y, x);

			p = kernel.applyToPixel(kernel,
				at(y - 1, x - 1), at(y - 1, x), at(y - 1, x + 1),
				at(y, x - 1), at(y, x), at(y, x + 1),
				at(y + 1, x - 1), at(y + 1, x), at(y + 1, x + 1)
			);
		}
	}
	this->matrix = temp;

	makeWxView();
}

cv::Vec3b& Image::at(int row, int col) {
	return matrix.at<cv::Vec3b>(row, col);
}

double get_luminance(int r, int g, int b) {
	// NTSC formula for pixel intensity;
	return 0.299 * r + 0.587 * g + 0.114 * b;
}
