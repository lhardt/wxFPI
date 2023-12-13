#include "effects.h"


void HorizontalEffect::applyToImage(Image & image) {
	image.applyHorTransform();
}


void VerticalEffect::applyToImage(Image& image) {
	image.applyVerTransform();
}

void LeftRotationEffect::applyToImage(Image& image) {
	image.applyRotLeftTranform();
}

void RightRotationEffect::applyToImage(Image& image) {
	image.applyRotRightTranform();
}