#pragma once

#include <chrono>
#include <vector>

struct Rect {
	int x, y, w, h;
};

class Confetti {
public:
	void start(int screenW, int screenH, int offsetX, int offsetY,
	           std::vector<Rect> exclusionZones);
	void update();
	void stop();

private:
	struct Particle {
		int x;
		float y;
		int drawnRow;
		int color;
		float speed;
	};

	void spawnParticle(Particle& p, bool randomY) const;
	static void clearParticle(const Particle& p);
	static void drawParticle(const Particle& p);
	bool overlapsExclusion(int px, int py) const;

	std::vector<Particle> _particles;
	std::vector<Rect> _exclusions;
	std::chrono::steady_clock::time_point _lastUpdate;
	int _screenW = 0;
	int _screenH = 0;
	int _offsetX = 0;
	int _offsetY = 0;
	bool _active = false;
};
