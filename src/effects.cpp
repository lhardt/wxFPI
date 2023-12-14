#include "effects.h"


wxString Effect::describe() {
	return "Default Effect Description";
}

void HorizontalEffect::applyToImage(Image & image) {
	image.applyHorTransform();
}
wxString HorizontalEffect::describe() {
	return wxT("HorizontalMirror");
}


void VerticalEffect::applyToImage(Image& image) {
	image.applyVerTransform();
}
wxString VerticalEffect::describe() {
	return wxT("VerticalMirror");
}

void LeftRotationEffect::applyToImage(Image& image) {
	image.applyRotLeftTranform();
}
wxString LeftRotationEffect::describe() {
	return wxT("LeftRotation");
}

void RightRotationEffect::applyToImage(Image& image) {
	image.applyRotRightTranform();
}
wxString RightRotationEffect::describe() {
	return wxT("RightRotation");
}


void GreyscaleEffect::applyToImage(Image& image) {
	// Equivalently: image.applyGreyTransform();
	cv::Mat new_mat;
	cv::cvtColor(image.GetMatrix(), new_mat, cv::COLOR_BGR2GRAY);
	image.SetMatrix(new_mat);
}
wxString GreyscaleEffect::describe() {
	return wxT("Greyscale");
}

void InvertEffect::applyToImage(Image& image) {
	image.applyInvertTransform();
}
wxString InvertEffect::describe() {
	return wxString::Format("Invert");
}

void EqualizeEffect::applyToImage(Image& image) {
	image.applyEqualize();
}
wxString EqualizeEffect::describe() {
	return wxString::Format("Equalize");
}

GaussianEffect::GaussianEffect(int r) : m_range(r) { }
wxString GaussianEffect::describe() {
	return wxString::Format("Gaussian(R=%d)", m_range);
}
void GaussianEffect::applyToImage(Image& image) {
	cv::Mat new_mat;
	cv::GaussianBlur(image.GetMatrix(), new_mat, cv::Size(m_range, m_range), 0);
	image.SetMatrix(new_mat);
}

CannyEffect::CannyEffect(double t1, double t2) : m_t1(t1), m_t2(t2) { }
wxString CannyEffect::describe() {
	return wxString::Format("Canny(%lf - %lf)", m_t1, m_t2);
}
void CannyEffect::applyToImage(Image& image) {
	cv::Mat new_mat;
	cv::Canny(image.GetMatrix(), new_mat, m_t1, m_t2);
	image.SetMatrix(new_mat);
}

SobelEffect::SobelEffect(double dx, double dy, int size) : m_dx(dx), m_dy(dy), m_size(size) { }
wxString SobelEffect::describe() {
	return wxString::Format(wxT("Sobel(dx=%lf  dy=%lf  size=%lf)"), m_dx, m_dy, m_size);
}
void SobelEffect::applyToImage(Image& image) {
	cv::Mat new_mat;
	cv::Sobel(image.GetMatrix(), new_mat, image.GetMatrix().depth(), m_dx, m_dy, m_size);
	image.SetMatrix(new_mat);
}


LinearEffect::LinearEffect(double a, double b) : m_a(a), m_b(b){

}
wxString LinearEffect::describe() {
	return wxString::Format(wxT("LinearEffect(alpha=%lf  beta=%lf)"), m_a, m_b);
}
void LinearEffect::applyToImage(Image& image) {
	cv::Mat new_mat;
	image.GetMatrix().convertTo(new_mat, -1, m_a, m_b);
	image.SetMatrix(new_mat);
}


ResizeEffect::ResizeEffect(int new_w, int new_h) : m_w(new_w), m_h(new_h) { }
wxString ResizeEffect::describe() {
	return wxString::Format(wxT("ResizeEffect(new_w= %i  new_h= %i )"), m_w, m_h);
}
void ResizeEffect::applyToImage(Image& image) {
	cv::Mat new_mat;
	cv::resize(image.GetMatrix(), new_mat, cv::Size(m_w, m_h));
	image.SetMatrix(new_mat);
}
