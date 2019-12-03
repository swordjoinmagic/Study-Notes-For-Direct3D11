#include "MathF.h"

const float MathF::PI = 3.1415926f;
const float MathF::Deg2Rad = MathF::PI / 180.0f;

float MathF::Radians(float angle) {
	return angle * MathF::Deg2Rad;
}