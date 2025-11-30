// Game.cc
#include "Game.h"
#include "Collision.h"
#include <common.h>
#include <maths/Maths.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Sky.h>
#include <entities/gameObjects/Bird.h>
#include <entities/gameObjects/ObstacleHolder.h>
#include <entities/gameObjects/BatteryHolder.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <entities/gameObjects/Camera.h>
// #include <entities/gameObjects/Tree.h>  // ❌ Đã xóa cây
#include <models/Geometry.h>
#include <renderEngine/DisplayManager.h>
#include <io/MouseManager.h>
#include <io/Parser.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
using std::cout;

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

float GAME::AIRPLANE_DISTANCE = 0.0f;
float GAME::MILES = 0.0f;
float GAME::HEALTH = 100.0f;
float TIMER = 0;

/* Helper function declaration */
void updateFPSCount(double& previousSecond, int& updates);

Game::Game() {
  currentTime = 0;
  lastTime = DisplayManager::getTime();
  previousSecond = lastTime;
  delta = 0;
  updates = 0;
}

Game::~Game() {
  // TreeManager::cleanup();  // ❌ Đã xóa cây
  DisplayManager::cleanDisplay();
  Geometry::cleanGeometry();
}

void Game::init() {
  Parser::parse();
  DisplayManager::createDisplay();
  Geometry::initGeometry();
  Light::theOne().setPosition(LIGHT::X, LIGHT::Y, LIGHT::Z);
  
  // ❌ XÓA CÂY
  // TreeManager::init();
}

Game& Game::theOne() {
  static Game game;
  return game;
}

void Game::run() {
    if (shouldUpdate()) {
      // temporary code for updating game angle
      GAME::AIRPLANE_DISTANCE += GAME::SPEED;

      MouseManager::update();
      Camera::primary().update();
      Light::theOne().update();
      DisplayManager::prepareDisplay();

      // update light intensity
      AMBIENT_LIGHT_INTENSITY = glm::max(1.0f, AMBIENT_LIGHT_INTENSITY - 0.05f);
      // check collision
      Collision::checkCollisionAgainstPlane();
      ParticleHolder::theOne().update();

      renderer.render();

      ObstacleHolder::theOne().update();
      BatteryHolder::theOne().update();
      // Sky::theOne().update();  // ❌ XÓA ĐÁM MÂY
      Bird::theOne().update();  // 🐦 Con chim
      DisplayManager::updateDisplay();

      // update health
      GAME::HEALTH -= 0.025f;
      GAME::HEALTH = Maths::clamp(-0.1f, GAME::HEALTH, 100.0f);
      
      // ⚠️ KIỂM TRA HẾT MÁU → GAME OVER
      if (GAME::HEALTH <= 0.0f && !isGameOver) {
        isGameOver = true;
        gameOverTime = DisplayManager::getTime();
        std::cout << "\n";
        std::cout << "=================================\n";
        std::cout << "💀 GAME OVER - HẾT MÁU!\n";
        std::cout << "=================================\n";
        std::cout << "Quãng đường: " << GAME::AIRPLANE_DISTANCE << "\n";
        std::cout << "Game sẽ kết thúc sau 3 giây...\n";
      }
      
      // Chờ 3 giây sau khi game over rồi thoát
      if (isGameOver && (DisplayManager::getTime() - gameOverTime > 3.0)) {
        DisplayManager::closeDisplay();
      }
      
      ++updates;
    }

    if (GAME::DISPLAY_FPS)
      updateFPSCount(previousSecond, updates);
}

bool Game::shouldRun() {
  return !DisplayManager::shouldCloseDisplay();
}

bool Game::shouldUpdate() {
  currentTime = DisplayManager::getTime();
  delta += currentTime - lastTime;
  lastTime = currentTime;
  if (delta >= 1.0 / GAME::FPS) {
    delta -= 1.0/ GAME::FPS;
    ++TIMER;
    return true;
  } else {
    return false;
  }
}

/* Helper function implementation */

void updateFPSCount(double& previousSecond, int& updates) {
  if (DisplayManager::getTime() - previousSecond < 1.0) {
    return;
  }

  ++previousSecond;
  cout << "FPS: " << updates << "\n";
  updates = 0;
}
