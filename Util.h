#pragma once
#include "Engine.h"
#include <cmath>

struct Vector {
	float x {0};
	float y {0};

	inline float Length() {
		return sqrt(x * x + y * y);
	}
};

void DrawLine(float x1, float y1, float x2, float y2, uint32_t color = -1);
float RandFloat(float min = 0., float max = 1.);

bool PointInSphere(Vector point, Vector center, float radius);