#include "Util.h"
#include <cmath>
#include <random>
#include <chrono>


void DrawLineInternal(float x1, float y1, float x2, float y2, uint32_t color = -1) {
	// Bresenham's line algorithm taken directly from wikipedia
	int x1i = roundf(x1);
	int y1i = roundf(y1);
	int x2i = roundf(x2);
	int y2i = roundf(y2);

	int dx = abs(x2i - x1i);
	int sx = x1i < x2i ? 1 : -1;
	int dy = -abs(y2i - y1i);
	int sy = y1i < y2i ? 1 : -1;
	int err = dx + dy;
	while (true) {
		if (x1i >= 0 && y1i >= 0 && x1i < SCREEN_WIDTH && y1i < SCREEN_HEIGHT) {
			buffer[y1i][x1i] = color;
		}

		if (x1i == x2i && y1i == y2i) break;
		int e2 = 2 * err;

		if (e2 >= dy) {
			err += dy;
			x1i += sx;
		}

		if (e2 <= dx) {
			err += dx;
			y1i += sy;
		}
	}
}

void DrawLine(float x1, float y1, float x2, float y2, uint32_t color) {
	DrawLineInternal(x1, y1, x2, y2, color);

	// Draw line second time in case it went off screen
	if (x1 < 0 || x2 < 0) {
		x1 += SCREEN_WIDTH;
		x2 += SCREEN_WIDTH;
	} else if (x1 > SCREEN_WIDTH || x2 > SCREEN_WIDTH) {
		x1 -= SCREEN_WIDTH;
		x2 -= SCREEN_WIDTH;
	}

	if (y1 < 0 || y2 < 0) {
		y1 += SCREEN_HEIGHT;
		y2 += SCREEN_HEIGHT;
	} else if (y1 > SCREEN_HEIGHT || y2 > SCREEN_HEIGHT) {
		y1 -= SCREEN_HEIGHT;
		y2 -= SCREEN_HEIGHT;
	}

	DrawLineInternal(x1, y1, x2, y2, color);
}

float RandFloat(float min, float max) {
	static std::mt19937 rd(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rd);
}

bool PointInSphere(Vector point, Vector center, float radius) {
	return Vector{ point.x - center.x, point.y - center.y }.Length() < radius;
}