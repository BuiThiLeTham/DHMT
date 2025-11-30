// Bird.h
#pragma once
#include <models/Geometry.h>
#include <entities/Entity.h>
#include <vector>

class Entity;

class Bird {
private:
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec4 axisX, axisY, axisZ;
  std::vector<Entity*> components;
  float speed;

  // bird body parts
  Entity birdBody;      // Thân chim
  Entity birdChest;     // Ngực chim
  Entity birdTail;      // Đuôi chim
  Entity birdHead;      // Đầu chim
  Entity birdBeak;      // Mỏ chim
  Entity wingLeft;      // Cánh trái
  Entity wingRight;     // Cánh phải
  Entity legLeft;       // Chân trái
  Entity legRight;      // Chân phải
  Entity footLeft;      // Bàn chân trái
  Entity footRight;     // Bàn chân phải
  Entity eyeLeft;       // Mắt trái
  Entity eyeRight;      // Mắt phải
  
  // feathers (lông vũ)
  Entity tailFeathers[6];  // Lông đuôi
  Entity feathers[12];     // Lông vũ trên đầu

public:
  Bird();
  ~Bird();

  void updateFeathers();
  void updateWings();
  void rotate(float dx, float dy, float dz, glm::vec3 center);
  void translate(float dx, float dy, float dz);
  void update();
  Entity& getBody();
  void knockBack(glm::vec3 otherPosition);

  static Bird& theOne();
  static std::vector<Entity*> rigidBody;
};
