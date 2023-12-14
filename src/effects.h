#ifndef WXFPI_EFFECTS_H
#define WXFPI_EFFECTS_H

#include "image.h"

class Effect {
public:
	virtual void applyToImage(Image& image) = 0;
	virtual wxString describe();
};


class HorizontalEffect : public Effect {
public: void applyToImage(Image & image);
	virtual wxString describe();
};

class VerticalEffect : public Effect {
public: void applyToImage(Image& image);
	  virtual wxString describe();
};

class LeftRotationEffect : public Effect {
public: void applyToImage(Image& image);
	  virtual wxString describe();
};

class RightRotationEffect : public Effect {
public: void applyToImage(Image& image);
	  virtual wxString describe();
};

class GreyscaleEffect : public Effect {
public: void applyToImage(Image& image);
	  virtual wxString describe();
};

class EqualizeEffect : public Effect {
public: void applyToImage(Image& image);
	  virtual wxString describe();
};

class InvertEffect : public Effect {
public: void applyToImage(Image& image);
	  virtual wxString describe();
};

class GaussianEffect : public Effect {
public: 
	GaussianEffect(int range);
	virtual wxString describe();
	void applyToImage(Image& image);
	int m_range;
};

class SobelEffect : public Effect {
public:
	SobelEffect(double dx, double dy, int size);
	virtual wxString describe();
	void applyToImage(Image& image);
	double m_dx, m_dy, m_size;
};

class CannyEffect : public Effect {
public:
	CannyEffect(double t1, double t2);
	virtual wxString describe();
	void applyToImage(Image& image);
	double m_t1, m_t2;
};

class LinearEffect : public Effect {
public:
	LinearEffect(double a, double b);
	virtual wxString describe();
	void applyToImage(Image& image);
	double m_a;
	double m_b;
};

class ResizeEffect : public Effect {
public:
	ResizeEffect(int new_w, int new_h);
	virtual wxString describe();
	void applyToImage(Image& image);
	int m_w;
	int m_h;
};


#endif /* WXFPI_EFFECTS_H */
