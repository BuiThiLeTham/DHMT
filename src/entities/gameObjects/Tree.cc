// Tree.cc - Implementation
#include "Tree.h"
#include <models/Geometry.h>
#include <common.h>
#include <iostream>

// Màu cây
glm::vec3 treeBrown(0.4f, 0.25f, 0.1f);    // Nâu thân cây
glm::vec3 treeGreen(0.2f, 0.6f, 0.2f);     // Xanh lá cây

std::vector<Tree*> TreeManager::trees;

Tree::Tree(glm::vec3 pos, float scale) : position(pos) {
  // Thân cây (cube dài) - nâu
  trunk = new Entity(
    Geometry::cube,
    position,
    treeBrown,
    glm::vec3(0.5f * scale, 3.0f * scale, 0.5f * scale)  // Width, Height, Depth
  );
  
  // Tán lá (sphere hoặc cone) - xanh
  leaves = new Entity(
    Geometry::sphere,
    position + glm::vec3(0.0f, 4.0f * scale, 0.0f),  // Ở trên thân
    treeGreen,
    glm::vec3(2.0f * scale, 2.5f * scale, 2.0f * scale)
  );
  
  // Thêm vào scene
  Entity::addEntity(trunk);
  Entity::addEntity(leaves);
}

Tree::~Tree() {
  delete trunk;
  delete leaves;
}

// ==========================================
// TREE MANAGER
// ==========================================
void TreeManager::init() {
  std::cout << "🌳 Tạo rừng cây..." << std::endl;
  
  // Tạo nhiều cây ở các vị trí khác nhau
  // Hàng 1: Gần camera
  trees.push_back(new Tree(glm::vec3(-50.0f, -40.0f, 20.0f), 1.2f));
  trees.push_back(new Tree(glm::vec3(-30.0f, -40.0f, 15.0f), 1.0f));
  trees.push_back(new Tree(glm::vec3(-10.0f, -40.0f, 25.0f), 1.3f));
  trees.push_back(new Tree(glm::vec3(10.0f, -40.0f, 18.0f), 0.9f));
  trees.push_back(new Tree(glm::vec3(30.0f, -40.0f, 22.0f), 1.1f));
  trees.push_back(new Tree(glm::vec3(50.0f, -40.0f, 20.0f), 1.0f));
  
  // Hàng 2: Xa hơn
  trees.push_back(new Tree(glm::vec3(-55.0f, -40.0f, -10.0f), 1.4f));
  trees.push_back(new Tree(glm::vec3(-40.0f, -40.0f, -5.0f), 1.1f));
  trees.push_back(new Tree(glm::vec3(-20.0f, -40.0f, -15.0f), 1.0f));
  trees.push_back(new Tree(glm::vec3(0.0f, -40.0f, -8.0f), 1.2f));
  trees.push_back(new Tree(glm::vec3(20.0f, -40.0f, -12.0f), 0.95f));
  trees.push_back(new Tree(glm::vec3(40.0f, -40.0f, -6.0f), 1.3f));
  trees.push_back(new Tree(glm::vec3(55.0f, -40.0f, -10.0f), 1.0f));
  
  // Hàng 3: Rất xa (nhỏ hơn - xa camera)
  trees.push_back(new Tree(glm::vec3(-60.0f, -40.0f, -40.0f), 1.5f));
  trees.push_back(new Tree(glm::vec3(-35.0f, -40.0f, -35.0f), 1.2f));
  trees.push_back(new Tree(glm::vec3(-15.0f, -40.0f, -45.0f), 1.1f));
  trees.push_back(new Tree(glm::vec3(15.0f, -40.0f, -38.0f), 1.3f));
  trees.push_back(new Tree(glm::vec3(35.0f, -40.0f, -42.0f), 1.0f));
  trees.push_back(new Tree(glm::vec3(60.0f, -40.0f, -40.0f), 1.4f));
  
  // Thêm cây bên phải
  trees.push_back(new Tree(glm::vec3(-45.0f, -40.0f, 35.0f), 0.8f));
  trees.push_back(new Tree(glm::vec3(-25.0f, -40.0f, 38.0f), 1.0f));
  trees.push_back(new Tree(glm::vec3(5.0f, -40.0f, 32.0f), 0.9f));
  trees.push_back(new Tree(glm::vec3(25.0f, -40.0f, 36.0f), 1.1f));
  trees.push_back(new Tree(glm::vec3(45.0f, -40.0f, 34.0f), 0.85f));
  
  std::cout << "✅ Đã tạo " << trees.size() << " cây!" << std::endl;
}

void TreeManager::cleanup() {
  for (auto tree : trees) {
    delete tree;
  }
  trees.clear();
}

