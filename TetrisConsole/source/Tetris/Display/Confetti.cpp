#include "Confetti.h"

#include <algorithm>
#include <iostream>

#include "Color.h"
#include "Random.h"
#include "rlutil.h"

using namespace std;

static constexpr int kParticleCount = 25;
static constexpr int kMinSpeed = 4;
static constexpr int kMaxSpeed = 12;

static const int kColors[] = {
	Color::LIGHTBLUE, Color::BLUE, Color::BROWN, Color::YELLOW,
	Color::GREEN, Color::MAGENTA, Color::RED
};
static constexpr int kColorCount = 7;

void Confetti::start(int screenW, int screenH, int offsetX, int offsetY,
                     vector<Rect> exclusionZones) {
	_screenW = screenW;
	_screenH = screenH;
	_offsetX = offsetX;
	_offsetY = offsetY;
	_exclusions = move(exclusionZones);
	_active = true;

	_particles.resize(kParticleCount);
	for (auto& p : _particles)
		spawnParticle(p, true);

	_lastUpdate = chrono::steady_clock::now();
}

void Confetti::update() {
	if (!_active) return;

	auto now = chrono::steady_clock::now();
	float elapsed = chrono::duration<float>(now - _lastUpdate).count();
	_lastUpdate = now;

	for (auto& p : _particles) {
		p.y += p.speed * elapsed;
		int newRow = static_cast<int>(p.y);

		if (newRow > _offsetY + _screenH) {
			clearParticle(p);
			spawnParticle(p, false);
			continue;
		}

		if (newRow != p.drawnRow) {
			clearParticle(p);
			if (!overlapsExclusion(p.x, newRow)) {
				p.drawnRow = newRow;
				drawParticle(p);
			} else {
				p.drawnRow = -1;
			}
		}
	}
}

void Confetti::stop() {
	if (!_active) return;

	for (const auto& p : _particles)
		clearParticle(p);

	_particles.clear();
	_active = false;
}

void Confetti::spawnParticle(Particle& p, bool randomY) const {
	int slots = _screenW / 2;
	int slot = Random::getInteger(0, slots - 1);
	p.x = _offsetX + 1 + slot * 2;

	if (randomY)
		p.y = static_cast<float>(_offsetY + 1 + Random::getInteger(0, _screenH - 1));
	else
		p.y = static_cast<float>(_offsetY + 1);

	p.drawnRow = -1;
	p.color = kColors[Random::getInteger(0, kColorCount - 1)];
	p.speed = static_cast<float>(Random::getInteger(kMinSpeed, kMaxSpeed));
}

void Confetti::clearParticle(const Particle& p) {
	if (p.drawnRow < 1) return;
	rlutil::locate(p.x, p.drawnRow);
	rlutil::setBackgroundColor(Color::BLACK);
	cout << "  ";
}

void Confetti::drawParticle(const Particle& p) {
	if (p.drawnRow < 1) return;
	rlutil::locate(p.x, p.drawnRow);
	rlutil::setColor(p.color);
	rlutil::setBackgroundColor(Color::BLACK);
	cout << "██";
}

bool Confetti::overlapsExclusion(int px, int py) const {
	return std::any_of(_exclusions.begin(), _exclusions.end(), [px, py](const Rect& r) {
		return py >= r.y && py < r.y + r.h &&
		       px + 1 >= r.x && px < r.x + r.w;
	});
}
