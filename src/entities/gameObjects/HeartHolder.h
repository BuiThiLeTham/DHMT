// HeartHolder.h
#pragma once
#include <entities/DynamicEntity.h>

class HeartHolder {
private:
  std::vector<DynamicEntity*> hearts;
  float lastSpawnDistance;
public:
  HeartHolder();
  ~HeartHolder();

  void spawn(float distance);
  void update();

  static HeartHolder& theOne();
};
