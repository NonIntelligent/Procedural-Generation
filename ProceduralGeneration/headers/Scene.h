#pragma once

#include <vector>
#include <string>
#include <algorithm>

class Scene {
	int width = 1280;
	int height = 720;

	void setupLights();

public:

	Scene();
	~Scene();

	void init();

	void deletePrimative();
	void deleteAllObjects();
	void deleteResources();

	int getWidth() const;
	int getHeight() const;

	void setRenderableSize(int width, int height);
};