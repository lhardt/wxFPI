#ifndef WXFPI_IMAGE_H
#define WXFPI_IMAGE_H

#include <string>
#include "wx.h"
#include <opencv2/opencv.hpp>

typedef class Kernel Kernel;

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

	cv::Vec3b& at(int x, int y);

	void applyInvertTransform();
	void applyGreyTransform();
	void applyHorTransform();
	void applyVerTransform();
	void applyQuantTranform(int n_tones);
	void applyKernel(const Kernel & kernel);
};


double get_luminance(int r, int g, int b);

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