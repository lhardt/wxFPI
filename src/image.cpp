#include "image.h"

#include <bitset>

/*********************************************************/
/*                    INITIALIZATION                     */
/*********************************************************/

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

int Image::GetH() const {
	return h;
}

int Image::GetW() const {
	return w;
}

wxBitmap Image::GetWxBitmap() const {
	return bitmap;
}

cv::Mat& Image::GetMatrix() {
	return matrix;
}

/*********************************************************/
/*                         UTIL                          */
/*********************************************************/

double get_luminance(int r, int g, int b) {
	// NTSC formula for pixel intensity;
	double ans = 0.299 * r + 0.587 * g + 0.114 * b;
	if (ans < 0) ans = 0;
	if (ans > 255) ans = 255;
	return ans;
}

uint8_t trunc_pixel(double pixel_value) {
	if (pixel_value > 255) pixel_value = 255;
	if (pixel_value < 0) pixel_value = 0;

	return (uint8_t)(pixel_value + 0.5);
}

cv::Vec3b avg_pixel(cv::Vec3b p1, cv::Vec3b p2, cv::Vec3b p3, cv::Vec3b p4) {
	cv::Vec3b result;

	result[0] = trunc_pixel(((double)p1[0] + (double)p2[0] + (double)p3[0] + (double)p4[0]) / 4);
	result[1] = trunc_pixel(((double)p1[1] + (double)p2[1] + (double)p3[1] + (double)p4[1]) / 4);
	result[2] = trunc_pixel(((double)p1[2] + (double)p2[2] + (double)p3[2] + (double)p4[2]) / 4);
	
	return result;
}

cv::Vec3b avg_pixel(cv::Vec3b p1, cv::Vec3b p2) {
	return avg_pixel(p1, p1, p2, p2);
}

cv::Vec3b change_luminance(cv::Vec3b p, double new_luminance) {
	cv::Vec3b new_p;
	// TODO: convert to LAB, change L value, and change back;
	double lum = get_luminance(p[0], p[1], p[2]);
	new_p[0] = trunc_pixel((double)p[0] * new_luminance / lum);
	new_p[1] = trunc_pixel((double)p[1] * new_luminance / lum);
	new_p[2] = trunc_pixel((double)p[2] * new_luminance / lum );
	return new_p;
}

/*********************************************************/
/*                   COLOR TRANSFORM.                    */
/*********************************************************/

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

	int bin_size = (high - low + 1LL) / n_bins;
	double bin_size_d = (high - low + 1) / (double)n_bins;
	wxLogInfo("quant: bin size will be %lf !", bin_size_d);

	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);

			int pv = p[0];
			// pv in [low .. low + bin_size - 1] goes to bin 0 
			int pbin = (pv - low) / bin_size;

			//                     int new_lum = ((l_low-1)+bin_size*(2*bin+1))/2;

			double pintensity = low + pbin * bin_size_d;
			if (pintensity > 255) pintensity = 255;

			p[0] = p[1] = p[2] = (int)pintensity;
		}
	}
	makeWxView();
}

void Image::applyBrightnessEnh(double  value){
	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);
			
			p[0] = trunc_pixel((int)p[0] + value);
			p[1] = trunc_pixel((int)p[1] + value);
			p[2] = trunc_pixel((int)p[2] + value);
		}
	}
	makeWxView();
}

void Image::applyContrastEnh(double value){
	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);

			p[0] = trunc_pixel((int)p[0] * value);
			p[1] = trunc_pixel((int)p[1] * value);
			p[2] = trunc_pixel((int)p[2] * value);
		}
	}
	makeWxView();
}

void Image::applyEqualize() {
	Histogram hist(*this);
	std::vector<int> cum_hist = hist.GetCumulative();

	for (int i_row = 0; i_row < matrix.rows; i_row++) {
		for (int i_col = 0; i_col < matrix.cols; i_col++) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(i_row, i_col);

			int p_lum = trunc_pixel(get_luminance(p[0], p[1], p[2]));
			double p_new_lum = cum_hist[p_lum] * 255.0 / cum_hist[255];

			// TODO: change to LAB space and equalize with colors
			p[0] = trunc_pixel(p[0] * p_new_lum / p_lum);
			p[1] = trunc_pixel(p[1] * p_new_lum / p_lum);
			p[2] = trunc_pixel(p[2] * p_new_lum / p_lum);
		}
	}
	makeWxView();
}

void Image::applyHistogram(Histogram& new_hist) {
	Histogram cur_hist(*this);
	
	std::vector<int> cur_cum = cur_hist.GetCumulative();
	std::vector<int> new_cum = new_hist.GetCumulative();

	std::vector<int> new_shade(256, 0);

	int i_new = 0;
	// Try to match i_cur with some i_new
	for (int i_cur = 0; i_cur < 256; ++i_cur) {
		// Find closest new tone;
		while (i_new < 255) {
			int curr_dist = std::abs(cur_cum[i_cur] - new_cum[i_new]);
			int next_dist = std::abs(cur_cum[i_cur] - new_cum[i_new + 1]);
			if (next_dist < curr_dist) {
				++i_new;
			} else break;
		}
		new_shade[i_cur] = i_new;
	}

	for (int r = 0; r < h; ++r) {
		for (int c = 0; c < w; ++c) {
			cv::Vec3b& p = at(r, c);
			int lum = trunc_pixel(get_luminance(p[0], p[1], p[2]));
			p = change_luminance(p, new_shade[ lum ]);
		}
	}
	makeWxView();
}

/*********************************************************/
/*                   IMAGE TRANSFORM.                    */
/*********************************************************/

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

void Image::applyRotLeftTranform() {
	cv::Mat new_image(cv::Size(matrix.rows, matrix.cols), CV_8UC3);

	for (int i_row = 0; i_row < matrix.rows; ++i_row) {
		for (int i_col = 0; i_col < matrix.cols; ++i_col) {
			cv::Vec3b& p = new_image.at<cv::Vec3b>(matrix.cols - 1 - i_col, i_row); //(i_col, /**/ i_row);
			p = at(i_row, i_col);
		}
	}

	wxLogInfo("new image has rows %d cols %d", new_image.rows, new_image.cols);

	this->matrix = new_image;
	int tmp = h; 
	h = w; w = tmp;

	makeWxView();
}

void Image::applyRotRightTranform() {
	cv::Mat new_image(cv::Size(matrix.rows, matrix.cols), CV_8UC3);

	for (int i_row = 0; i_row < matrix.rows; ++i_row) {
		for (int i_col = 0; i_col < matrix.cols; ++i_col) {
			cv::Vec3b& p = new_image.at<cv::Vec3b>(i_col, matrix.rows - 1 - i_row);  //(/* -*/ i_col, i_row);
			p = at(i_row, i_col);
		}
	}
	this->matrix = new_image;
	int tmp = h;
	h = w; w = tmp;
	makeWxView();
}

void Image::applyZoomInTransform() {
	int w = matrix.cols, h = matrix.rows;
	cv::Mat new_image(cv::Size( 2 * w - 1, 2 * h - 1), CV_8UC3);

	// Copy old pixels
	for (int r = 0; r < h; ++r) {
		for (int c = 0; c < w; ++c) {
			cv::Vec3b& trg = new_image.at<cv::Vec3b>(2 * r, 2 * c);
			trg = at(r, c);
		}
	}

	// Fill in horizontals
	for (int r = 0; r < h; ++r) {
		for (int c = 0; c < w - 1; ++c) {
			cv::Vec3b& trg = new_image.at<cv::Vec3b>(2 * r, 2 * c + 1);
			trg = avg_pixel(at(r, c), at(r,c + 1));
		}
	}

	// Fill in verticals
	for (int r = 0; r < h - 1; ++r) {
		for (int c = 0; c < w; ++c) {
			cv::Vec3b& trg = new_image.at<cv::Vec3b>(2 * r + 1, 2 * c);
			trg = avg_pixel(at(r, c), at(r + 1, c));
		}
	}

	// Fill in corners
	for (int r = 0; r < h - 1; ++r) {
		for (int c = 0; c < w - 1; ++c) {
			cv::Vec3b& trg = new_image.at<cv::Vec3b>(2 * r + 1, 2 * c + 1);
			trg = avg_pixel(at(r, c), at(r + 1, c), at(r, c+1), at(r+1, c+1));
		}
	}

	this->matrix = new_image;
	this->w = 2 * w - 1;
	this->h = 2 * h - 1;
	makeWxView();
}

/*********************************************************/
/*             CONVOLUTION TRANSFORM.                    */
/*********************************************************/

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

Histogram::Histogram(Image& image) : m_lum_hist(256, 0) {
	m_n_pixels = image.GetW() * image.GetH();

	cv::Mat& matrix = image.GetMatrix();
	
	for (int r = 0; r < matrix.rows; ++r) {
		for (int c = 0; c < matrix.cols; ++c) {
			cv::Vec3b& p = matrix.at<cv::Vec3b>(r, c);

			int lum = 0.5 + get_luminance(p[0], p[1], p[2]);
			m_lum_hist[lum] += 1;
		}
	}
	
}

Histogram::Histogram() : m_lum_hist(256, 0), m_n_pixels(0) {

}

int Histogram::GetNPixels() {
	return m_n_pixels;
}

int Histogram::GetMaxValue() {
	int max_val = 0;
	for (int i = 0; i < 255; ++i) {
		max_val = max_val > m_lum_hist[i] ? max_val : m_lum_hist[i];
	}
	return max_val;
}

std::vector<int> Histogram::GetLumHist() {
	return m_lum_hist;
}

std::vector<int> Histogram::GetCumulative() {
	std::vector<int> cum_hist = std::vector<int>(m_lum_hist);

	for (int i = 1; i < 256; ++i) {
		cum_hist[i] += cum_hist[i - 1];
	}
	return cum_hist;
}
