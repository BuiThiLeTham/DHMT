# 🎮 MÔ TẢ CHI TIẾT CÁC VẬT THỂ TRONG GAME

## Tổng quan

Game Bird Flight có 3 loại vật thể chính:
- **Bird (Con chim)**: Nhân vật do người chơi điều khiển
- **Hearts (Trái tim)**: Vật phẩm thu thập để hồi máu (+1 HP)
- **Triangles (Tam giác)**: Chướng ngại vật gây sát thương (-10 HP)

---

## 🎓 LIÊN KẾT VỚI CÁC THUẬT TOÁN ĐỒ HỌA

Document này minh họa cách **9 thuật toán đồ họa máy tính** được áp dụng vào các vật thể trong game.

### Bảng ánh xạ Thuật toán → Vật thể

| # | Thuật toán | Ngày | Áp dụng vào vật thể | Mô tả cụ thể |
|---|------------|------|---------------------|--------------|
| **1** | **Xén hình (Clipping)** | 6/10 | Bird, Hearts, Triangles | Frustum culling loại bỏ objects ngoài camera view |
| **2** | **Tô màu (Flood Fill)** | 6/10 | All objects | GPU rasterization tô màu từng pixel trên bề mặt 3D |
| **3** | **Biến đổi 2D** | 13/10 | Bird components | Translation/Rotation/Scale cho từng phần thân (31 parts) |
| **4** | **Kết hợp biến đổi 2D** | 13/10 | Bird hierarchy | MVP matrix: Model × View × Projection cho rendering |
| **5** | **Biểu diễn 3D** | 20/10 | Hearts, Triangles | Heart = Parametric curve, Triangle = Tetrahedron mesh |
| **6** | **Quan sát 3D** | 27/10 | Camera → All objects | View matrix transforms world to camera space |
| **7** | **Vẽ đường/mặt cong** | 10/11 | Hearts, Sea | Heart curve, Sea sine wave, Wing flapping curve |
| **8** | **Khử khuất** | 17/11 | All objects | Z-buffer + Back-face culling + Frustum culling |
| **9** | **Tô bóng Phong** | 24/11 | All objects | Per-pixel lighting: ambient + diffuse + specular |

### Chi tiết ứng dụng thuật toán

#### 1️⃣ Xén hình (Clipping) - 6/10

**Frustum Culling cho Hearts/Triangles**:
```cpp
// HeartHolder.cc, TriangleHolder.cc
if (object->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE) {
    delete object;  // Loại bỏ objects ngoài view frustum
}
```

**Cohen-Sutherland concept**: Objects ngoài `[offscreenLeft, offscreenRight]` bị cull

**Xem thêm**: README.md → Algorithm 1: Clipping

---

#### 2️⃣ Tô màu (Flood Fill) - 6/10

**GPU Rasterization**: Mỗi triangle được chia thành pixels

**Bird body colors**:
```cpp
birdBody   → Yellow (1.0, 0.9, 0.2)  // 200+ vertices
birdChest  → White  (1.0, 1.0, 1.0)  // Flood filled
wingLeft   → Orange (1.0, 0.6, 0.1)  // Per-pixel color
```

**Fragment shader** tô màu từng pixel dựa trên interpolated color

**Xem thêm**: README.md → Algorithm 2: Flood Fill

---

#### 3️⃣ Biến đổi 2D - 13/10

**Bird component transformations**:

| Component | Translation | Rotation | Scaling |
|-----------|-------------|----------|---------|
| birdHead | (4.0, 1.0, 0) | (0, 0, 0) | (2.0, 2.0, 2.0) |
| wingLeft | (0, 0.5, 5.0) | sin(t)×30° around X | (4.0, 0.4, 8.0) |
| feathers[i] | Dynamic | 0 | (0.3, height(t), 0.3) |

**Matrix form**:
```
T(dx,dy,dz) = [[1,0,0,dx], [0,1,0,dy], [0,0,1,dz], [0,0,0,1]]
R_z(θ) = [[cos θ, -sin θ, 0, 0], [sin θ, cos θ, 0, 0], ...]
S(sx,sy,sz) = [[sx,0,0,0], [0,sy,0,0], [0,0,sz,0], [0,0,0,1]]
```

**Xem thêm**: README.md → Algorithm 3: 2D Transformations

---

#### 4️⃣ Kết hợp biến đổi 2D - 13/10

**Hierarchical transformation cho Bird**:

```cpp
// Bird.cc - Wing transformation chain
M_wing = T_bird × R_bird × T_wing_offset × R_flap × S_wing

Step by step:
1. S_wing: Scale wing to (4.0, 0.4, 8.0)
2. R_flap: Rotate by sin(t)×0.5 rad for flapping
3. T_wing_offset: Translate to (0, 0.5, 5.0) from bird center
4. R_bird: Apply bird's rotation (pitch from mouse)
5. T_bird: Apply bird's position in world
```

**Order matters**: SRT (Scale → Rotate → Translate)

**Xem thêm**: README.md → Algorithm 4: Composite Transformations

---

#### 5️⃣ Biểu diễn 3D - 20/10

**Heart - Parametric Curve**:
```cpp
// Geometry.cc - createHeart()
x(t) = 16 × sin³(t)
y(t) = 13×cos(t) - 5×cos(2t) - 2×cos(3t) - cos(4t)
z ∈ [-0.5×scale, 0.5×scale]

Tessellation: 126 points × 10 depth slices = 1260 vertices
```

**Triangle - Polygon Mesh**:
```cpp
// Geometry.cc - createTetrahedron()
4 vertices → 4 triangular faces → 12 vertices in VBO
Regular tetrahedron with edge length ≈ 2.45
```

**Bird - Composite Object**:
```
31 primitives: Cubes (27) + Cockpit (1) + Spheres (implicit in collision)
```

**Xem thêm**: README.md → Algorithm 5: 3D Object Representation

---

#### 6️⃣ Quan sát 3D - 27/10

**View Matrix** transforms all objects to camera space:

```cpp
// Camera.cc → applied to Bird, Hearts, Triangles
mat4 viewMatrix = lookAt(cameraPos, target, up)

// In entity.vert shader
vec4 viewPos = viewMatrix × modelMatrix × vec4(position, 1.0)
gl_Position = projectionMatrix × viewPos
```

**Perspective Projection**:
```
FOV: 60°
Aspect: 1920/1080
Near: 1.0
Far: 1000.0

Hearts at distance 50 appear smaller than hearts at distance 10
```

**Xem thêm**: README.md → Algorithm 6: 3D Viewing

---

#### 7️⃣ Vẽ đường/mặt cong - 10/11

**Heart curve** (parametric):
```
Period: 2π
Samples: 2π/0.05 = 126 points
Curve type: Cardioid variation
```

**Wing flapping** (sine wave):
```cpp
// Bird.cc - updateWings()
angle(t) = 0.5 × sin(0.2t)
Period: 2π/0.2 ≈ 31.4 frames @ 60 FPS ≈ 0.52 seconds
Frequency: 1.9 Hz
```

**Sea waves** (2D sine):
```cpp
// Geometry.cc - createSea()
y(x,z) = 0.5 × sin(0.5x) × cos(0.5z)
Wavelength: 4π ≈ 12.57 units
```

**Feather breathing** (cosine):
```cpp
height(t) = 0.25 + 0.08 × cos(0.16t + i/3)
```

**Xem thêm**: README.md → Algorithm 7: Curve Drawing

---

#### 8️⃣ Khử khuất - 17/11

**Z-buffer** (all objects):
```cpp
// DisplayManager.cc
glEnable(GL_DEPTH_TEST)
glDepthFunc(GL_LESS)

// Renderer.cc
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
```

**Back-face culling**:
```cpp
glEnable(GL_CULL_FACE)
glCullFace(GL_BACK)
glFrontFace(GL_CCW)  // Counter-clockwise winding

Result: Hearts/Triangles/Bird back faces not rendered (~50% reduction)
```

**Frustum culling** (CPU-side):
```cpp
// HeartHolder.cc
if (heart->getDistance() + offscreenRight < currentDistance) {
    delete heart;  // Don't even send to GPU
}
```

**Hierarchy**:
1. Frustum culling (CPU) → Remove entire objects
2. Back-face culling (GPU) → Remove back triangles
3. Z-buffer (GPU) → Remove occluded pixels

**Xem thêm**: README.md → Algorithm 8: Hidden Surface Removal

---

#### 9️⃣ Tô bóng Phong - 24/11

**Applied to all objects** via `entity.frag`:

```glsl
// Phong lighting model
vec3 ambient  = 0.15 × lightColor
vec3 diffuse  = max(dot(normal, lightDir), 0.0) × lightColor
vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), 32) × 0.5 × lightColor

vec3 finalColor = (ambient + diffuse + specular) × objectColor
```

**Per-object colors**:
- Bird body: Yellow (1.0, 0.9, 0.2)
- Hearts: Green (0.13, 0.55, 0.13)
- Triangles: Red (0.8, 0.13, 0.13)

**Shininess**: 32 (medium glossy)

**Normal vectors**:
- Bird: Cube normals (6 directions)
- Hearts: Parametric curve tangent → normal
- Triangles: Face normals (4 directions)
- Sea: Calculated from height field derivatives

**Xem thêm**: README.md → Algorithm 9: Phong Shading

---

### 📊 Tổng hợp ứng dụng thuật toán theo vật thể

| Vật thể | Thuật toán sử dụng (số thứ tự) |
|---------|--------------------------------|
| **Bird** | 1, 2, 3, 4, 6, 7 (wing sine), 8, 9 |
| **Hearts** | 1, 2, 5 (parametric), 6, 7 (curve), 8, 9 |
| **Triangles** | 1, 2, 5 (mesh), 6, 8, 9 |
| **Sea** | 2, 5 (parametric surface), 6, 7 (sine wave), 8, 9 |

**Lưu ý**: 
- Tất cả vật thể đều dùng thuật toán 2 (Flood Fill - GPU rasterization), 6 (3D Viewing), 8 (Z-buffer), 9 (Phong)
- Bird phức tạp nhất với 7/9 thuật toán
- Hearts minh họa rõ nhất thuật toán 5 và 7 (parametric curve)

---

## 🐦 CON CHIM (Bird Class)

### Tổng quan

Con chim là **composite object** phức tạp nhất trong game, được tạo thành từ 31 entities riêng biệt kết hợp với nhau.

**File nguồn**: 
- `src/entities/gameObjects/Bird.h` (định nghĩa class)
- `src/entities/gameObjects/Bird.cc` (cài đặt)

**Số lượng components**:
```
Bird = 13 body parts + 6 tail feathers + 12 head feathers = 31 entities
```

### Cấu trúc thân chim

#### Body Parts (13 phần chính)

| Phần thân | Primitive Shape | Màu sắc | Kích thước (x,y,z) | Vị trí tương đối |
|-----------|----------------|---------|---------------------|------------------|
| **birdBody** | Cockpit (Ellipsoid) | Vàng (1.0, 0.9, 0.2) | 1.0 | (0, 0, 0) |
| **birdChest** | Cube | Trắng (1.0, 1.0, 1.0) | (2.5, 2.0, 2.5) | (2.0, -0.5, 0.0) |
| **birdHead** | Cube | Vàng (1.0, 0.9, 0.2) | (2.0, 2.0, 2.0) | (4.0, 1.0, 0.0) |
| **birdBeak** | Cube | Đỏ (1.0, 0.3, 0.2) | (1.5, 0.5, 0.8) | (5.5, 1.0, 0.0) |
| **birdTail** | Cube | Cam (1.0, 0.6, 0.1) | (2.0, 0.3, 3.0) | (-4.5, 0.5, 0.0) |
| **wingLeft** | Cube | Cam (1.0, 0.6, 0.1) | (4.0, 0.4, 8.0) | (0.0, 0.5, 5.0) |
| **wingRight** | Cube | Cam (1.0, 0.6, 0.1) | (4.0, 0.4, 8.0) | (0.0, 0.5, -5.0) |
| **legLeft** | Cube | Nâu BROWN | (0.3, 2.0, 0.3) | (0.5, -2.5, 1.0) |
| **legRight** | Cube | Nâu BROWN | (0.3, 2.0, 0.3) | (0.5, -2.5, -1.0) |
| **footLeft** | Cube | Nâu BROWN | (0.8, 0.2, 0.5) | (1.0, -3.5, 1.0) |
| **footRight** | Cube | Nâu BROWN | (0.8, 0.2, 0.5) | (1.0, -3.5, -1.0) |
| **eyeLeft** | Cube | Đen (0.1, 0.1, 0.1) | (0.3, 0.3, 0.3) | (4.8, 1.8, 0.8) |
| **eyeRight** | Cube | Đen (0.1, 0.1, 0.1) | (0.3, 0.3, 0.3) | (4.8, 1.8, -0.8) |

#### Tail Feathers (6 sợi lông đuôi)

**Code**: `Bird.cc`, dòng 75-80

```cpp
for (int i = 0; i < 6; ++i) {
    float offsetZ = -2.0f + (float)i * 0.8f;
    tailFeathers[i] = new Entity(
        Geometry::cube, 
        glm::vec3(-6.0f, 0.3f, offsetZ), 
        orange, 
        glm::vec3(1.5f, 0.2f, 0.5f)
    );
}
```

**Đặc điểm**:
- Vị trí: Từ (-6.0, 0.3, -2.0) đến (-6.0, 0.3, 2.0)
- Khoảng cách giữa các sợi: 0.8 units
- Màu cam (giống cánh)
- Kích thước: 1.5 × 0.2 × 0.5

#### Head Feathers (12 sợi lông vũ trên đầu)

**Code**: `Bird.cc`, dòng 82-87

```cpp
for (int i = 0; i < 12; ++i) {
    int col = i % 3;           // Cột (0-2)
    int row = i / 3;           // Hàng (0-3)
    float startX = 3.5f;
    float startY = 2.8f;
    float startZ = -0.6f;
    feathers[i] = new Entity(
        Geometry::cube, 
        glm::vec3(startX + row*0.3f, startY, startZ + col*0.4f),
        yellow, 
        glm::vec3(0.3f)
    );
}
```

**Đặc điểm**:
- Bố trí: Grid 3 cột × 4 hàng = 12 sợi
- Vị trí bắt đầu: (3.5, 2.8, -0.6)
- Spacing: 0.3 (X), 0.4 (Z)
- Màu vàng (giống thân)
- Kích thước ban đầu: 0.3 × 0.3 × 0.3

### Hệ thống Animation

#### 1. Wing Flapping (Vỗ cánh)

**File**: `Bird.cc`, dòng 162-188

```cpp
void Bird::updateWings() {
    static float wingAngle = 0.0f;
    
    // Sine wave animation: ±30 degrees
    float wingFlap = glm::sin(wingAngle) * 0.5f;  // 0.5 rad ≈ 30°
    
    // Left wing flaps up/down
    glm::vec3 wingLeftAxis = glm::normalize(glm::vec3(axisX));
    static float prevWingFlapLeft = 0.0f;
    wingLeft->changeRotation(wingLeftAxis, wingFlap - prevWingFlapLeft);
    prevWingFlapLeft = wingFlap;
    
    // Right wing mirrors left
    static float prevWingFlapRight = 0.0f;
    wingRight->changeRotation(wingLeftAxis, -wingFlap - prevWingFlapRight);
    prevWingFlapRight = -wingFlap;
    
    // Tail feathers wave with phase offset
    for (int i = 0; i < 6; ++i) {
        float tailWave = glm::sin(wingAngle + i * 0.3f) * 0.1f;
        static float prevTailWave[6] = {0, 0, 0, 0, 0, 0};
        glm::vec3 tailAxis = glm::normalize(glm::vec3(axisY));
        tailFeathers[i]->changeRotation(tailAxis, tailWave - prevTailWave[i]);
        prevTailWave[i] = tailWave;
    }
    
    wingAngle += 0.2f;  // Speed: 0.2 rad/frame
}
```

**Thông số**:
- **Biên độ vỗ cánh**: ±0.5 radian ≈ ±28.6 độ
- **Tốc độ**: 0.2 rad/frame × 60 FPS = 12 rad/s
- **Tần số**: 12/(2π) ≈ 1.9 Hz (gần 2 lần vỗ mỗi giây)
- **Cánh trái/phải**: Đối xứng (mirrored)
- **Lông đuôi**: 6 sợi với phase offset 0.3 rad
- **Biên độ đuôi**: ±0.1 radian ≈ ±5.7 độ

**Công thức toán học**:
```
Wing angle(t) = 0.5 × sin(ωt)
  where ω = 0.2 rad/frame

Tail feather[i] angle(t) = 0.1 × sin(ωt + i×0.3)
  Phase difference between feathers: 0.3 rad ≈ 17°
```

#### 2. Feather Breathing (Lông vũ thở)

**File**: `Bird.cc`, dòng 150-158

```cpp
void Bird::updateFeathers() {
    static float featherAngle = 0.0f;
    
    for (int i = 0; i < 12; ++i) {
        // Height oscillates: 0.25 ± 0.08
        float height = 0.25f + glm::cos(featherAngle + i/3) * 0.08f;
        
        // Calculate displacement to maintain bottom position
        float dy = (height - feathers[i]->getScale().y) / 2;
        glm::vec3 translateVector = dy * glm::normalize(glm::vec3(axisY));
        
        feathers[i]->changePosition(translateVector.x, translateVector.y, translateVector.z);
        feathers[i]->setScale(0.3f, height, 0.3f);
    }
    
    featherAngle += 0.16f;
}
```

**Thông số**:
- **Chiều cao dao động**: 0.17 đến 0.33 (±32% so với 0.25)
- **Tốc độ**: 0.16 rad/frame
- **Tần số**: 0.16×60/(2π) ≈ 1.5 Hz
- **Grouping**: i/3 → 4 nhóm (mỗi nhóm 3 sợi cùng phase)

**Công thức**:
```
height[i](t) = 0.25 + 0.08 × cos(0.16t + floor(i/3))
Range: [0.17, 0.33]
```

#### 3. Mouse Control (Điều khiển chuột)

**File**: `Bird.cc`, dòng 190-226

```cpp
void Bird::update() {
    if (GAME::HEALTH > 0) {
        // Map mouse position to movement bounds
        float targetX = Maths::clamp(
            MouseManager::getX(), 
            -1.0f, 1.0f,           // Mouse range
            -BIRD::AMPWIDTH,       // Min X
            -0.7f * BIRD::AMPWIDTH // Max X
        );
        
        float targetY = Maths::clamp(
            MouseManager::getY(),
            -0.75f, 0.75f,                      // Mouse range
            BIRD::Y - BIRD::AMPHEIGHT,          // Min Y
            BIRD::Y + BIRD::AMPHEIGHT           // Max Y
        );
        
        // Add collision knockback
        COLLISION_DISPLACEMENT_X += COLLISION_SPEED_X;
        targetX += COLLISION_DISPLACEMENT_X;
        
        // Smooth movement (lerp)
        float deltaX = targetX - position.x;
        float deltaY = targetY - position.y;
        translate(
            deltaX * BIRD::MOVE_SENSITIVITY, 
            deltaY * BIRD::MOVE_SENSITIVITY, 
            0.0f
        );
        
        // Pitch rotation based on vertical movement
        float targetRotationZ = deltaY * BIRD::ROTATE_SENSITIVITY;
        rotate(0.0f, 0.0f, targetRotationZ - rotation.z, position);
        rotation.z = targetRotationZ;
        
        // Damping for collision knockback
        COLLISION_SPEED_X += -COLLISION_DISPLACEMENT_X * 0.2f;
        if (COLLISION_SPEED_X > 0)
            COLLISION_DISPLACEMENT_X = 0;
        COLLISION_DISPLACEMENT_X += -COLLISION_DISPLACEMENT_X * 0.1f;
    }
}
```

**Movement Mapping**:

| Input | Range | Output | Range |
|-------|-------|--------|-------|
| Mouse X | [-1, 1] | Bird X | [-AMPWIDTH, -0.7×AMPWIDTH] |
| Mouse Y | [-0.75, 0.75] | Bird Y | [Y-AMPHEIGHT, Y+AMPHEIGHT] |
| Delta Y | - | Rotation Z | Proportional |

**Physics Parameters**:
- `MOVE_SENSITIVITY`: Tốc độ di chuyển (lerp factor)
- `ROTATE_SENSITIVITY`: Độ nghiêng khi bay lên/xuống
- Collision damping: 0.2 (fast) + 0.1 (slow)

#### 4. Collision Response (Phản ứng va chạm)

**File**: `Bird.cc`, dòng 238-243

```cpp
void Bird::knockBack(glm::vec3 otherPosition) {
    glm::vec3 distance = position - otherPosition;
    float length = glm::length(distance);
    
    // Impulse-based knockback
    COLLISION_SPEED_X = 20.0f * distance.x / length;
    
    // Screen flash effect
    AMBIENT_LIGHT_INTENSITY = 2.0f;
}
```

**Cơ chế**:
1. Tính vector từ vật thể va chạm đến bird
2. Normalize và nhân với impulse strength (20.0)
3. Áp dụng force theo trục X (đẩy sang bên)
4. Damping tự động trong `update()` (0.2 + 0.1)
5. Hiệu ứng ánh sáng flash (AMBIENT × 2)

**Công thức**:
```
impulse = 20.0 × (bird_pos - obstacle_pos) / |bird_pos - obstacle_pos|
velocity_x = impulse.x
displacement_x(t+1) = displacement_x(t) + velocity_x - 0.2×displacement - 0.1×displacement
```

#### 5. Death Animation (Hoạt ảnh chết)

**File**: `Bird.cc`, dòng 192-201

```cpp
if (GAME::HEALTH <= 0.0f) {
    static float totalRotation = 0.0f;
    static float y = 0.0f;
    
    const float zRotation = 0.3f;  // degrees per frame
    totalRotation += zRotation;
    y += 0.01f;
    
    // Fall down (accelerating)
    if (y <= 2.0f)
        translate(0.0f, -y, 0.0f);
    
    // Spin until face-down (~80°)
    if (totalRotation < 80.0f)
        rotate(0.0f, 0.0f, -glm::radians(zRotation), position);
}
```

**Thông số**:
- **Rotation rate**: 0.3°/frame = 18°/second @ 60 FPS
- **Total rotation**: 80° (gần vuông góc)
- **Fall acceleration**: y = 0.01 × frame² (pseudo-gravity)
- **Max fall distance**: 2.0 units

### Collision Detection

**Rigid Body Setup** (`Bird.cc`, dòng 68-70):

```cpp
rigidBody.push_back(birdBody);
birdBody->setBody(new Sphere(6.0f));
rigidBody.push_back(birdHead);
```

**Bounding volumes**:
- **Body**: Sphere với radius = 6.0 units
- **Head**: Thêm vào rigid body (collision detail)
- **Algorithm**: Sphere-sphere collision test

**Collision test** (xử lý trong `Collision.cc`):
```cpp
bool collides = distance(bird.position, object.position) < (bird.radius + object.radius)
```

### Hierarchical Transformation

**Component Tree**:

```
Bird (root transform)
├── Global Transform (player control)
│   ├── Translation (mouse X/Y → world position)
│   └── Rotation (pitch based on deltaY)
│
├── birdBody (center, collision)
├── birdChest
├── birdHead
│   ├── eyeLeft
│   ├── eyeRight
│   └── feathers[12]
│       └── Local Transform (breathing animation)
├── birdBeak
├── birdTail
│   └── tailFeathers[6]
│       └── Local Transform (wave animation)
├── wingLeft
│   └── Local Transform (flapping animation)
├── wingRight
│   └── Local Transform (flapping mirrored)
├── legLeft (static rotation)
├── legRight (static rotation)
├── footLeft
└── footRight
```

**Transform Order**:
```
Final Matrix = T_global × R_global × T_local × R_local × S_local

Example - Left Wing:
M_wing = T_bird × R_bird × T_wing_offset × R_flap × S_wing
```

**Implementation** (`Bird.cc`):

```cpp
void Bird::rotate(float dx, float dy, float dz, glm::vec3 center) {
    // Update axis vectors
    glm::mat4 rotationMatrix = Maths::calculateRotationMatrix(dx, dy, dz, center);
    axisX = rotationMatrix * axisX;
    axisY = rotationMatrix * axisY;
    axisZ = rotationMatrix * axisZ;
    
    // Apply to all components
    for (int i = 0; i < components.size(); ++i) {
        components[i]->changeRotation(rotationMatrix);
    }
}
```

---

## ❤️ TRÁI TIM (Heart Objects)

### Tổng quan

**File nguồn**: `src/entities/gameObjects/HeartHolder.cc`

Trái tim là **collectible items** giúp người chơi hồi phục máu. Được tạo từ **parametric curve** hình trái tim.

### Thuộc tính cơ bản

```cpp
Type:       BATTERY (restore health)
Geometry:   Geometry::heart (parametric curve)
Color:      GREEN (0.13, 0.55, 0.13)
Scale:      4.5f
Collision:  Sphere(4.5f)
Effect:     +1 HP when collected
```

### Parametric Heart Curve

**Công thức toán học** (`Geometry.cc`, dòng 250-300):

```
2D Heart Curve (Cartesian):
  (x² + y² - 1)³ - x²y³ = 0

Parametric Form:
  x(t) = 16 × sin³(t)
  y(t) = 13×cos(t) - 5×cos(2t) - 2×cos(3t) - cos(4t)
  
  where t ∈ [0, 2π]

Derivative (for normals):
  dx/dt = 48 × sin²(t) × cos(t)
  dy/dt = -13×sin(t) + 10×sin(2t) + 6×sin(3t) + 4×sin(4t)

3D Extrusion:
  z ∈ [-0.5×scale, 0.5×scale]
  depth_step = 0.1
```

**Tessellation**:
```
Angular step: 0.05 radians
Vertices per ring: 2π/0.05 ≈ 126
Depth slices: (0.5 - (-0.5))/0.1 = 10
Total vertices: 126 × 10 = 1260 vertices
```

**Code Implementation**:

```cpp
RawModel Geometry::createHeart(float scale) {
    std::vector<float> vertices, normals;
    float step = 0.05f;
    
    for (float t = 0; t < 2 * M_PI; t += step) {
        // Parametric equations
        float x = scale * 16 * pow(sin(t), 3);
        float y = scale * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));
        
        // Extrude to 3D
        for (float d = -0.5f; d <= 0.5f; d += 0.1f) {
            float z = d * scale;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal from tangent
            float dx_dt = scale * 48 * pow(sin(t), 2) * cos(t);
            float dy_dt = scale * (-13*sin(t) + 10*sin(2*t) + 
                                    6*sin(3*t) + 4*sin(4*t));
            
            glm::vec3 tangent(dx_dt, dy_dt, 0);
            glm::vec3 normal = glm::normalize(glm::vec3(-tangent.y, tangent.x, 0));
            
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
    
    return Loader::loadToVAO(vertices, normals);
}
```

### Spawning System

**Code**: `HeartHolder.cc`, dòng 20-46

```cpp
void HeartHolder::spawn(float distance) {
    // Check spawn condition
    if (distance >= lastSpawnDistance + miniumDist_B) {
        lastSpawnDistance = distance;
        
        // Random chance to spawn
        if (!Maths::chance(spawnChance_B)) return;
        
        // Number of hearts: 1-11
        int heartNumber = 1 + Maths::rand(0, 10);
        
        // Random height
        float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
        
        // Create formation (sine wave pattern)
        for (int i = 0; i < heartNumber; ++i) {
            float angle = offscreenLeft + i * 0.02f;
            float height = h + glm::cos((float)i * 0.2f) * 5.0f;
            
            glm::vec3 position(
                height * glm::sin(angle),
                height * glm::cos(angle) - SEA::RADIUS,
                0.0f
            );
            
            DynamicEntity* heart = new DynamicEntity(
                BATTERY,
                Geometry::heart,
                position,
                heartColor,
                4.5f  // scale
            );
            
            heart->changeRotation(i * 0.1f, i * 0.1f, 0.0f);
            heart->setBody(new Sphere(4.5f));
            heart->setDistance(distance + i * 0.03f);
            hearts.push_back(heart);
            DynamicEntity::addEntity(heart);
        }
    }
}
```

**Spawn Parameters**:

| Parameter | Ý nghĩa | Giá trị điển hình |
|-----------|---------|-------------------|
| `miniumDist_B` | Khoảng cách tối thiểu giữa các nhóm | ~50-100 units |
| `spawnChance_B` | Xác suất xuất hiện | 0.3-0.7 (30-70%) |
| `heartNumber` | Số trái tim mỗi nhóm | 1-11 (random) |
| `minHeight`, `maxHeight` | Phạm vi độ cao | Phụ thuộc SEA::RADIUS |

**Formation Pattern**:

```
Horizontal spacing: 0.02 radians per heart
Vertical wave: amplitude = 5.0, frequency = 0.2

Position calculation:
  angle[i] = offscreenLeft + i × 0.02
  height[i] = h + 5.0 × cos(i × 0.2)
  
  x[i] = height[i] × sin(angle[i])
  y[i] = height[i] × cos(angle[i]) - SEA::RADIUS
  z[i] = 0

Result: Hearts form a gentle S-curve in 3D space
```

### Animation System

**Code**: `HeartHolder.cc`, dòng 59-63

```cpp
void HeartHolder::update() {
    spawn(GAME::AIRPLANE_DISTANCE);
    
    // Cleanup off-screen hearts
    for (int i = 0; i < hearts.size(); ++i) {
        if (hearts[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE 
            || !hearts[i]->getLifespan()) {
            delete hearts[i];
            hearts.erase(hearts.begin() + i);
            --i;
        }
    }
    
    // Update rotation for all hearts
    for (auto& heart : hearts) {
        // 1. Self-rotation around Y axis
        heart->changeRotation(0.0f, 0.05f, 0.0f);
        
        // 2. Orbit around cylinder (world rotation)
        heart->changeRotation(
            glm::vec3(0.0f, 0.0f, 1.0f),  // Z axis (world)
            GAME::SPEED,                   // Angular velocity
            glm::vec3(0.0f, -SEA::RADIUS, 0.0f)  // Pivot point
        );
    }
}
```

**Dual Rotation System**:

1. **Local Rotation** (Self-spin):
   - Axis: Local Y (0, 1, 0)
   - Speed: 0.05 rad/frame = 3 rad/s @ 60 FPS
   - Frequency: 3/(2π) ≈ 0.48 Hz
   - Effect: Hearts tự quay chậm rãi

2. **World Rotation** (Orbit):
   - Axis: World Z (0, 0, 1)
   - Speed: GAME::SPEED (biến đổi theo gameplay)
   - Pivot: (0, -SEA::RADIUS, 0)
   - Effect: Hearts di chuyển theo hình trụ

**Công thức tổng hợp**:
```
M_heart = T_orbit × R_orbit × T_offset × R_spin × S_scale

where:
  T_orbit = translate to pivot (0, -SEA::RADIUS, 0)
  R_orbit = rotate around Z by GAME::SPEED
  T_offset = translate to spawn position
  R_spin = rotate around local Y by 0.05t
  S_scale = scale(4.5)
```

### Cleanup System

**Code**: `HeartHolder.cc`, dòng 48-54

```cpp
// Remove hearts that are:
// 1. Behind camera (out of view)
// 2. Collected by player (lifespan = false)

if (hearts[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE 
    || !hearts[i]->getLifespan()) {
    delete hearts[i];
    hearts.erase(hearts.begin() + i);
    --i;  // Adjust index after erase
}
```

**Conditions**:
- **Behind camera**: `distance + offscreenRight < current_distance`
- **Collected**: `lifespan == false` (set by collision handler)

---

## 🔺 TAM GIÁC (Triangle Obstacles)

### Tổng quan

**File nguồn**: `src/entities/gameObjects/TriangleHolder.cc`

Tam giác là **obstacles** (chướng ngại vật) gây sát thương cho người chơi. Được tạo từ hình **tetrahedron** (tứ diện đều).

### Thuộc tính cơ bản

```cpp
Type:       OBSTACLE (deal damage)
Geometry:   Geometry::tetrahedron (regular tetrahedron)
Color:      RED (0.8, 0.13, 0.13)
Scale:      3.0f
Collision:  Sphere(3.0f)
Effect:     -10 HP when collided
```

### Tetrahedron Geometry

**Định nghĩa** (`Geometry.cc`, dòng 200-240):

```cpp
RawModel Geometry::createTetrahedron() {
    // 4 vertices of regular tetrahedron
    glm::vec3 v0(0, 1, 0);      // Top vertex
    glm::vec3 v1(-1, -1, 1);    // Front-left
    glm::vec3 v2(1, -1, 1);     // Front-right
    glm::vec3 v3(0, -1, -1);    // Back
    
    std::vector<float> vertices, normals;
    
    // Helper lambda to add triangle with normal
    auto addTriangle = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        // Calculate face normal (CCW winding)
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        
        // Add 3 vertices with same normal
        for (glm::vec3 v : {a, b, c}) {
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    };
    
    // 4 triangular faces
    addTriangle(v0, v1, v2);  // Front face
    addTriangle(v0, v2, v3);  // Right face
    addTriangle(v0, v3, v1);  // Left face
    addTriangle(v1, v3, v2);  // Bottom face
    
    return Loader::loadToVAO(vertices, normals);
}
```

**Thuộc tính hình học**:

```
Vertices: 4
Edges: 6
Faces: 4 (all equilateral triangles)

Edge length (unit scale):
  |v0 - v1| = sqrt((0-(-1))² + (1-(-1))² + (0-1)²) = sqrt(1+4+1) = sqrt(6) ≈ 2.45
  
All edges equal length (regular tetrahedron)

Volume = (a³)/(6√2) where a = edge length
       ≈ 2.45³/(6√2) ≈ 1.73

Surface area = √3 × a² ≈ 10.39
```

**Vertex layout**:
```
Total vertices in VBO: 4 faces × 3 vertices = 12 vertices
(Each vertex repeated for flat shading with face normals)
```

### Spawning System

**Code**: `TriangleHolder.cc`, dòng 18-37

```cpp
void TriangleHolder::spawn(float distance) {
    // Check minimum distance between obstacles
    if (distance >= lastSpawnDistance + miniumDist_O) {
        lastSpawnDistance = distance;
        
        // Random spawn chance
        if (!Maths::chance(spawnChance_O)) return;
        
        // Random height
        float h = Maths::rand(minHeight, maxHeight) + SEA::RADIUS;
        
        // Position at left edge of screen
        glm::vec3 position(
            h * glm::sin(offscreenLeft),
            h * glm::cos(offscreenLeft) - SEA::RADIUS,
            0.0f
        );
        
        DynamicEntity* triangle = new DynamicEntity(
            OBSTACLE,
            Geometry::tetrahedron,
            position,
            triangleColor,  // RED
            3.0f
        );
        
        triangle->setBody(new Sphere(3.0f));
        triangle->setDistance(distance);
        triangles.push_back(triangle);
        DynamicEntity::addEntity(triangle);
    }
}
```

**Spawn Characteristics**:

| Feature | Hearts | Triangles | Lý do |
|---------|--------|-----------|-------|
| **Formation** | Group (1-11) | Single | Tránh quá nhiều chướng ngại |
| **Spacing** | miniumDist_B | miniumDist_O | miniumDist_O > miniumDist_B |
| **Pattern** | Sine wave | Random height | Obstacles không dự đoán được |
| **Spawn rate** | spawnChance_B | spawnChance_O | Thường spawnChance_O < spawnChance_B |
| **Scale** | 4.5 | 3.0 | Dễ nhìn thấy collectibles |
| **Color** | Green | Red | Warning vs Reward |

**Spawn Algorithm**:

```
Every frame:
  current_distance = GAME::AIRPLANE_DISTANCE
  
  if (current_distance >= last_spawn + miniumDist_O):
    if (random() < spawnChance_O):
      height = random(minHeight, maxHeight) + SEA::RADIUS
      angle = offscreenLeft
      position = cylindrical_to_cartesian(height, angle, 0)
      spawn_triangle(position)
      last_spawn = current_distance
```

### Animation System

**Code**: `TriangleHolder.cc`, dòng 51-55

```cpp
void TriangleHolder::update() {
    spawn(GAME::AIRPLANE_DISTANCE);
    
    // Cleanup
    for (int i = 0; i < triangles.size(); ++i) {
        if (triangles[i]->getDistance() + offscreenRight < GAME::AIRPLANE_DISTANCE 
            || !triangles[i]->getLifespan()) {
            delete triangles[i];
            triangles.erase(triangles.begin() + i);
            --i;
        }
    }
    
    // Update rotation
    for (auto& triangle : triangles) {
        // Self-rotation around Y axis
        triangle->changeRotation(0.0f, 0.05f, 0.0f);
        
        // World rotation (cylindrical orbit)
        triangle->changeRotation(
            glm::vec3(0.0f, 0.0f, 1.0f),
            GAME::SPEED,
            glm::vec3(0.0f, -SEA::RADIUS, 0.0f)
        );
    }
}
```

**Rotation System** (identical to hearts):

1. **Self-spin**: 0.05 rad/frame around local Y
2. **Orbit**: GAME::SPEED around world Z axis
3. **Pivot**: (0, -SEA::RADIUS, 0)

**Giống nhau**:
- Cả hearts và triangles dùng cùng dual rotation system
- Tốc độ tự quay: 0.05 rad/frame
- Orbit pivot: center of cylindrical world

**Khác nhau**:
- Geometry: Parametric curve vs Polyhedron
- Số lượng vertices: 1260 vs 12
- Rendering cost: Hearts cao hơn nhiều

---

## 🌊 ĐỊA HÌNH (Terrain/Sea)

### Tổng quan

**File nguồn**: 
- `src/models/Geometry.cc` (createSea)
- `shaders/sea.vert` (vertex shader)
- `shaders/sea.frag` (fragment shader)

Địa hình là **parametric surface** tạo từ hàm sine 2D, tạo hiệu ứng sóng nước.

### Parametric Surface Equation

**Code**: `Geometry.cc`, dòng 180-210

```cpp
RawModel Geometry::createSea(float width, float depth, int segments) {
    std::vector<float> vertices, normals;
    
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            // Grid position
            float x = (i / (float)segments - 0.5f) * width;
            float z = (j / (float)segments - 0.5f) * depth;
            
            // 2D sine wave for height
            float y = 0.5f * sin(x * 0.5f) * cos(z * 0.5f);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal from partial derivatives
            float dydx = 0.25f * cos(x * 0.5f) * cos(z * 0.5f);
            float dydz = -0.25f * sin(x * 0.5f) * sin(z * 0.5f);
            
            glm::vec3 normal = glm::normalize(glm::vec3(-dydx, 1.0f, -dydz));
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
    
    return Loader::loadToVAO(vertices, normals);
}
```

**Công thức toán học**:

```
Height function:
  y(x, z) = 0.5 × sin(0.5x) × cos(0.5z)

Partial derivatives:
  ∂y/∂x = 0.25 × cos(0.5x) × cos(0.5z)
  ∂y/∂z = -0.25 × sin(0.5x) × sin(0.5z)

Normal vector:
  N = normalize((-∂y/∂x, 1, -∂y/∂z))
    = normalize((-0.25cos(0.5x)cos(0.5z), 1, 0.25sin(0.5x)sin(0.5z)))
```

**Mesh parameters**:

```
Grid: segments × segments
Vertices: segments²
Triangles: (segments-1)² × 2

Example with segments=100:
  Vertices: 10,000
  Triangles: 19,602
  
Wave properties:
  Amplitude: 0.5
  Wavelength X: 2π/0.5 = 4π ≈ 12.57
  Wavelength Z: 2π/0.5 = 4π ≈ 12.57
```

### Vertex Shader Animation

**File**: `shaders/sea.vert`

```glsl
uniform float time;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

void main() {
    vec3 pos = position;
    
    // Animated wave displacement
    float wave = sin(time * 0.5 + position.x * 0.3) * 0.2;
    pos.y += wave;
    
    // Transform to world space
    vec4 worldPos = modelMatrix * vec4(pos, 1.0);
    FragPos = worldPos.xyz;
    
    // Transform to clip space
    gl_Position = projectionMatrix * viewMatrix * worldPos;
    
    // Transform normal
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
}
```

**Wave animation**:

```
displacement(x, t) = 0.2 × sin(0.5t + 0.3x)

Parameters:
  Amplitude: 0.2
  Frequency (time): 0.5 rad/s
  Frequency (space): 0.3 rad/unit
  Wavelength: 2π/0.3 ≈ 20.94 units
  Period: 2π/0.5 ≈ 12.57 seconds
  Speed: wavelength/period ≈ 1.66 units/s
```

**Combined with static wave**:

```
Total height:
  y_total = y_static + y_animated
          = 0.5×sin(0.5x)×cos(0.5z) + 0.2×sin(0.5t + 0.3x)
          
Range: [-0.7, 0.7]
```

---

## 📊 SO SÁNH CÁC VẬT THỂ

### Bảng tổng hợp

| Thuộc tính | Bird | Heart | Triangle | Sea |
|------------|------|-------|----------|-----|
| **Số entities** | 31 (composite) | Dynamic (~0-50) | Dynamic (~0-20) | 1 (static) |
| **Geometry type** | Primitives | Parametric curve | Polyhedron | Parametric surface |
| **Vertices** | ~200 total | 1260 each | 12 each | segments² |
| **Triangles** | ~100 total | ~1200 each | 4 each | 2×(segments-1)² |
| **Animation types** | 3 systems | Dual rotation | Dual rotation | Vertex shader |
| **Collision shape** | Sphere(6.0) | Sphere(4.5) | Sphere(3.0) | None |
| **Color** | Multi-color | Green | Red | Blue gradient |
| **Purpose** | Player | Collectible | Obstacle | Environment |
| **Spawn** | Singleton | Formation | Single | N/A |
| **Update cost** | High | Medium | Low | Low |
| **Render cost** | Medium | High | Low | Very High |

### Performance Analysis

**Draw calls per frame** (typical):

```
Bird:       1 draw call (31 entities batched)
Hearts:     20 draw calls (20 hearts on screen)
Triangles:  5 draw calls (5 obstacles)
Sea:        1 draw call (large mesh)
UI:         1 draw call
Background: 1 draw call
---
Total:      ~29 draw calls per frame
```

**Vertex processing** (typical frame):

```
Bird:       31 × ~6.5 vertices = ~200 vertices
Hearts:     20 × 1260 = 25,200 vertices
Triangles:  5 × 12 = 60 vertices
Sea:        100² = 10,000 vertices
---
Total:      ~35,460 vertices per frame
```

**Fragment processing** (1920×1080):

```
Full screen: 2,073,600 pixels
Z-buffer early rejection: ~50%
Average fragments: ~1,000,000 per frame @ 60 FPS = 60M fragments/s
```

### Complexity Comparison

**Creation complexity**:

| Object | Time | Space | Algorithm |
|--------|------|-------|-----------|
| Bird | O(1) | O(1) | 31 primitives |
| Heart | O(n) | O(n) | Parametric tessellation (n=1260) |
| Triangle | O(1) | O(1) | 4 vertices |
| Sea | O(n²) | O(n²) | Grid generation (n=segments) |

**Update complexity** (per object):

| Object | Computation | Memory access |
|--------|-------------|---------------|
| Bird | O(31) = O(1) | 31 transforms |
| Heart | O(1) | 2 rotations |
| Triangle | O(1) | 2 rotations |
| Sea | O(0) | GPU only |

**Collision detection** (per frame):

```
Bird vs Hearts:    O(h) where h = number of hearts
Bird vs Triangles: O(t) where t = number of triangles

Total: O(h + t) sphere-sphere tests
Typical: 20 + 5 = 25 collision checks
```

---

## 🎨 HIERARCHICAL TRANSFORMATION DETAILS

### Bird Component Hierarchy

```
Bird Root Transform
│
├─ Global Transform (controlled by player input)
│  ├─ Translation: Mouse X/Y → Cylindrical coordinates
│  └─ Rotation: Pitch based on vertical velocity
│
├─ Body Center (0, 0, 0)
│  ├─ birdBody (Cockpit)
│  │  └─ Collision Sphere(6.0)
│  ├─ birdChest (+2.0, -0.5, 0)
│  └─ birdTail (-4.5, +0.5, 0)
│     └─ tailFeathers[6] → Wave Animation
│        ├─ feather[0] at z=-2.0
│        ├─ feather[1] at z=-1.2
│        ├─ ...
│        └─ feather[5] at z=+2.0
│
├─ Head Assembly (+4.0, +1.0, 0)
│  ├─ birdHead (Cube 2×2×2)
│  ├─ birdBeak (+1.5, 0, 0)
│  ├─ eyeLeft (+0.8, +0.8, +0.8)
│  ├─ eyeRight (+0.8, +0.8, -0.8)
│  └─ feathers[12] → Breathing Animation
│     ├─ Grid layout: 3 cols × 4 rows
│     └─ Vertical scale oscillation
│
├─ Wing Assembly
│  ├─ wingLeft (0, +0.5, +5.0)
│  │  └─ Flapping Animation: ±30° around X-axis
│  └─ wingRight (0, +0.5, -5.0)
│     └─ Flapping Animation: ∓30° around X-axis (mirrored)
│
└─ Leg Assembly
   ├─ legLeft (+0.5, -2.5, +1.0)
   │  ├─ Static rotation: 0.2 rad around Z
   │  └─ footLeft (+0.5, -1.0, 0)
   └─ legRight (+0.5, -2.5, -1.0)
      ├─ Static rotation: -0.2 rad around Z
      └─ footRight (+0.5, -1.0, 0)
```

### Transform Matrix Computation

**For each component**:

```cpp
// Pseudo-code for component transformation

// 1. Local offset from parent
mat4 T_local = translate(component.localPosition)

// 2. Local rotation/scale
mat4 R_local = rotate(component.localRotation)
mat4 S_local = scale(component.localScale)

// 3. Animation transform (if any)
mat4 T_anim = animationTransform(time)

// 4. Parent (bird) global transform
mat4 T_global = translate(bird.position)
mat4 R_global = rotate(bird.rotation)

// 5. Final transform
mat4 M_component = T_global × R_global × T_local × T_anim × R_local × S_local

// 6. Send to shader
uniform mat4 modelMatrix = M_component
```

**Example - Left Wing**:

```
Local offset: (0, 0.5, 5.0)
Local scale: (4.0, 0.4, 8.0)
Animation: Rotate around X by sin(t)×0.5

M_wing = T_bird × R_bird × T_offset × R_flap × S_scale

Numeric example at t=0, bird at (10, 5, 0), rotation (0, 0, 0.1):

T_bird = [1  0  0  10]    R_bird = [cos(0.1)  -sin(0.1)  0  0]
         [0  1  0   5]             [sin(0.1)   cos(0.1)  0  0]
         [0  0  1   0]             [0          0         1  0]
         [0  0  0   1]             [0          0         0  1]

T_offset = translate(0, 0.5, 5)
R_flap = rotate_x(sin(t) × 0.5) = rotate_x(0) = I
S_scale = scale(4, 0.4, 8)

Final position of wing center ≈ (10, 5.5, 5) in world space
```

---

## 🔬 ADVANCED TOPICS

### Normal Calculation for Parametric Surfaces

**Heart curve normals**:

```
Given parametric surface: P(t) = (x(t), y(t))

Tangent vector:
  T = dP/dt = (dx/dt, dy/dt, 0)

Normal vector (2D → 3D):
  N = (-dy/dt, dx/dt, 0)  // Rotate tangent by 90°
  N_normalized = N / |N|

For heart:
  dx/dt = 48 sin²(t) cos(t)
  dy/dt = -13sin(t) + 10sin(2t) + 6sin(3t) + 4sin(4t)
  
  N = normalize((13sin(t) - 10sin(2t) - 6sin(3t) - 4sin(4t),
                 48sin²(t)cos(t),
                 0))
```

**Sea surface normals**:

```
Given height field: y = f(x, z)

Partial derivatives:
  ∂y/∂x = fx
  ∂y/∂z = fz

Tangent vectors:
  Tx = (1, fx, 0)
  Tz = (0, fz, 1)

Normal (cross product):
  N = Tx × Tz = (1, fx, 0) × (0, fz, 1)
              = (fx×1 - 0×fz, 0×0 - 1×1, 1×fz - fx×0)
              = (fx, -1, fz)
  
But we want upward normal, so negate:
  N = (-fx, 1, -fz)
  N_normalized = N / sqrt(fx² + 1 + fz²)
```

### Collision Optimization

**Spatial partitioning** (not implemented, but recommended):

```
Current: O(n) collision checks per frame
  - Bird vs all hearts: O(h)
  - Bird vs all triangles: O(t)
  - Total: O(h + t)

With uniform grid:
  - Grid cell size: 2 × max(object_radius)
  - Bird only checks objects in same cell
  - Average: O(h/cells + t/cells)
  - Best case: O(1)

With cylindrical sectors:
  - Divide world into angular sectors
  - Only check objects in bird's sector ± 1
  - Reduction: ~8x fewer checks
```

**Bounding sphere optimization**:

```cpp
// Current implementation
bool collides = distance(a.pos, b.pos) < a.radius + b.radius

// Optimized (avoid sqrt)
float radiusSum = a.radius + b.radius
bool collides = distanceSquared(a.pos, b.pos) < radiusSum * radiusSum

// Further optimization: AABB pre-test
if (abs(a.x - b.x) > radiusSum) return false  // Early exit
if (abs(a.y - b.y) > radiusSum) return false
if (abs(a.z - b.z) > radiusSum) return false
return distanceSquared(a.pos, b.pos) < radiusSum * radiusSum
```

---

## 📚 REFERENCES

### Mathematical Formulas

1. **Parametric Heart Curve**: 
   - Cartesian: (x²+y²-1)³ - x²y³ = 0
   - Source: Wolfram MathWorld

2. **Sine Wave**:
   - y = A×sin(ωt + φ)
   - Period: T = 2π/ω

3. **Transformation Matrices**:
   - Translation: T(dx,dy,dz) = [[1,0,0,dx],[0,1,0,dy],[0,0,1,dz],[0,0,0,1]]
   - Rotation around axis: Rodrigues' formula

4. **Normal Calculation**:
   - For parametric curves: N ⊥ T (tangent)
   - For height fields: N = normalize((-∂f/∂x, 1, -∂f/∂z))

### Code References

- **Bird.cc**: Complete composite object implementation
- **HeartHolder.cc**: Dynamic spawning and formation system
- **TriangleHolder.cc**: Obstacle management
- **Geometry.cc**: Parametric surface generation
- **Collision.cc**: Sphere-sphere collision detection

---

**Document version**: 1.0  
**Last updated**: December 1, 2025  
**Author**: DHMT Project Team
