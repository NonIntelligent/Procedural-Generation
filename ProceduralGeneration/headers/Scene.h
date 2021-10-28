#pragma once

#include <vector>
#include <string>
#include <algorithm>

class Scene {
	int width = 1280;
	int height = 720;

	const int MAP_SIZE = 65;
	unsigned long long seed = 123456789;

	void setupLights();

public:

	Scene();
	~Scene();

	void init();

	void generateScene();

	void deletePrimative();
	void deleteAllObjects();
	void deleteResources();

	int getWidth() const;
	int getHeight() const;

	void setRenderableSize(int width, int height);
};