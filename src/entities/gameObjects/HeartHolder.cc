// HeartHolder.cc
#include "HeartHolder.h"
#include <common.h>
#include <maths/Object3D.h>
#include <maths/Maths.h>
#include <models/Geometry.h>
#include <iostream>
using std::vector;

glm::vec3 heartColor(GREEN[0], GREEN[1], GREEN[2]); // Trái tim màu xanh lá

HeartHolder::HeartHolder() {}

HeartHolder::~HeartHolder() {
  for (auto& heart : hearts) {
    delete heart;
  }
}

void HeartHolder::spawn(float distance) {
  if (distance >= lastSpawnDistance + miniumDist_B) {
    lastSpawnDistance = distance;
    if (!Maths::chance(spawnChance_B))
      return;
    int heartNumber = 1 + Maths::rand(0, 10);
    float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
    for (int i = 0; i < heartNumber; ++i) {
      float angle = offscreenLeft + i * 0.02f;
      float height = h + glm::cos((float)i * 0.2f) * 5.0f;
      glm::vec3 position(height * glm::sin(angle), height * glm::cos(angle) - SEA::RADIUS, 0.0f);
      float scale = 4.5f; // To hơn để thấy rõ hình trái tim
      DynamicEntity* heart = new DynamicEntity(
        BATTERY,
        Geometry::heart, // Hình trái tim màu xanh lá
        position,
        heartColor, // Màu xanh lá
        scale
      );
      heart->changeRotation(i * 0.1f, i * 0.1f, 0.0f);
      heart->setBody(new Sphere(scale));
      heart->setDistance(distance + i * 0.03f);
      hearts.push_back(heart);
      DynamicEntity::addEntity(heart);
    }
  }
}

void HeartHolder::update() {
  spawn(GAME::AIRPLANE_DISTANCE);
  for (int i = 0; i < hearts.size(); ++i) {
    if (hearts[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE || !hearts[i]->getLifespan()) {
      delete hearts[i];
      hearts.erase(hearts.begin() + i);
      --i;
    }
  }
  // update rotation
  for (auto& heart : hearts) {
    heart->changeRotation(0.0f, 0.05f, 0.0f);
    heart->changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), GAME::SPEED, glm::vec3(0.0f, -SEA::RADIUS, 0.0f));
  }
}

HeartHolder& HeartHolder::theOne() {
  static HeartHolder heartHolder;
  return heartHolder;
}
