#pragma once
#include <vector>
#include "Util.h"


class Actor {
public:
	void Draw() const;
	void MoveStep(float time_delta);
	void AccelerateStep(float time_delta);


	inline void SetModel(std::vector<Vector> model) {
		model_ = std::move(model);
	}

public:
	Vector pos;
	float angle {0};
	Vector velocity;
	float acceleration {0};
	float max_velocity {100};
	float radius {1};
	float scale {1};

private:
	std::vector<Vector> model_;
};

