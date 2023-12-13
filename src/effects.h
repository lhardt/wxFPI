#ifndef WXFPI_EFFECTS_H
#define WXFPI_EFFECTS_H

#include "image.h"

class HorizontalEffect : public Effect {
	public: void applyToImage(Image & image);
};
class VerticalEffect : public Effect {
	public: void applyToImage(Image& image);
};
class LeftRotationEffect : public Effect {
	public: void applyToImage(Image& image);
};
class RightRotationEffect : public Effect {
	public: void applyToImage(Image& image);
};


#endif /* WXFPI_EFFECTS_H */
