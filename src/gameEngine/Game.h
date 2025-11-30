// Game.h
#pragma once
#include <renderEngine/Renderer.h>

class Game {
private:
  Renderer renderer;

  double currentTime, lastTime, previousSecond, delta;
  int updates = 0;
  bool isGameOver = false;  // Cờ kết thúc game
  double gameOverTime = 0;  // Thời gian bắt đầu game over
public:
  Game();
  ~Game();

  void run();
  bool shouldRun();
  bool shouldUpdate();

  static void init();
  static Game& theOne();
};
