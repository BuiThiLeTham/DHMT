// Bird.cc - Con chim 🐦
#include "Bird.h"
#include "Camera.h"
#include <common.h>
#include <maths/Object3D.h>
#include <maths/Maths.h>
#include <utils/Debug.h>
#include <io/MouseManager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>
using std::cout;
using std::vector;

float COLLISION_SPEED_X = 0;
float COLLISION_SPEED_Y = 0;
float COLLISION_DISPLACEMENT_X = 0;
float COLLISION_DISPLACEMENT_Y = 0;

const float BIRD_MIN_SPEED = 0.48f;
const float BIRD_MAX_SPEED = 0.64f;

// Màu sắc con chim
glm::vec3 yellow(1.0f, 0.9f, 0.2f);      // Vàng - thân chim
glm::vec3 orange(1.0f, 0.6f, 0.1f);      // Cam - cánh, đuôi
glm::vec3 white(WHITE[0], WHITE[1], WHITE[2]);  // Trắng - ngực
glm::vec3 brown(BROWN[0], BROWN[1], BROWN[2]);  // Nâu - chân
glm::vec3 black(0.1f, 0.1f, 0.1f);       // Đen - mắt
glm::vec3 red(1.0f, 0.3f, 0.2f);         // Đỏ - mỏ

vector<Entity*> Bird::rigidBody;

Bird::Bird() :
  speed(0.0f),
  axisX(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)),
  axisY(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)),
  axisZ(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)),
  // Thân chim (vàng, hình cầu lớn)
  birdBody(Geometry::cockpit, glm::vec3(0.0f), yellow),
  // Ngực chim (trắng, hơi lồi)
  birdChest(Geometry::cube, glm::vec3(2.0f, -0.5f, 0.0f), white, glm::vec3(2.5f, 2.0f, 2.5f)),
  // Đầu chim (vàng, nhỏ hơn thân)
  birdHead(Geometry::cube, glm::vec3(4.0f, 1.0f, 0.0f), yellow, glm::vec3(2.0f, 2.0f, 2.0f)),
  // Mỏ chim (đỏ, nhọn)
  birdBeak(Geometry::cube, glm::vec3(5.5f, 1.0f, 0.0f), red, glm::vec3(1.5f, 0.5f, 0.8f)),
  // Đuôi chim (cam, hình quạt)
  birdTail(Geometry::cube, glm::vec3(-4.5f, 0.5f, 0.0f), orange, glm::vec3(2.0f, 0.3f, 3.0f)),
  // Cánh trái (cam, dài và mỏng)
  wingLeft(Geometry::cube, glm::vec3(0.0f, 0.5f, 5.0f), orange, glm::vec3(4.0f, 0.4f, 8.0f)),
  // Cánh phải (cam, dài và mỏng)
  wingRight(Geometry::cube, glm::vec3(0.0f, 0.5f, -5.0f), orange, glm::vec3(4.0f, 0.4f, 8.0f)),
  // Chân trái (nâu, mảnh)
  legLeft(Geometry::cube, glm::vec3(0.5f, -2.5f, 1.0f), brown, glm::vec3(0.3f, 2.0f, 0.3f)),
  // Chân phải (nâu, mảnh)
  legRight(Geometry::cube, glm::vec3(0.5f, -2.5f, -1.0f), brown, glm::vec3(0.3f, 2.0f, 0.3f)),
  // Bàn chân trái (nâu, nhỏ)
  footLeft(Geometry::cube, glm::vec3(1.0f, -3.5f, 1.0f), brown, glm::vec3(0.8f, 0.2f, 0.5f)),
  // Bàn chân phải (nâu, nhỏ)
  footRight(Geometry::cube, glm::vec3(1.0f, -3.5f, -1.0f), brown, glm::vec3(0.8f, 0.2f, 0.5f)),
  // Mắt trái (đen, nhỏ)
  eyeLeft(Geometry::cube, glm::vec3(4.8f, 1.8f, 0.8f), black, glm::vec3(0.3f, 0.3f, 0.3f)),
  // Mắt phải (đen, nhỏ)
  eyeRight(Geometry::cube, glm::vec3(4.8f, 1.8f, -0.8f), black, glm::vec3(0.3f, 0.3f, 0.3f))
{
  // Thêm các phần của con chim vào components
  components.push_back(&birdBody);
  components.push_back(&birdChest);
  components.push_back(&birdHead);
  components.push_back(&birdBeak);
  components.push_back(&birdTail);
  components.push_back(&wingLeft);
  components.push_back(&wingRight);
  components.push_back(&legLeft);
  components.push_back(&legRight);
  components.push_back(&footLeft);
  components.push_back(&footRight);
  components.push_back(&eyeLeft);
  components.push_back(&eyeRight);

  // Rigid body cho va chạm
  rigidBody.push_back(&birdBody);
  birdBody.setBody(new Sphere(6.0f));
  rigidBody.push_back(&birdHead);
  
  // Tạo lông đuôi (6 sợi)
  for (int i = 0; i < 6; ++i) {
    float offsetZ = -2.0f + (float)i * 0.8f;
    tailFeathers[i] = Entity(Geometry::cube, glm::vec3(-6.0f, 0.3f, offsetZ), orange, glm::vec3(1.5f, 0.2f, 0.5f));
    components.push_back(&(tailFeathers[i]));
  }
  
  // Tạo lông vũ trên đầu (12 sợi)
  for (int i = 0; i < 12; ++i) {
    int col = i % 3;
    int row = i / 3;
    float startX = 3.5f;
    float startY = 2.8f;
    float startZ = -0.6f;
    feathers[i] = Entity(Geometry::cube, glm::vec3(startX + (float)row * 0.3f, startY, startZ + (float)col * 0.4f), yellow, glm::vec3(0.3f));
    components.push_back(&(feathers[i]));
  }

  // Thêm tất cả entities vào scene
  for (int i = 0; i < components.size(); ++i) {
    Entity::addEntity(components[i]);
  }

  // Xoay chân hơi ra ngoài cho tự nhiên
  legLeft.changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), 0.2f);
  legRight.changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), -0.2f);
  
  translate(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z);
}

Bird::~Bird() {}

void Bird::rotate(float dx, float dy, float dz, glm::vec3 center) {
  rotation += glm::vec3(dx, dy, dz);
  float angle = dx != 0.0f ? dx : dy != 0.0f ? dy : dz;
  glm::mat4 rotationMatrix = Maths::calculateRotationMatrix(dx, dy, dz, center);
  axisX = rotationMatrix * axisX;
  axisY = rotationMatrix * axisY;
  axisZ = rotationMatrix * axisZ;
  for (int i = 0; i < components.size(); ++i) {
    components[i]->changeRotation(rotationMatrix);
  }
}

void Bird::translate(float dx, float dy, float dz) {
  position += glm::vec3(dx, dy, dz);
  glm::mat4 translationMatrix = Maths::calculateTranslationMatrix(dx, dy, dz);
  for (int i = 0; i < components.size(); ++i) {
    components[i]->changePosition(translationMatrix);
  }
}

// Cập nhật lông vũ trên đầu (bay theo gió)
void Bird::updateFeathers() {
  static float featherAngle = 0.0f;
  for (int i = 0; i < 12; ++i) {
    float height = 0.25f + glm::cos(featherAngle + i / 3) * 0.08f;
    float dy = (height - feathers[i].getScale().y) / 2;
    glm::vec3 translateVector = dy * glm::normalize(glm::vec3(axisY.x, axisY.y, axisY.z));
    feathers[i].changePosition(translateVector.x, translateVector.y, translateVector.z);
    feathers[i].setScale(0.3f, height, 0.3f);
  }
  featherAngle += 0.16f;
}

// Tạo hiệu ứng vỗ cánh
void Bird::updateWings() {
  static float wingAngle = 0.0f;
  
  // Tính góc vỗ cánh (dao động từ -30° đến +30°)
  float wingFlap = glm::sin(wingAngle) * 0.5f; // ~30 degrees
  
  // Vỗ cánh trái lên xuống
  glm::vec3 wingLeftAxis = glm::normalize(glm::vec3(axisX.x, axisX.y, axisX.z));
  static float prevWingFlapLeft = 0.0f;
  wingLeft.changeRotation(wingLeftAxis, wingFlap - prevWingFlapLeft);
  prevWingFlapLeft = wingFlap;
  
  // Vỗ cánh phải lên xuống (đối xứng)
  static float prevWingFlapRight = 0.0f;
  wingRight.changeRotation(wingLeftAxis, -wingFlap - prevWingFlapRight);
  prevWingFlapRight = -wingFlap;
  
  // Lông đuôi cũng dao động nhẹ
  for (int i = 0; i < 6; ++i) {
    float tailWave = glm::sin(wingAngle + i * 0.3f) * 0.1f;
    static float prevTailWave[6] = {0, 0, 0, 0, 0, 0};
    glm::vec3 tailAxis = glm::normalize(glm::vec3(axisY.x, axisY.y, axisY.z));
    tailFeathers[i].changeRotation(tailAxis, tailWave - prevTailWave[i]);
    prevTailWave[i] = tailWave;
  }
  
  wingAngle += 0.2f; // Tốc độ vỗ cánh
}

void Bird::update() {
  if (GAME::HEALTH <= 0.0f) {
    static float totalRotation = 0.0f;
    static float y = 0.0f;
    const float zRotation = 0.3f;
    totalRotation += zRotation;
    y += 0.01f;
    if (y <= 2.0f)
      translate(0.0f, -y, 0.0f);
    if (totalRotation < 80.0f)
      rotate(0.0f, 0.0f, -glm::radians(zRotation), position);
  } else {
    speed = Maths::clamp(MouseManager::getX(), -0.5f, 0.5f, BIRD_MIN_SPEED, BIRD_MAX_SPEED);
    float targetX = Maths::clamp(MouseManager::getX(), -1.0f, 1.0f, -AIRPLANE::AMPWIDTH, -0.7f * AIRPLANE::AMPWIDTH);
    float targetY = Maths::clamp(MouseManager::getY(), -0.75f, 0.75f, AIRPLANE::Y - AIRPLANE::AMPHEIGHT, AIRPLANE::Y + AIRPLANE::AMPHEIGHT);

    COLLISION_DISPLACEMENT_X += COLLISION_SPEED_X;
    targetX += COLLISION_DISPLACEMENT_X;

    float deltaX = targetX - position.x;
    float deltaY = targetY - position.y;
    translate(deltaX * AIRPLANE::MOVE_SENSITIVITY, deltaY * AIRPLANE::MOVE_SENSITIVITY, 0.0f);
    float targetRotationZ = deltaY * AIRPLANE::ROTATE_SENSITITY;

    rotate(0.0f, 0.0f, targetRotationZ - rotation.z, position);
    rotation.z = targetRotationZ;


    COLLISION_SPEED_X += -COLLISION_DISPLACEMENT_X * 0.2f;
    if (COLLISION_SPEED_X > 0)
      COLLISION_DISPLACEMENT_X = 0;
    COLLISION_DISPLACEMENT_X += -COLLISION_DISPLACEMENT_X * 0.1f;
  }

  // Cập nhật lông vũ
  updateFeathers();
  // Vỗ cánh
  updateWings();
  // Camera theo dõi chim
  Camera::primary().chasePoint(position);
} 

void Bird::knockBack(glm::vec3 otherPosition) {
  glm::vec3 distance = position - otherPosition;
  float length = glm::length(distance);
  COLLISION_SPEED_X = 20.0f * distance.x / length;
  AMBIENT_LIGHT_INTENSITY = 2.0f;
}

Entity& Bird::getBody() {
  return birdBody;
}

Bird& Bird::theOne() {
  static Bird bird;
  return bird;
}

