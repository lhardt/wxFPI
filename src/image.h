#ifndef WXFPI_IMAGE_H
#define WXFPI_IMAGE_H

#include <string>
#include "wx.h"
#include <opencv2/opencv.hpp>
#include <vector>

typedef class Kernel Kernel;
typedef class Histogram Histogram;
typedef class Effect Effect;

class Image {
private:
	cv::Mat matrix;
	wxBitmap bitmap;
	int w, h;

public:
	Image(std::string filename);
	Image(cv::Mat matrix);
	Image(const Image& other);
	Image(int h, int w);

	void SaveAs(std::string filename);

	void refreshWxView();
	wxBitmap GetWxBitmap() const;
	cv::Mat& GetMatrix();
	void SetMatrix(const cv::Mat &mat);
	int GetW() const;
	int GetH() const;

	cv::Vec3b& at(int x, int y);

	void applyEffect(Effect* effect, bool updateView = false);

	/* Color Transformations */
	void applyInvertTransform();
	void applyGreyTransform();
	void applyQuantTranform(int n_tones);
	void applyBrightnessEnh(double value);
	void applyContrastEnh(double value);
	void applyEqualize();
	void applyHistogram(Histogram& hist);

	/* Image Transformations */
	void applyHorTransform();
	void applyVerTransform();
	void applyRotLeftTranform();
	void applyRotRightTranform();
	void applyZoomInTransform();
	/* Convolution */
	void applyKernel(const Kernel & kernel);
};

double get_luminance(int r, int g, int b);

class Histogram {
public:
	Histogram(Image& image);

	int GetNPixels();
	int GetMaxValue();
	std::vector<int> GetLumHist();
	std::vector<int> GetCumulative();
	std::vector<double> GetRelCumulative();
private:
	Histogram();

	std::vector<int> m_lum_hist;
	int m_n_pixels;
};

enum KernelBorderBehaviour {
	COPY_BORDER
};

class Kernel {
public:
	KernelBorderBehaviour cornerBehaviour;
	double weights[3][3];
	double shift;

	double applyToPixelChannel(const Kernel& kernel,
		uchar p00, uchar p01, uchar p02,
		uchar p10, uchar p11, uchar p12,
		uchar p20, uchar p21, uchar p22) const;
	cv::Vec3b applyToPixel(const Kernel& kernel,
		const cv::Vec3b& p00, const cv::Vec3b& p01, const cv::Vec3b& p02,
		const cv::Vec3b& p10, const cv::Vec3b& p11, const cv::Vec3b& p12,
		const cv::Vec3b& p20, const cv::Vec3b& p21, const cv::Vec3b& p22) const;
};

const extern Kernel KERNEL_GAUSS;
const extern Kernel KERNEL_LAPLACE;
const extern Kernel KERNEL_HIGH;
const extern Kernel KERNEL_PREW_HX;
const extern Kernel KERNEL_PREW_HY;
const extern Kernel KERNEL_SOBEL_HX;
const extern Kernel KERNEL_SOBEL_HY;




#endif /* WXFPI_IMAGE_H */