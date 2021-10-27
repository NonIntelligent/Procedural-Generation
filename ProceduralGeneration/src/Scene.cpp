#include "Scene.h"
#include "vendor/OBJ_Loader.h"

#include <string>
#include <fstream>
#include <algorithm>

Scene::Scene() {

}

Scene::~Scene() {

}

void Scene::setupLights() {

}


void Scene::init() {
	setupLights();
}


void Scene::deletePrimative() {

}

void Scene::deleteAllObjects() {

}

void Scene::deleteResources() {

}

int Scene::getWidth() const{
	return width;
}

int Scene::getHeight() const{
	return height;
}


void Scene::setRenderableSize(int width, int height) {
	this->width = width;
	this->height = height;
}

