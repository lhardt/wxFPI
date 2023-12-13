#include "effects.h"


void HorizontalEffect::applyToImage(Image & image) {
	image.applyHorTransform();
}

void VerticalEffect::applyToImage(Image& image) {
	image.applyVerTransform();
}