#pragma once
#include "string"

struct VECTOR {
  float x = 0.0f, y = 0.0f;

  VECTOR(float _x = 0.0f, float _y = 0.0f) {
	x = _x;
	y = _y;
  }

  VECTOR operator+(const VECTOR& v) const {
	return { x + v.x, y + v.y };
  }

  VECTOR operator-(const VECTOR& v) const {
	return { x - v.x, y - v.y };
  }

  VECTOR operator*(const float scalar) const {
	return { x * scalar, y * scalar };
  }

  VECTOR operator/(float scalar) const {
	return { x / scalar, y / scalar };
  }

  float magnitude() const {
	return sqrtf(x * x + y * y);
  }

  VECTOR normalize() const {
	float mag = magnitude();
	if (mag == 0) return { 0, 0 };
	return { x / mag, y / mag };
  }

  float dot(const VECTOR& v) const {
	return x * v.x + y * v.y;
  }

  float cross(const VECTOR& v) const {
	return x * v.y - y * v.x;
  }
};