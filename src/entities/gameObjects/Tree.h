// Tree.h - Cây 🌳
#pragma once
#include <entities/Entity.h>
#include <glm/glm.hpp>

class Tree {
private:
  Entity* trunk;    // Thân cây
  Entity* leaves;   // Tán lá
  glm::vec3 position;

public:
  Tree(glm::vec3 pos, float scale = 1.0f);
  ~Tree();
  
  glm::vec3 getPosition() const { return position; }
};

// TreeManager - Quản lý nhiều cây
class TreeManager {
private:
  static std::vector<Tree*> trees;

public:
  static void init();
  static void cleanup();
  static std::vector<Tree*>& getTrees() { return trees; }
};

