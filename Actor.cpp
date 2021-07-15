#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Actor.h"
#include "Util.h"

void Actor::Draw() const {
	for (size_t i = 0; i < model_.size(); ++i) {
		auto cur = model_[i];
		auto next = model_[(i + 1) % model_.size()];

		cur.x *= scale;
		cur.y *= scale;
		next.x *= scale;
		next.y *= scale;

		float angle_ = angle * M_PI / 180;
		Vector cur_rot = { cur.x * cos(angle_) + cur.y * sin(angle_) + pos.x, -cur.x * sin(angle_) + cur.y * cos(angle_) + pos.y };
		Vector next_rot = { next.x * cos(angle_) + next.y * sin(angle_) + pos.x, -next.x * sin(angle_) + next.y * cos(angle_) + pos.y };
		DrawLine(cur_rot.x, cur_rot.y, next_rot.x, next_rot.y);
	}
}

void Actor::MoveStep(float time_delta) {
	pos.x += velocity.x * time_delta;
	pos.y += velocity.y * time_delta;
	pos.x = std::fmod(pos.x, SCREEN_WIDTH);
	pos.y = std::fmod(pos.y, SCREEN_HEIGHT);
	if (pos.x < 0) {
		pos.x += SCREEN_WIDTH;
	}
	if (pos.y < 0) {
		pos.y += SCREEN_HEIGHT;
	}
}

void Actor::AccelerateStep(float time_delta) {
	float angle_ = angle * M_PI / 180;
	velocity.x += cos(angle_) * acceleration * time_delta;
	velocity.y -= sin(angle_) * acceleration * time_delta;
	if (velocity.Length() > max_velocity) {
		velocity.x *= max_velocity / velocity.Length();
		velocity.y *= max_velocity / velocity.Length();
	}
}
