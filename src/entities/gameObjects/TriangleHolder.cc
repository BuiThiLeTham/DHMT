// TriangleHolder.cc
#include "TriangleHolder.h"
#include <maths/Object3D.h>
#include <common.h>
#include <utils/Debug.h>
#include <maths/Maths.h>
#include <models/Geometry.h>
#include <iostream>

glm::vec3 triangleColor(RED[0], RED[1], RED[2]);

TriangleHolder::TriangleHolder(): lastSpawnDistance(offscreenLeft) {}

TriangleHolder::~TriangleHolder() {
  for (auto& triangle : triangles) {
    delete triangle;
  }
}

void TriangleHolder::spawn(float distance) {
  if (distance >= lastSpawnDistance + miniumDist_O) {
    lastSpawnDistance = distance;
    if (!Maths::chance(spawnChance_O))
      return;
    float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
    glm::vec3 position(h * glm::sin(offscreenLeft), h * glm::cos(offscreenLeft) - SEA::RADIUS, 0.0f);
    float scale = 3.0f;
    DynamicEntity* triangle = new DynamicEntity(
      OBSTACLE,
      Geometry::tetrahedron, // Hình tứ diện (tam giác)
      position,
      triangleColor, // Màu đỏ
      scale
    );
    triangle->setBody(new Sphere(scale));
    triangle->setDistance(distance);
    triangles.push_back(triangle);
    DynamicEntity::addEntity(triangle);
  }
}

void TriangleHolder::update() {
  spawn(GAME::AIRPLANE_DISTANCE);
  for (int i = 0; i < triangles.size(); ++i) {
    if (triangles[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE || !triangles[i]->getLifespan()) {
      delete triangles[i];
      triangles.erase(triangles.begin() + i);
      --i;
    }
  }
  // update rotation
  for (auto& triangle : triangles) {
    triangle->changeRotation(0.0f, 0.05f, 0.0f);
    triangle->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), GAME::SPEED, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
  }
}

TriangleHolder& TriangleHolder::theOne() {
  static TriangleHolder triangleHolder;
  return triangleHolder;
}
