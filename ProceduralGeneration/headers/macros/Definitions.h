#pragma once

#define PI		3.14159265359
#define PI_f	3.1415927f

inline double toRad(double degrees) {
	return (degrees * PI) / 180;
}

inline float toRadf(float degrees) {
	return (degrees * PI_f) / 180;
}

inline double toDegrees(double radians) {
	return (radians * 180) / PI;
}

inline float toDegreesf(float radians) {
	return (radians * 180) / PI_f;
}