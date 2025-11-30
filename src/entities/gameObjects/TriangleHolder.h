// TriangleHolder.h
#pragma once
#include <entities/DynamicEntity.h>

class TriangleHolder {
private:
  std::vector<DynamicEntity*> triangles;
  float lastSpawnDistance;
public:
  TriangleHolder();
  ~TriangleHolder();

  void spawn(float distance);
  void update();

  static TriangleHolder& theOne();
};
