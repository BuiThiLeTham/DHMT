# ĐỒ HỌA MÁY TÍNH - BIRD FLIGHT GAME
## OpenGL 3D Graphics Algorithms Implementation

Dự án game 3D bay chim được phát triển bằng OpenGL 3.3+, minh họa đầy đủ các thuật toán và kỹ thuật đồ họa máy tính từ cơ bản đến nâng cao.

---

## 🎮 MÔ TẢ GAME

### Gameplay
- **Nhân vật**: Con chim với cánh vỗ động (flapping animation)
- **Mục tiêu**: Thu thập trái tim xanh (+1 HP), tránh tam giác đỏ (-10 HP)
- **Điều khiển**: Di chuyển chuột để bay lên/xuống
- **Môi trường**: Bầu trời xanh gradient, địa hình mặt đất

### Đặc điểm kỹ thuật
- **Engine**: OpenGL 3.3 Core Profile
- **Shading Language**: GLSL 330
- **Libraries**: GLFW3 (window), GLAD (OpenGL loader), GLM (math)
- **Build System**: CMake 3.10+

---

## 📚 CÁC THUẬT TOÁN ĐỒ HỌA ĐƯỢC CÀI ĐẶT

| # | Thuật toán | Ngày | Độ phức tạp | Mục đích |
|---|------------|------|-------------|----------|
| 1 | Xén hình (Clipping) | 6/10 | O(n) | Loại bỏ vật thể ngoài view frustum |
| 2 | Tô màu (Flood Fill) | 6/10 | O(n×m) | Tô màu bề mặt 3D |
| 3 | Biến đổi 2D | 13/10 | O(1) | Translation, Rotation, Scaling |
| 4 | Kết hợp biến đổi 2D | 13/10 | O(1) | Matrix multiplication (MVP) |
| 5 | Biểu diễn 3D | 20/10 | O(n) | Parametric surfaces, polygon mesh |
| 6 | Quan sát 3D | 27/10 | O(1) | View & Projection matrices |
| 7 | Vẽ đường/mặt cong | 10/11 | O(n) | Parametric curves, tessellation |
| 8 | Khử khuất | 17/11 | O(n×m) | Z-buffer, back-face culling |
| 9 | Tô bóng Phong | 24/11 | O(p) | Per-pixel lighting |

---

## 🚀 HƯỚNG DẪN COMPILE VÀ CHẠY

### Yêu cầu hệ thống
- **CMake**: 3.10 trở lên
- **C++ Compiler**: GCC 7+, Clang 5+, hoặc MSVC 2017+
- **OpenGL**: 3.3 trở lên
- **Thư viện**: GLFW3, GLAD, GLM (đã có trong `external/`)

### Compile trên Windows (MSYS2/MinGW)

```bash
# 1. Mở MSYS2 terminal
cd /d/tool/c++/dhmt/DHMT

# 2. Tạo thư mục build
mkdir build
cd build

# 3. Generate Makefile
cmake -G "MSYS Makefiles" ..

# 4. Compile
make -j4

# 5. Chạy game
./TheAviator.exe
```

### Compile trên Linux/macOS

```bash
mkdir build
cd build
cmake ..
make -j4
./TheAviator
```

### Controls (Điều khiển)
- **Mouse Movement**: Di chuyển chim lên/xuống
- **ESC**: Thoát game

---

## 📖 CHI TIẾT CÁC THUẬT TOÁN ĐỒ HỌA

---

## 1️⃣ XÉN HÌNH (CLIPPING) - 6/10

### 🎓 Lý thuyết

**Xén hình** là quá trình loại bỏ các phần hình học nằm ngoài vùng hiển thị (view volume/frustum) để tối ưu hiệu năng rendering.

#### **Cohen-Sutherland Line Clipping Algorithm**

Thuật toán xén đoạn thẳng 2D sử dụng mã hóa vùng:

```
Bước 1: Chia không gian thành 9 vùng bằng 4 bit:
  - Bit 0: TOP (y > y_max)
  - Bit 1: BOTTOM (y < y_min)
  - Bit 2: RIGHT (x > x_max)
  - Bit 3: LEFT (x < x_min)

Bước 2: Tính mã cho 2 điểm đầu/cuối đoạn thẳng

Bước 3: Kiểm tra:
  - Nếu code1 | code2 == 0 → Hoàn toàn trong → Giữ nguyên
  - Nếu code1 & code2 != 0 → Hoàn toàn ngoài → Loại bỏ
  - Ngược lại → Tính giao điểm với biên → Xén

Bước 4: Lặp lại cho đến khi hoàn toàn trong hoặc ngoài
```

**Độ phức tạp**: O(1) cho mỗi đoạn thẳng

#### **Sutherland-Hodgman Polygon Clipping**

Thuật toán xén đa giác bằng cách xén tuần tự theo từng cạnh của view window:

```
for each edge of clipping window:
    newPolygon = empty
    for each edge of polygon:
        p1, p2 = edge endpoints
        
        if p1 inside && p2 inside:
            add p2 to newPolygon
        elif p1 inside && p2 outside:
            add intersection to newPolygon
        elif p1 outside && p2 inside:
            add intersection and p2 to newPolygon
        elif p1 outside && p2 outside:
            do nothing
    
    polygon = newPolygon
```

**Độ phức tạp**: O(n) với n là số đỉnh

#### **3D View Frustum Culling**

Trong 3D graphics, view frustum là hình chóp cụt được định nghĩa bởi 6 mặt phẳng:
- **Near plane**: Mặt phẳng gần
- **Far plane**: Mặt phẳng xa
- **Left, Right, Top, Bottom**: 4 mặt phẳng bên

```cpp
// Frustum culling test cho sphere
bool isInFrustum(vec3 center, float radius) {
    for (int i = 0; i < 6; i++) {
        float distance = dot(planes[i].normal, center) + planes[i].d;
        if (distance < -radius)
            return false; // Outside frustum
    }
    return true; // Inside or intersecting
}
```

### 💻 Cài đặt trong dự án

#### **File: `src/entities/gameObjects/Camera.cc`** (dòng 41-47)

```cpp
glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(
        glm::radians(getFov()),                    // FOV = 60°
        (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT,
        NEAR_PLANE,   // 1.0f - Xén vật thể quá gần
        FAR_PLANE     // 1000.0f - Xén vật thể quá xa
    );
}
```

**Giải thích**:
- `NEAR_PLANE`: Tất cả vật thể có z < 1.0 bị xén (quá gần camera)
- `FAR_PLANE`: Tất cả vật thể có z > 1000.0 bị xén (quá xa camera)
- OpenGL tự động xén các vertex ngoài frustum

#### **File: `shaders/entity.vert`** (dòng 20-25)

```glsl
void main() {
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    vec4 viewPos = viewMatrix * worldPos;
    gl_Position = projectionMatrix * viewPos;
    
    // OpenGL tự động xén các đỉnh có:
    // gl_Position.x/w ngoài [-1, 1]
    // gl_Position.y/w ngoài [-1, 1]
    // gl_Position.z/w ngoài [-1, 1] (NDC - Normalized Device Coordinates)
}
```

#### **File: `src/entities/gameObjects/HeartHolder.cc`** (dòng 65-72)

```cpp
void HeartHolder::update() {
    // Object culling - loại bỏ vật thể bay ra khỏi màn hình
    for (auto it = hearts.begin(); it != hearts.end();) {
        if ((*it)->getZ() > Camera::theOne()->getZ() + 50) {
            delete *it;
            it = hearts.erase(it);  // Xén vật thể ngoài tầm nhìn
        } else {
            ++it;
        }
    }
}
```

**Giải thích**: Frustum culling cấp cao - loại bỏ toàn bộ object thay vì chỉ xén polygon

### 🎯 Ứng dụng
- ✅ Giảm số lượng polygon cần render (tăng FPS)
- ✅ Tránh render vật thể không nhìn thấy
- ✅ Tối ưu bandwidth GPU

---

## 2️⃣ TÔ MÀU (FLOOD FILL) - 6/10

### 🎓 Lý thuyết

**Flood Fill** là thuật toán tô màu vùng kín, thường dùng trong paint programs và texture filling.

#### **Recursive Flood Fill (4-connected)**

```cpp
void floodFill(int x, int y, Color fillColor, Color boundaryColor) {
    Color currentColor = getPixel(x, y);
    
    // Base cases
    if (currentColor == boundaryColor) return;
    if (currentColor == fillColor) return;
    
    // Fill current pixel
    setPixel(x, y, fillColor);
    
    // Recursive fill 4 neighbors
    floodFill(x+1, y, fillColor, boundaryColor);  // Right
    floodFill(x-1, y, fillColor, boundaryColor);  // Left
    floodFill(x, y+1, fillColor, boundaryColor);  // Up
    floodFill(x, y-1, fillColor, boundaryColor);  // Down
}
```

**Nhược điểm**: Stack overflow với vùng lớn

#### **Queue-based Flood Fill (Iterative)**

```cpp
void floodFillIterative(int x, int y, Color fillColor) {
    Queue<Point> queue;
    queue.push({x, y});
    
    while (!queue.empty()) {
        Point p = queue.pop();
        
        if (getPixel(p.x, p.y) != fillColor) {
            setPixel(p.x, p.y, fillColor);
            
            // Add 4 neighbors to queue
            queue.push({p.x+1, p.y});
            queue.push({p.x-1, p.y});
            queue.push({p.x, p.y+1});
            queue.push({p.x, p.y-1});
        }
    }
}
```

**Độ phức tạp**: O(n×m) với n, m là kích thước vùng

#### **Scanline Flood Fill (Tối ưu nhất)**

```cpp
void scanlineFill(int x, int y, Color fillColor) {
    Stack<Span> stack;
    stack.push({x, x, y, 1});  // {x1, x2, y, direction}
    
    while (!stack.empty()) {
        Span span = stack.pop();
        
        // Tô màu scanline ngang
        for (int i = span.x1; i <= span.x2; i++) {
            setPixel(i, span.y, fillColor);
        }
        
        // Tìm span ở dòng trên/dưới
        // ... (phức tạp hơn nhưng hiệu quả)
    }
}
```

**Ưu điểm**: Giảm số lần push/pop, tối ưu cache locality

### 💻 Cài đặt trong dự án

#### **File: `src/textures/Texture.cc`** (dòng 35-45)

```cpp
void Texture::fillColor(GLubyte r, GLubyte g, GLubyte b) {
    // Flood fill toàn bộ texture buffer
    for (int i = 0; i < width * height * 4; i += 4) {
        pixels[i]   = r;    // Red channel
        pixels[i+1] = g;    // Green channel
        pixels[i+2] = b;    // Blue channel
        pixels[i+3] = 255;  // Alpha channel
    }
    // Upload to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
```

#### **File: `shaders/entity.frag`** (dòng 65-75) - GPU Flood Fill

```glsl
void main() {
    // Phong lighting calculation
    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = specularStrength * specularFactor * lightColor;
    
    // Flood fill fragment với màu tính toán
    vec3 fragColor = (ambient + diffuse + specular) * color;
    
    // Fog blending
    vec3 finalColor = mix(fogColor, fragColor, fogFactor);
    colorTexture = vec4(finalColor, 1.0);
}
```

**Giải thích**: 
- GPU thực hiện "flood fill" song song cho hàng triệu fragments
- Mỗi fragment shader = 1 pixel được tô màu
- Hardware rasterization = scanline flood fill tối ưu

#### **Rasterization Pipeline**

```
Vertex Shader → Triangle Assembly → Rasterization → Fragment Shader
                                         ↓
                                   Flood Fill từng pixel
                                   trong tam giác
```

### 🎯 Ứng dụng
- ✅ Tô màu bề mặt 3D models
- ✅ Texture generation
- ✅ GPU rasterization (hardware acceleration)
- ✅ Paint tools trong game editors

---

## 3️⃣ BIẾN ĐỔI 2D (2D TRANSFORMATIONS) - 13/10

### 🎓 Lý thuyết

Các phép biến đổi 2D cơ bản sử dụng **ma trận đồng nhất (homogeneous coordinates)** 3×3:

#### **Translation (Tịnh tiến)**

```
T(tx, ty) = | 1  0  tx |
            | 0  1  ty |
            | 0  0  1  |

P' = T × P
| x' |   | 1  0  tx |   | x |   | x + tx |
| y' | = | 0  1  ty | × | y | = | y + ty |
| 1  |   | 0  0  1  |   | 1 |   |   1    |
```

#### **Rotation (Quay quanh gốc tọa độ)**

```
R(θ) = | cos(θ)  -sin(θ)  0 |
       | sin(θ)   cos(θ)  0 |
       |   0        0     1 |

// Quay ngược chiều kim đồng hồ
x' = x×cos(θ) - y×sin(θ)
y' = x×sin(θ) + y×cos(θ)
```

#### **Scaling (Co giãn)**

```
S(sx, sy) = | sx  0   0 |
            | 0   sy  0 |
            | 0   0   1 |

x' = x × sx
y' = y × sy

// Trường hợp đặc biệt:
// sx = sy: Uniform scaling (co/giãn đều)
// sx ≠ sy: Non-uniform scaling
// sx < 0 hoặc sy < 0: Reflection (đối xứng)
```

#### **Shearing (Biến dạng)**

```
// Shear theo X (dọc theo trục X)
Shx(sh) = | 1   sh  0 |
          | 0   1   0 |
          | 0   0   1 |

x' = x + y×sh
y' = y

// Shear theo Y
Shy(sh) = | 1   0   0 |
          | sh  1   0 |
          | 0   0   1 |

x' = x
y' = y + x×sh
```

### 💻 Cài đặt trong dự án

#### **File: `shaders/ui.vert`** - 2D UI Transformation

```glsl
#version 330 core
layout(location = 0) in vec2 position;

uniform vec2 offset;     // Translation
uniform vec2 scale;      // Scaling
uniform float rotation;  // Rotation angle

void main() {
    vec2 pos = position;
    
    // 1. Scaling
    pos *= scale;
    
    // 2. Rotation (2D rotation matrix)
    float c = cos(rotation);
    float s = sin(rotation);
    vec2 rotated = vec2(
        pos.x * c - pos.y * s,
        pos.x * s + pos.y * c
    );
    
    // 3. Translation
    rotated += offset;
    
    gl_Position = vec4(rotated, 0.0, 1.0);
}
```

#### **File: `src/entities/gameObjects/Bird.cc`** (dòng 180-195) - Wing Rotation

```cpp
void Bird::updateWings() {
    float time = glfwGetTime();
    float wingSpeed = 5.0f;
    float wingAmplitude = glm::radians(30.0f); // ±30 degrees
    
    // Rotation 2D cho cánh (quay quanh trục Z)
    float angle = sin(time * wingSpeed) * wingAmplitude;
    
    // Left wing: rotation matrix R(+angle)
    leftWing->setRotation(0, 0, angle);
    
    // Right wing: rotation matrix R(-angle) - đối xứng
    rightWing->setRotation(0, 0, -angle);
}
```

**Giải thích**:
- Sine wave tạo chuyển động vỗ cánh mượt mà
- `angle ∈ [-30°, +30°]` giới hạn góc vỗ
- 2 cánh đối xứng qua mặt phẳng giữa

#### **File: `src/models/Geometry.cc`** (dòng 45-65) - UV Texture Mapping

```cpp
RawModel Geometry::createSphere(float radius, int slices, int stacks) {
    std::vector<float> vertices, normals, texCoords;
    
    for (int i = 0; i <= stacks; ++i) {
        for (int j = 0; j <= slices; ++j) {
            // 2D Transformation: Scaling + Translation
            float u = (float)j / slices;        // Scale [0, slices] → [0, 1]
            float v = (float)i / stacks;        // Scale [0, stacks] → [0, 1]
            
            texCoords.push_back(u);
            texCoords.push_back(v);
            // ...
        }
    }
}
```

### 🎯 Ứng dụng
- ✅ Cánh chim vỗ (rotation animation)
- ✅ UI element positioning (translation + scaling)
- ✅ Texture coordinate mapping
- ✅ Particle system transformations

---

## 4️⃣ KẾT HỢP CÁC PHÉP BIẾN ĐỔI 2D - 13/10

### 🎓 Lý thuyết

Kết hợp nhiều phép biến đổi bằng **nhân ma trận**. Thứ tự nhân **QUAN TRỌNG** vì ma trận không giao hoán (A×B ≠ B×A).

#### **Composite Transformation**

```
M_combined = M_n × M_(n-1) × ... × M_2 × M_1

// Áp dụng từ phải sang trái:
P' = M_combined × P = M_n × (M_(n-1) × (... × (M_1 × P)))
```

#### **Thứ tự chuẩn: SRT (Scale → Rotate → Translate)**

```
M = T × R × S

1. Scale trước (co/giãn tại gốc tọa độ)
2. Rotate tiếp (quay tại gốc tọa độ)
3. Translate cuối (di chuyển đến vị trí cuối cùng)
```

**Ví dụ**: Quay hình vuông quanh tâm của nó

```
Bước 1: Translate về gốc tọa độ      T(-cx, -cy)
Bước 2: Rotate quanh gốc               R(θ)
Bước 3: Translate về vị trí cũ        T(cx, cy)

M = T(cx, cy) × R(θ) × T(-cx, -cy)
```

#### **Ma trận MVP (Model-View-Projection)**

Trong 3D graphics, đây là composite transformation quan trọng nhất:

```
gl_Position = P × V × M × vertex

P: Projection matrix (3D → 2D projection)
V: View matrix (World space → Camera space)
M: Model matrix (Object space → World space)

// Kết hợp thành 1 ma trận duy nhất:
MVP = P × V × M
gl_Position = MVP × vertex  // Chỉ 1 phép nhân ma trận!
```

### 💻 Cài đặt trong dự án

#### **File: `src/maths/Object3D.cc`** (dòng 25-45) - Model Matrix (SRT)

```cpp
glm::mat4 Object3D::getTransformationMatrix() {
    glm::mat4 matrix = glm::mat4(1.0f);  // Identity matrix
    
    // 1. TRANSLATE (T)
    matrix = glm::translate(matrix, position);
    
    // 2. ROTATE (R) - Euler angles (Yaw-Pitch-Roll)
    matrix = glm::rotate(matrix, glm::radians(rotation.y), 
                         glm::vec3(0, 1, 0));  // Yaw (Y-axis)
    matrix = glm::rotate(matrix, glm::radians(rotation.x), 
                         glm::vec3(1, 0, 0));  // Pitch (X-axis)
    matrix = glm::rotate(matrix, glm::radians(rotation.z), 
                         glm::vec3(0, 0, 1));  // Roll (Z-axis)
    
    // 3. SCALE (S)
    matrix = glm::scale(matrix, scale);
    
    return matrix;  // M = T × R_y × R_x × R_z × S
}
```

**Lưu ý**: Thứ tự thực tế là **T×R×S** (đọc từ trên xuống), nhưng do GLM dùng post-multiplication nên viết ngược.

#### **File: `src/entities/gameObjects/Camera.cc`** (dòng 45-47) - View Matrix

```cpp
glm::mat4 Camera::getViewMatrix() {
    // View matrix = Inverse của camera transformation
    // V = [R^T | -R^T × position]
    return glm::lookAt(
        position,           // Camera position
        position + front,   // Look-at point
        up                  // Up vector
    );
}
```

**Công thức LookAt**:

```
forward = normalize(target - eye)
right = normalize(cross(forward, worldUp))
up = cross(right, forward)

View Matrix = | right.x    right.y    right.z    -dot(right, eye)   |
              | up.x       up.y       up.z       -dot(up, eye)      |
              | -forward.x -forward.y -forward.z  dot(forward, eye) |
              | 0          0          0           1                 |
```

#### **File: `shaders/entity.vert`** (dòng 18-28) - MVP Combination

```glsl
uniform mat4 projectionMatrix;  // P
uniform mat4 viewMatrix;        // V
uniform mat4 modelMatrix;       // M

void main() {
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    vec4 viewPos = viewMatrix * worldPos;
    gl_Position = projectionMatrix * viewPos;
    
    // Tương đương:
    // gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    // gl_Position = (P × V × M) × vertex
}
```

**Tối ưu**: CPU tính `MVP = P×V×M` một lần, GPU chỉ nhân `MVP×vertex` cho mỗi đỉnh.

### 🎯 Ứng dụng
- ✅ Mỗi entity có position, rotation, scale riêng
- ✅ Camera transformation (inverse model matrix)
- ✅ Hierarchical transformations (parent-child objects)
- ✅ Animation blending

---

## 5️⃣ BIỂU DIỄN ĐỐI TƯỢNG 3D - 20/10

### 🎓 Lý thuyết

Có nhiều cách biểu diễn đối tượng 3D trong computer graphics:

#### **Polygon Mesh (Lưới đa giác)**

Cách phổ biến nhất: biểu diễn bề mặt bằng tập hợp các tam giác.

```
Vertex: (x, y, z, nx, ny, nz, u, v)
  - Position: (x, y, z)
  - Normal: (nx, ny, nz)  
  - Texture coords: (u, v)

Face: (v1, v2, v3) - 3 vertex indices

Mesh = {Vertices[], Faces[]}
```

**Ưu điểm**:
- Hardware acceleration (GPU rất nhanh với triangles)
- Dễ transform, lighting, texturing
- Flexible topology

**Nhược điểm**:
- Nhiều vertex cho bề mặt cong
- File size lớn

#### **Parametric Surfaces (Mặt tham số)**

Biểu diễn bề mặt bằng phương trình toán học:

```
S(u, v) = (x(u, v), y(u, v), z(u, v))
với u, v ∈ [0, 1]

Ví dụ - Sphere:
x = r × sin(πu) × cos(2πv)
y = r × cos(πu)
z = r × sin(πu) × sin(2πv)

Heart curve (2D → 3D extrusion):
x = 16 × sin³(t)
y = 13×cos(t) - 5×cos(2t) - 2×cos(3t) - cos(4t)
z = depth
```

**Ưu điểm**:
- Compact representation
- Infinite resolution
- Easy to modify parameters

#### **Constructive Solid Geometry (CSG)**

Kết hợp các primitive shapes bằng boolean operations:

```
Union: A ∪ B
Intersection: A ∩ B  
Difference: A \ B

Example:
Cylinder - Sphere = Hollow cylinder
Cube ∩ Sphere = Rounded cube
```

#### **Vertex Array Object (VAO) & Vertex Buffer Object (VBO)**

Cách OpenGL lưu trữ mesh data:

```cpp
// 1. Tạo VAO
GLuint vao;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);

// 2. Tạo VBO cho vertices
GLuint vbo;
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 3. Định nghĩa vertex attributes
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);  // Position
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));  // Normal
glEnableVertexAttribArray(1);
```

### 💻 Cài đặt trong dự án

#### **File: `src/models/RawModel.h`** - Mesh Data Structure

```cpp
class RawModel {
private:
    GLuint vaoID;        // Vertex Array Object ID
    GLuint vboID;        // Vertex Buffer Object ID
    int vertexCount;     // Số lượng đỉnh
    
public:
    RawModel(GLuint vao, GLuint vbo, int count);
    void render();  // glDrawArrays(GL_TRIANGLES, 0, vertexCount)
};
```

#### **File: `src/models/Geometry.cc`** (dòng 250-290) - Parametric Heart

```cpp
RawModel Geometry::createHeart(float scale) {
    std::vector<float> vertices, normals;
    float step = 0.05f;  // Tessellation step
    
    // Parametric heart curve (2D)
    for (float t = 0; t < 2 * M_PI; t += step) {
        // Classic heart equation
        float x = scale * 16 * pow(sin(t), 3);
        float y = scale * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));
        
        // Extrude to 3D (add depth)
        for (float d = -0.5f; d <= 0.5f; d += 0.1f) {
            float z = d * scale;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Calculate normal vector (derivative of curve)
            float dx_dt = scale * 48 * pow(sin(t), 2) * cos(t);
            float dy_dt = scale * (-13*sin(t) + 10*sin(2*t) + 6*sin(3*t) + 4*sin(4*t));
            
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

#### **File: `src/models/Geometry.cc`** (dòng 100-160) - Parametric Sphere

```cpp
RawModel Geometry::createSphere(float radius, int slices, int stacks) {
    std::vector<float> vertices, normals;
    
    // Parametric sphere using spherical coordinates
    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;  // Polar angle [0, π]
        
        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;  // Azimuthal angle [0, 2π]
            
            // Parametric equations
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // For sphere, normal = normalize(position - center)
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
    
    // Generate triangles using index buffer
    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;
            
            // Triangle 1
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            // Triangle 2
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    return Loader::loadToVAO(vertices, normals, indices);
}
```

#### **File: `src/models/Geometry.cc`** (dòng 200-240) - Polygon Mesh Tetrahedron

```cpp
RawModel Geometry::createTetrahedron() {
    // 4 vertices of regular tetrahedron
    glm::vec3 v0(0, 1, 0);           // Top
    glm::vec3 v1(-1, -1, 1);         // Front-left
    glm::vec3 v2(1, -1, 1);          // Front-right
    glm::vec3 v3(0, -1, -1);         // Back
    
    std::vector<float> vertices, normals;
    
    // Helper lambda to add triangle
    auto addTriangle = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        // Calculate face normal
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        
        // Add 3 vertices
        for (glm::vec3 v : {a, b, c}) {
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    };
    
    // 4 faces (triangles)
    addTriangle(v0, v1, v2);  // Front
    addTriangle(v0, v2, v3);  // Right
    addTriangle(v0, v3, v1);  // Left
    addTriangle(v1, v3, v2);  // Bottom
    
    return Loader::loadToVAO(vertices, normals);
}
```

#### **File: `src/entities/gameObjects/Bird.cc`** (dòng 50-130) - Composite Object

```cpp
void Bird::createModel() {
    // Composite object = nhiều primitive shapes
    
    // Body (sphere)
    body = new Entity(Geometry::sphere(2.5f, 20, 20), BROWN);
    body->setPosition(0, 0, 0);
    
    // Head (smaller sphere)
    head = new Entity(Geometry::sphere(1.5f, 20, 20), BROWN);
    head->setPosition(0, 2.0f, 1.5f);
    
    // Beak (cone/cockpit shape)
    beak = new Entity(Geometry::cockpit(0.8f, 1.5f), ORANGE);
    beak->setPosition(0, 2.0f, 2.5f);
    
    // Wings (boxes)
    leftWing = new Entity(Geometry::box(0.5f, 2.0f, 3.0f), BROWNDARK);
    leftWing->setPosition(-2.0f, 0, 0);
    
    rightWing = new Entity(Geometry::box(0.5f, 2.0f, 3.0f), BROWNDARK);
    rightWing->setPosition(2.0f, 0, 0);
    
    // Tail (box)
    tail = new Entity(Geometry::box(0.3f, 2.5f, 1.5f), BROWNDARK);
    tail->setPosition(0, 0, -2.5f);
}
```

**Hierarchical transformation**: Tất cả parts di chuyển cùng bird

### 🎯 Ứng dụng
- ✅ Heart collectibles (parametric curve)
- ✅ Triangle obstacles (polygon mesh)
- ✅ Bird model (composite object)
- ✅ Terrain (procedural mesh generation)

---

## 6️⃣ QUAN SÁT ĐỐI TƯỢNG 3D (3D VIEWING) - 27/10

### 🎓 Lý thuyết

Quá trình chuyển từ 3D world sang 2D screen qua **Graphics Pipeline**:

```
Object Space → World Space → View Space → Clip Space → NDC → Screen Space
     M             V              P          ÷w        Viewport
```

#### **View Matrix (Camera Transformation)**

**LookAt Algorithm**:

```
Cho:
- eye: Vị trí camera
- target: Điểm nhìn
- worldUp: Vector "lên" của thế giới (thường là (0, 1, 0))

Tính:
1. forward = normalize(target - eye)
2. right = normalize(cross(forward, worldUp))
3. up = cross(right, forward)

View Matrix:
V = | right.x    right.y    right.z    -dot(right, eye)   |
    | up.x       up.y       up.z       -dot(up, eye)      |
    | -forward.x -forward.y -forward.z  dot(forward, eye) |
    | 0          0          0           1                 |
```

**Giải thích**: View matrix là **inverse** của camera transformation matrix

#### **Projection Matrix**

**Perspective Projection** (phối cảnh):

```
Tạo cảm giác "gần to xa nhỏ"

Parameters:
- fov: Field of view (góc nhìn), thường 45°-90°
- aspect: Tỷ lệ width/height
- near: Mặt phẳng gần
- far: Mặt phẳng xa

f = cotangent(fov/2)

P = | f/aspect   0      0                      0                    |
    | 0          f      0                      0                    |
    | 0          0      (far+near)/(near-far)  2×far×near/(near-far)|
    | 0          0      -1                     0                    |

Sau khi nhân: (x', y', z', w')
x_ndc = x'/w'  // Perspective divide
y_ndc = y'/w'
z_ndc = z'/w'
```

**Orthographic Projection** (trực giao):

```
Không có perspective, dùng cho CAD, strategy games

P = | 2/(r-l)   0         0        -(r+l)/(r-l) |
    | 0         2/(t-b)   0        -(t+b)/(t-b) |
    | 0         0        -2/(f-n)  -(f+n)/(f-n) |
    | 0         0         0         1           |

l, r, b, t, n, f: left, right, bottom, top, near, far
```

### 💻 Cài đặt trong dự án

#### **File: `src/entities/gameObjects/Camera.cc`** (dòng 45-52)

```cpp
glm::mat4 Camera::getViewMatrix() {
    // GLM implementation of LookAt
    return glm::lookAt(
        position,           // eye
        position + front,   // target = eye + direction
        up                  // worldUp
    );
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(
        glm::radians(getFov()),                    // fov = 60° → radians
        (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT, // aspect ratio
        NEAR_PLANE,                                 // near = 1.0f
        FAR_PLANE                                   // far = 1000.0f
    );
}
```

#### **File: `src/entities/gameObjects/Camera.cc`** (dòng 80-105) - Chase Camera

```cpp
void Camera::update() {
    Bird* bird = Bird::theOne();
    
    // Smooth follow camera (lerp)
    glm::vec3 targetPos = bird->getPosition() + glm::vec3(0, 5, 20);
    position = glm::mix(position, targetPos, 0.1f);  // Linear interpolation
    
    // LookAt bird
    glm::vec3 direction = glm::normalize(bird->getPosition() - position);
    front = direction;
    
    // Update up vector (maintain perpendicular to forward)
    right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    up = glm::cross(right, front);
}
```

**Giải thích**:
- `glm::mix(a, b, t)` = lerp = a + t×(b - a)
- Smooth following tránh camera giật cục
- Always look at bird

#### **File: `shaders/entity.vert`** (dòng 15-30)

```glsl
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

void main() {
    // Transform to world space
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    FragPos = worldPos.xyz;
    
    // Transform to view space
    vec4 viewPos = viewMatrix * worldPos;
    
    // Transform to clip space (with perspective)
    gl_Position = projectionMatrix * viewPos;
    
    // Transform normal (use normal matrix to handle non-uniform scaling)
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
}
```

**Pipeline steps**:
1. Model matrix: Object space → World space
2. View matrix: World space → Camera space  
3. Projection matrix: Camera space → Clip space (homogeneous coords)
4. Perspective divide (tự động): Clip space → NDC
5. Viewport transform (tự động): NDC → Screen coords

### 🎯 Ứng dụng
- ✅ Perspective projection (realistic 3D)
- ✅ Chase camera following bird
- ✅ FOV 60° for wide viewing angle
- ✅ Near/far plane clipping

---

## 7️⃣ VẼ ĐƯỜNG/MẶT CONG (CURVE DRAWING) - 10/11

### 🎓 Lý thuyết

#### **Parametric Curves**

Đường cong được định nghĩa bởi hàm tham số:

```
C(t) = (x(t), y(t), z(t)),  t ∈ [a, b]

Example - Circle:
x(t) = r × cos(t)
y(t) = r × sin(t)
t ∈ [0, 2π]
```

#### **Bézier Curves**

Đường cong được điều khiển bởi control points:

**Linear Bézier** (2 points):
```
B(t) = (1-t)×P₀ + t×P₁,  t ∈ [0, 1]
```

**Quadratic Bézier** (3 points):
```
B(t) = (1-t)²×P₀ + 2(1-t)t×P₁ + t²×P₂
```

**Cubic Bézier** (4 points):
```
B(t) = (1-t)³×P₀ + 3(1-t)²t×P₁ + 3(1-t)t²×P₂ + t³×P₃

Derivative (tangent):
B'(t) = 3(1-t)²(P₁-P₀) + 6(1-t)t(P₂-P₁) + 3t²(P₃-P₂)
```

#### **B-Splines**

Smooth curve through multiple control points với local control:

```
Basis function: N_{i,k}(t)

Curve: C(t) = Σ P_i × N_{i,k}(t)

Properties:
- Local control (move 1 point affects nearby only)
- C² continuity (smooth)
- Does not pass through control points (except endpoints)
```

#### **Tessellation (Chia nhỏ)**

Chuyển đường cong parametric thành line segments:

```cpp
std::vector<vec3> tessellate(Function curve, float tMin, float tMax, float step) {
    std::vector<vec3> points;
    for (float t = tMin; t <= tMax; t += step) {
        points.push_back(curve(t));
    }
    return points;
}

// Adaptive tessellation (denser where curvature is high)
float adaptiveStep(float curvature) {
    return 1.0 / (1.0 + curvature);  // Smaller step for high curvature
}
```

### 💻 Cài đặt trong dự án

#### **File: `src/models/Geometry.cc`** (dòng 250-300) - Heart Curve

```cpp
RawModel Geometry::createHeart(float scale) {
    std::vector<float> vertices, normals;
    float step = 0.05f;  // Tessellation resolution
    
    // 2D Parametric heart equation
    auto heartX = [](float t, float s) {
        return s * 16 * pow(sin(t), 3);
    };
    
    auto heartY = [](float t, float s) {
        return s * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));
    };
    
    // Tessellate curve
    for (float t = 0; t < 2 * M_PI; t += step) {
        float x = heartX(t, scale);
        float y = heartY(t, scale);
        
        // Extrude to 3D
        for (float depth = -0.5f; depth <= 0.5f; depth += 0.1f) {
            float z = depth * scale;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Tangent vector (derivative)
            float dx_dt = scale * 48 * pow(sin(t), 2) * cos(t);
            float dy_dt = scale * (-13*sin(t) + 10*sin(2*t) + 
                                    6*sin(3*t) + 4*sin(4*t));
            
            // Normal = perpendicular to tangent (2D)
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

**Công thức toán học**:

```
Heart curve (Cartesian):
(x² + y² - 1)³ - x²y³ = 0

Parametric form (easier to tessellate):
x(t) = 16×sin³(t)
y(t) = 13×cos(t) - 5×cos(2t) - 2×cos(3t) - cos(4t)

Derivative (for normal calculation):
dx/dt = 48×sin²(t)×cos(t)
dy/dt = -13×sin(t) + 10×sin(2t) + 6×sin(3t) + 4×sin(4t)
```

#### **File: `src/entities/gameObjects/Bird.cc`** (dòng 180-195) - Sine Wave Animation

```cpp
void Bird::updateWings() {
    float time = glfwGetTime();
    
    // Sine wave curve for smooth flapping
    float wingSpeed = 5.0f;
    float wingAmplitude = glm::radians(30.0f);
    
    // Parametric curve: angle(t) = A×sin(ωt)
    float angle = wingAmplitude * sin(wingSpeed * time);
    
    // Apply rotation
    leftWing->setRotationZ(angle);
    rightWing->setRotationZ(-angle);  // Mirror
}
```

**Animation curve**:
```
angle(t) = 30° × sin(5t)
Period T = 2π/5 ≈ 1.26 seconds
Frequency f = 5/(2π) ≈ 0.8 Hz
```

#### **File: `src/models/Geometry.cc`** (dòng 180-210) - Terrain Wave

```cpp
RawModel Geometry::createSea(float width, float depth, int segments) {
    std::vector<float> vertices, normals;
    
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            float x = (i / (float)segments - 0.5f) * width;
            float z = (j / (float)segments - 0.5f) * depth;
            
            // 2D sine wave for terrain height
            float y = 0.5f * sin(x * 0.5f) * cos(z * 0.5f);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Approximate normal using partial derivatives
            float dydx = 0.25f * cos(x * 0.5f) * cos(z * 0.5f);
            float dydz = -0.25f * sin(x * 0.5f) * sin(z * 0.5f);
            
            glm::vec3 normal = glm::normalize(glm::vec3(-dydx, 1, -dydz));
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
    
    return Loader::loadToVAO(vertices, normals);
}
```

### 🎯 Ứng dụng
- ✅ Heart shape (parametric curve with extrusion)
- ✅ Wing flapping (sine wave animation)
- ✅ Terrain waves (2D sine function)
- ✅ Sphere/cylinder (parametric surfaces)

---

## 8️⃣ KHỬ KHUẤT (HIDDEN SURFACE REMOVAL) - 17/11

### 🎓 Lý thuyết

Loại bỏ bề mặt bị che khuất để tránh render vật phía sau.

#### **Z-Buffer Algorithm (Depth Buffer)**

Thuật toán phổ biến nhất, được GPU hardware hỗ trợ:

```
Initialization:
for each pixel (x, y):
    depth_buffer[x][y] = ∞
    color_buffer[x][y] = background

Rendering:
for each triangle:
    for each pixel (x, y) in triangle:
        z = interpolate depth at (x, y)
        
        if z < depth_buffer[x][y]:  // Closer to camera
            depth_buffer[x][y] = z
            color_buffer[x][y] = shaded_color
```

**Độ phức tạp**: O(n×p) với n = triangles, p = pixels per triangle

**Ưu điểm**:
- Simple, fast
- Hardware accelerated
- Works with any polygon order

**Nhược điểm**:
- Memory cost (depth buffer = screen size)
- Transparency issues

#### **Painter's Algorithm**

Render từ xa đến gần (back-to-front):

```
1. Sort all polygons by depth (distance from camera)
2. Render from farthest to nearest
3. Nearer polygons paint over farther ones
```

**Nhược điểm**:
- Cannot handle overlapping cycles (A behind B, B behind C, C behind A)
- Expensive sorting O(n log n)
- Not hardware accelerated

#### **Back-Face Culling**

Loại bỏ mặt quay mặt ra ngoài (không nhìn thấy):

```cpp
// Dot product test
vec3 normal = cross(v1 - v0, v2 - v0);
vec3 viewDir = normalize(cameraPos - v0);

if (dot(normal, viewDir) < 0) {
    // Back-facing → cull (don't render)
}
```

**Đối với mesh kín**: Loại bỏ ~50% polygons!

#### **BSP Tree (Binary Space Partitioning)**

Phân chia không gian thành tree structure:

```
Build phase:
1. Choose splitting plane
2. Classify polygons: front, back, or split
3. Recursively build left and right subtrees

Render phase:
Traverse tree in back-to-front order relative to camera
```

**Ưu điểm**: Render order luôn đúng, không cần sort
**Nhược điểm**: Expensive pre-processing, static scenes only

### 💻 Cài đặt trong dự án

#### **File: `src/renderEngine/DisplayManager.cc`** (dòng 50-65)

```cpp
void DisplayManager::init() {
    // Enable depth testing (Z-buffer)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // Pass if incoming depth < current depth
    
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);   // Cull back faces
    glFrontFace(GL_CCW);   // Counter-clockwise = front face
    
    // Depth buffer precision
    glDepthRange(0.0, 1.0);  // Map [near, far] to [0, 1]
}
```

#### **File: `src/renderEngine/Renderer.cc`** (dòng 20-30)

```cpp
void Renderer::render() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //      ↑ Reset color to background
    //                              ↑ Reset depth to 1.0 (far)
    
    // Render all entities
    entityShader.render();  // GPU performs Z-test automatically
    seaShader.render();
    uiShader.render();
}
```

#### **File: `shaders/entity.frag`** - Fragment Shader

```glsl
void main() {
    // Calculate fragment color
    vec3 fragColor = (ambient + diffuse + specular) * color;
    colorTexture = vec4(fragColor, 1.0);
    
    // GPU automatically writes gl_FragDepth to depth buffer
    // gl_FragDepth = gl_FragCoord.z (automatic)
    
    // Depth test happens after fragment shader:
    // if (gl_FragDepth < depthBuffer[x][y]) {
    //     depthBuffer[x][y] = gl_FragDepth;
    //     colorBuffer[x][y] = colorTexture;
    // }
}
```

#### **File: `src/entities/gameObjects/HeartHolder.cc`** - Frustum Culling

```cpp
void HeartHolder::update() {
    for (auto it = hearts.begin(); it != hearts.end();) {
        // Frustum culling: remove objects outside view
        if ((*it)->getZ() > Camera::theOne()->getZ() + 50) {
            delete *it;
            it = hearts.erase(it);  // Don't even send to GPU
        } else {
            ++it;
        }
    }
}
```

**Hierarchy of culling**:
1. **Frustum culling** (CPU): Remove entire objects
2. **Back-face culling** (GPU): Remove back-facing triangles
3. **Z-buffer** (GPU): Remove occluded pixels

#### **Early Z-Test Optimization**

Modern GPUs perform depth test **before** fragment shader:

```
Vertex Shader → Rasterization → Early Z-Test → Fragment Shader → Late Z-Test
                                      ↓
                            Skip if depth test fails
                            (Huge performance gain!)
```

### 🎯 Ứng dụng
- ✅ Z-buffer removes hidden surfaces automatically
- ✅ Back-face culling saves ~50% rendering
- ✅ Frustum culling removes off-screen objects
- ✅ Early Z-test skips expensive fragment shading

---

## 9️⃣ TÔ BÓNG PHONG (PHONG SHADING) - 24/11

### 🎓 Lý thuyết

**Phong Reflection Model** mô phỏng cách ánh sáng tương tác với bề mặt:

```
I = I_ambient + I_diffuse + I_specular

I_ambient  = k_a × L_a
I_diffuse  = k_d × L_d × max(N·L, 0)
I_specular = k_s × L_s × max(R·V, 0)^shininess
```

Trong đó:
- `k_a, k_d, k_s`: Hệ số phản xạ (ambient, diffuse, specular)
- `L_a, L_d, L_s`: Cường độ ánh sáng
- `N`: Normal vector (pháp tuyến bề mặt)
- `L`: Light direction (hướng ánh sáng)
- `R`: Reflection vector
- `V`: View direction (hướng nhìn)
- `shininess`: Độ bóng (4-128)

#### **Ambient Lighting**

Ánh sáng môi trường (không phụ thuộc hướng):

```
I_ambient = k_a × L_a

// Không hướng, không đổ bóng
// Đảm bảo không có vùng hoàn toàn đen
```

#### **Diffuse Lighting (Lambert)**

Ánh sáng khuếch tán (mặt nhám):

```
I_diffuse = k_d × L_d × max(N·L, 0)

N·L = cos(θ)  // θ = góc giữa normal và light
// Sáng nhất khi θ = 0° (vuông góc)
// Tối khi θ ≥ 90° (ánh sáng từ phía sau)
```

#### **Specular Lighting (Phong)**

Ánh sáng phản chiếu (mặt bóng):

```
R = 2(N·L)N - L  // Reflection vector
I_specular = k_s × L_s × max(R·V, 0)^n

n: shininess parameter
  - n = 1: Mặt nhám (specular rộng)
  - n = 128: Mặt rất bóng (specular nhọn)
```

**Blinn-Phong** (tối ưu hơn):

```
H = normalize(L + V)  // Halfway vector
I_specular = k_s × L_s × max(N·H, 0)^n

// Nhanh hơn vì không cần tính R
```

#### **Gouraud vs Phong Shading**

| | Gouraud | Phong |
|---|---|---|
| **Tính toán** | Tại vertex | Tại mỗi pixel |
| **Nội suy** | Color | Normal vector |
| **Hiệu năng** | Nhanh hơn | Chậm hơn |
| **Chất lượng** | Specular highlights bị mất | Chính xác |
| **Mach banding** | Có thể xảy ra | Không |

### 💻 Cài đặt trong dự án

#### **File: `shaders/entity.frag`** (dòng 40-85) - Phong Shading

```glsl
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientLightIntensity;
uniform vec3 color;

in vec3 FragPos;    // World position (interpolated)
in vec3 Normal;     // Normal vector (interpolated)
in vec4 ViewSpace;  // View space position

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(FragPos - lightPos);  // Light direction
    
    // 1. AMBIENT
    float ambientStrength = 0.15 * ambientLightIntensity;
    vec3 ambient = ambientStrength * lightColor;
    
    // 2. DIFFUSE (Lambert)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 3. SPECULAR (Blinn-Phong)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(ViewSpace.xyz);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(norm, halfwayDir), 0.0), 32);  // shininess = 32
    vec3 specular = specularStrength * specularFactor * lightColor;
    
    // COMBINE
    vec3 fragColor = (ambient + diffuse + specular) * color;
    
    // FOG
    float dist = abs(ViewSpace.z);
    float fogFactor = clamp((FAR - dist) / (FAR - NEAR), 0.0, 1.0);
    vec3 finalColor = mix(fogColor, fragColor, fogFactor);
    
    colorTexture = vec4(finalColor, 1.0);
}
```

**Giải thích**:
- **Per-pixel lighting**: Phong shading tính toán cho MỖI fragment
- **Blinn-Phong**: Dùng halfway vector thay vì reflection (faster)
- **Shininess = 32**: Medium glossiness

#### **File: `shaders/entity.vert`** (dòng 15-35) - Normal Transformation

```glsl
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec4 ViewSpace;

void main() {
    // Transform position
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    FragPos = worldPos.xyz;
    
    ViewSpace = viewMatrix * worldPos;
    gl_Position = projectionMatrix * ViewSpace;
    
    // Transform normal (use normal matrix for non-uniform scaling)
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    Normal = normalMatrix * normal;
}
```

**Normal matrix**:
```
Normal transform ≠ Position transform

If model has non-uniform scaling:
normalMatrix = transpose(inverse(modelMatrix))

Reason: Normals are pseudo-vectors (perpendicular to surface)
```

#### **Comparison: Gouraud Implementation** (không dùng trong project)

```glsl
// Vertex Shader (Gouraud)
void main() {
    // Calculate lighting AT VERTEX
    vec3 norm = normalize(normalMatrix * normal);
    vec3 lightDir = normalize(lightPos - worldPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vertexColor = (ambient + diffuse) * color;  // Output color
    
    gl_Position = MVP * vec4(position, 1.0);
}

// Fragment Shader (Gouraud)
in vec3 vertexColor;  // Interpolated color from vertices

void main() {
    colorTexture = vec4(vertexColor, 1.0);  // Just use interpolated color
}
```

**Tại sao Phong tốt hơn**:
- Specular highlights chính xác (không bị mất)
- Smooth shading trên bề mặt cong
- Better for low-poly models

### 🎯 Ứng dụng
- ✅ Realistic lighting on bird, hearts, triangles
- ✅ Specular highlights on glossy surfaces
- ✅ Smooth shading with normal interpolation
- ✅ Per-pixel accuracy

---

---

**🎮 Chúc bạn chơi game vui vẻ và học tốt môn Đồ Họa Máy Tính!**

---

## 1️⃣ XÉN HÌNH (CLIPPING) - 6/10

### 📖 Lý thuyết thuật toán

**Xén hình** là quá trình loại bỏ các phần đối tượng nằm ngoài vùng hiển thị (view volume). Có nhiều thuật toán xén hình:

#### **Cohen-Sutherland Algorithm** (Xén đoạn thẳng 2D)
- Chia không gian thành 9 vùng bằng 4 bit mã (top, bottom, left, right)
- Kiểm tra nhanh: Nếu cả 2 điểm cùng vùng → giữ nguyên, cùng ngoài → loại bỏ
- Tính giao điểm với biên để xén

#### **Sutherland-Hodgman Algorithm** (Xén đa giác)
- Xén đa giác theo từng cạnh của view window
- Mỗi lần xén tạo ra đa giác mới

#### **3D Clipping** (View Frustum Culling)
- Xén theo 6 mặt phẳng của view frustum: near, far, left, right, top, bottom
- Sử dụng trong vertex shader với `gl_ClipDistance`

### 📍 Vị trí cài đặt trong project

#### **Frustum Culling tự động bởi OpenGL**
File: `shaders/entity.vert`, `shaders/sea.vert`

```glsl
// OpenGL tự động xén các đỉnh ngoài clip space [-1, 1]
gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
```

#### **Near/Far Plane Clipping**
File: `src/entities/gameObjects/Camera.cc` (dòng ~41-43)

```cpp
glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(
        glm::radians(getFov()),
        (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT,
        NEAR_PLANE,   // 1.0f - Xén vật thể quá gần
        FAR_PLANE     // 1000.0f - Xén vật thể quá xa
    );
}
```

#### **Object Culling (Loại bỏ vật thể ngoài màn hình)**
File: `src/entities/gameObjects/HeartHolder.cc` (dòng ~65-70)

```cpp
void HeartHolder::update() {
    // Xóa các trái tim bay ra khỏi màn hình
    for (auto it = hearts.begin(); it != hearts.end();) {
        if ((*it)->getZ() > Camera::theOne()->getZ() + 50) {
            delete *it;
            it = hearts.erase(it);  // Culling - loại bỏ vật thể
        }
    }
}
```

### 🎯 Kết quả
- Chỉ render các đối tượng trong tầm nhìn camera
- Tối ưu hiệu năng bằng cách loại bỏ vật thể không nhìn thấy
- Giảm số lượng polygon cần xử lý

---

## 2️⃣ TÔ MÀU (FLOOD FILL) - 6/10

### 📖 Lý thuyết thuật toán

**Flood Fill** là thuật toán tô màu vùng kín, có 2 phương pháp chính:

#### **Boundary Fill Algorithm**
- Bắt đầu từ điểm seed, tô màu cho đến khi gặp màu biên
- Thuật toán đệ quy hoặc dùng stack

#### **Flood Fill 4-connected (Queue-based)**
```
1. Thêm điểm seed vào queue
2. While queue không rỗng:
   - Lấy điểm p ra khỏi queue
   - Nếu p chưa tô và không phải màu biên:
     + Tô màu p
     + Thêm 4 láng giềng (trên, dưới, trái, phải) vào queue
```

#### **Flood Fill 8-connected**
- Tương tự 4-connected nhưng kiểm tra 8 láng giềng (thêm 4 góc chéo)

### 📍 Vị trí cài đặt trong project

#### **Texture Filling (Tô texture cho bề mặt)**
File: `src/textures/Texture.cc`

```cpp
// Tô màu toàn bộ texture buffer
void Texture::fillColor(GLubyte r, GLubyte g, GLubyte b) {
    for (int i = 0; i < width * height * 4; i += 4) {
        pixels[i] = r;      // Red
        pixels[i+1] = g;    // Green
        pixels[i+2] = b;    // Blue
        pixels[i+3] = 255;  // Alpha
    }
}
```

#### **Fragment Shader Flood Fill (GPU)**
File: `shaders/entity.frag` (dòng ~68-80)

```glsl
// Tô màu fragment với lighting
void main() {
    // Tính toán màu sắc cuối cùng
    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = specularStrength * specularFactor * lightColor;
    
    // Tô màu fragment (tương đương flood fill trên GPU)
    vec3 fragColor = (ambient + diffuse + specular) * color;
    colorTexture = vec4(fragColor, 1.0);
}
```

#### **Rasterization (Tô màu triangle)**
- OpenGL tự động thực hiện flood fill khi rasterize triangle
- Mỗi fragment trong tam giác được tô màu bởi fragment shader

### 🎯 Kết quả
- Các bề mặt 3D được tô màu đồng nhất
- Fragment shader tô màu mỗi pixel với lighting
- GPU thực hiện flood fill song song cho hàng triệu pixel

---

## 3️⃣ BIẾN ĐỔI 2D (2D TRANSFORMATIONS) - 13/10

### 📖 Lý thuyết thuật toán

Các phép biến đổi 2D cơ bản sử dụng ma trận 3×3 (homogeneous coordinates):

#### **Translation (Tịnh tiến)**
```
| 1  0  tx |   | x |   | x + tx |
| 0  1  ty | × | y | = | y + ty |
| 0  0  1  |   | 1 |   |   1    |
```

#### **Rotation (Quay)**
```
| cos(θ)  -sin(θ)  0 |   | x |   | x'cos(θ) - y'sin(θ) |
| sin(θ)   cos(θ)  0 | × | y | = | x'sin(θ) + y'cos(θ) |
|   0        0     1 |   | 1 |   |         1           |
```

#### **Scaling (Tỷ lệ)**
```
| sx  0   0 |   | x |   | x × sx |
| 0   sy  0 | × | y | = | y × sy |
| 0   0   1 |   | 1 |   |   1    |
```

#### **Shearing (Biến dạng)**
```
| 1   shx  0 |   | x |   | x + y×shx |
| shy  1   0 | × | y | = | y + x×shy |
| 0    0   1 |   | 1 |   |     1     |
```

### 📍 Vị trí cài đặt trong project

#### **UI Transformation (2D)**
File: `shaders/ui.vert` (shader cho UI elements)

```glsl
void main() {
    // Biến đổi 2D cho UI (health bar, score, etc.)
    vec2 transformedPos = position.xy;
    
    // Translation 2D
    transformedPos += offset;
    
    // Scaling 2D  
    transformedPos *= scale;
    
    gl_Position = vec4(transformedPos, 0.0, 1.0);
}
```

#### **Bird Wings Rotation (2D rotation in 3D space)**
File: `src/entities/gameObjects/Bird.cc` (dòng ~180-190)

```cpp
void Bird::updateWings() {
    // Rotation 2D cho cánh chim
    float angle = sin(glfwGetTime() * WING_SPEED) * WING_AMPLITUDE;
    
    // Ma trận rotation 2D (quay quanh trục Z)
    leftWing->setRotation(0, 0, angle);   // +angle
    rightWing->setRotation(0, 0, -angle); // -angle (đối xứng)
}
```

#### **Texture Coordinates Transformation**
File: `src/models/Geometry.cc` (dòng ~45-60)

```cpp
// UV mapping - biến đổi 2D từ 3D surface
for (int i = 0; i <= slices; ++i) {
    float u = (float)i / slices;        // Translation + Scaling
    float v = (float)j / stacks;
    texCoords.push_back(u);
    texCoords.push_back(v);
}
```

### 🎯 Kết quả
- Cánh chim vỗ lên xuống (rotation 2D)
- Health bar scaling theo HP
- UI elements positioning (translation)

---

## 4️⃣ KẾT HỢP CÁC PHÉP BIẾN ĐỔI 2D - 13/10

### 📖 Lý thuyết thuật toán

Kết hợp nhiều phép biến đổi bằng cách **nhân ma trận** theo thứ tự:

```
M_combined = M_translate × M_rotate × M_scale
```

⚠️ **Thứ tự quan trọng**: Ma trận không giao hoán (A×B ≠ B×A)

#### **Ví dụ: Quay quanh điểm bất kỳ (x₀, y₀)**
```
1. Translate về gốc tọa độ: T(-x₀, -y₀)
2. Rotate góc θ: R(θ)  
3. Translate về vị trí cũ: T(x₀, y₀)

M = T(x₀, y₀) × R(θ) × T(-x₀, -y₀)
```

#### **SRT Transformation (Standard Order)**
```
M = Translation × Rotation × Scale
```
- Scale trước (phóng to/nhỏ)
- Rotation tiếp (quay)
- Translation cuối (di chuyển)

### 📍 Vị trí cài đặt trong project

#### **Entity Transformation Matrix**
File: `src/maths/Object3D.cc` (dòng ~25-40)

```cpp
glm::mat4 Object3D::getTransformationMatrix() {
    glm::mat4 matrix = glm::mat4(1.0f);
    
    // 1. Translation
    matrix = glm::translate(matrix, position);
    
    // 2. Rotation (Yaw-Pitch-Roll)
    matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Yaw
    matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // Pitch
    matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Roll
    
    // 3. Scale
    matrix = glm::scale(matrix, scale);
    
    return matrix;  // T × R × S
}
```

#### **Camera View Matrix (Inverse Transformation)**
File: `src/entities/gameObjects/Camera.cc` (dòng ~45-47)

```cpp
glm::mat4 Camera::getViewMatrix() {
    // View = Inverse(Translation × Rotation)
    return glm::lookAt(position, position + front, up);
}
```

#### **Vertex Shader MVP Matrix**
File: `shaders/entity.vert` (dòng ~20-25)

```glsl
void main() {
    // Kết hợp Projection × View × Model
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    
    // Tương đương:
    // gl_Position = P × V × M × vertex
}
```

### 🎯 Kết quả
- Mỗi entity có vị trí, góc quay, kích thước riêng
- Camera transform ngược lại để tạo view matrix
- Vertex shader kết hợp tất cả transformations (MVP)

---

## 5️⃣ BIỂU DIỄN ĐỐI TƯỢNG 3D - 20/10

### 📖 Lý thuyết thuật toán

Có nhiều cách biểu diễn đối tượng 3D:

#### **Polygon Mesh (Lưới đa giác)**
- Biểu diễn bề mặt bằng tập hợp tam giác
- Mỗi đỉnh có: position, normal, texture coordinates
- Lưu trữ: Vertex array + Index array

#### **Parametric Surface (Mặt tham số)**
- Biểu diễn bề mặt bằng phương trình toán học
- Ví dụ: Sphere, Cylinder, Heart curve

```cpp
// Sphere: x = r×sin(θ)×cos(φ), y = r×sin(θ)×sin(φ), z = r×cos(θ)
// Heart: x = 16×sin³(t), y = 13×cos(t) - 5×cos(2t) - 2×cos(3t) - cos(4t)
```

#### **Constructive Solid Geometry (CSG)**
- Kết hợp các hình cơ bản bằng phép toán boolean (union, intersection, difference)

### 📍 Vị trí cài đặt trong project

#### **Vertex Array Object (VAO) + Vertex Buffer Object (VBO)**
File: `src/models/RawModel.h`

```cpp
class RawModel {
private:
    GLuint vaoID;        // Vertex Array Object ID
    GLuint vboID;        // Vertex Buffer Object ID  
    int vertexCount;     // Số lượng đỉnh
};
```

#### **Parametric Heart (Tham số hóa)**
File: `src/models/Geometry.cc` (dòng ~250-280)

```cpp
RawModel Geometry::createHeart(float scale) {
    std::vector<float> vertices, normals;
    
    // Parametric equation for 3D heart
    for (float t = 0; t < 2 * M_PI; t += 0.1) {
        for (float d = -0.5; d <= 0.5; d += 0.1) {
            // Heart curve formula
            float x = scale * 16 * pow(sin(t), 3);
            float y = scale * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));
            float z = d * scale;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Calculate normal vector
            // ...
        }
    }
    
    return Loader::loadToVAO(vertices, normals);
}
```

#### **Tetrahedron (Polygon Mesh)**
File: `src/models/Geometry.cc` (dòng ~200-230)

```cpp
RawModel Geometry::createTetrahedron() {
    // 4 đỉnh của tứ diện
    glm::vec3 v0(0, 1, 0);
    glm::vec3 v1(-1, -1, 1);
    glm::vec3 v2(1, -1, 1);
    glm::vec3 v3(0, -1, -1);
    
    // 4 mặt tam giác
    std::vector<float> vertices = {
        v0.x, v0.y, v0.z,  v1.x, v1.y, v1.z,  v2.x, v2.y, v2.z,  // Mặt 1
        v0.x, v0.y, v0.z,  v2.x, v2.y, v2.z,  v3.x, v3.y, v3.z,  // Mặt 2
        // ...
    };
    
    return Loader::loadToVAO(vertices, normals);
}
```

#### **Sphere (Parametric + Tessellation)**
File: `src/models/Geometry.cc` (dòng ~100-150)

```cpp
RawModel Geometry::createSphere(float radius, int slices, int stacks) {
    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;  // Góc dọc [0, π]
        
        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;  // Góc ngang [0, 2π]
            
            // Parametric sphere equation
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
}
```

#### **Bird Model (Composite Object)**
File: `src/entities/gameObjects/Bird.cc` (dòng ~50-120)

```cpp
void Bird::createModel() {
    // Thân chim
    body = new Entity(Geometry::sphere(2.5f, 20, 20), BROWN);
    
    // Đầu chim
    head = new Entity(Geometry::sphere(1.5f, 20, 20), BROWN);
    
    // Mỏ chim (cone/pyramid)
    beak = new Entity(Geometry::cockpit(0.8f, 1.5f), vec3(1.0f, 0.5f, 0.0f));
    
    // Cánh trái/phải
    leftWing = new Entity(Geometry::box(0.5f, 2.0f, 3.0f), BROWNDARK);
    rightWing = new Entity(Geometry::box(0.5f, 2.0f, 3.0f), BROWNDARK);
    
    // Đuôi
    tail = new Entity(Geometry::box(0.3f, 2.5f, 1.5f), BROWNDARK);
}
```

### 🎯 Kết quả
- Heart shape: Parametric curve với 3D depth
- Triangle obstacles: Tetrahedron mesh  
- Bird: Composite object từ nhiều primitive shapes
- Tất cả geometry được tạo thủ công (không dùng thư viện ngoài)

---

## 6️⃣ QUAN SÁT ĐỐI TƯỢNG 3D (3D VIEWING) - 27/10

### 📖 Lý thuyết thuật toán

Quá trình chuyển từ 3D world sang 2D screen qua **Graphics Pipeline**:

```
Object Space → World Space → View Space → Clip Space → NDC → Screen Space
     ↓              ↓              ↓            ↓         ↓          ↓
 Model Matrix   View Matrix   Projection   Clipping  Viewport   Rasterization
```

#### **View Matrix (Camera Transformation)**
Thuật toán **LookAt Matrix**:

```
1. Tính vector hướng nhìn: forward = normalize(target - eye)
2. Tính vector phải: right = normalize(cross(forward, worldUp))
3. Tính vector lên: up = cross(right, forward)

View Matrix = | right.x    right.y    right.z    -dot(right, eye)   |
              | up.x       up.y       up.z       -dot(up, eye)      |
              | -forward.x -forward.y -forward.z  dot(forward, eye) |
              | 0          0          0           1                 |
```

#### **Projection Matrix**

**Perspective Projection:**
```
f = cotangent(fov/2)

P = | f/aspect   0      0                0              |
    | 0          f      0                0              |
    | 0          0      (far+near)/(near-far)  2×far×near/(near-far) |
    | 0          0      -1               0              |
```

**Orthographic Projection:**
```
P = | 2/(r-l)    0          0         -(r+l)/(r-l) |
    | 0          2/(t-b)    0         -(t+b)/(t-b) |
    | 0          0          -2/(f-n)  -(f+n)/(f-n) |
    | 0          0          0          1           |
```

### 📍 Vị trí cài đặt trong project

#### **Camera Class (View Matrix)**
File: `src/entities/gameObjects/Camera.cc`

```cpp
// LookAt Matrix implementation (dòng ~45-47)
glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(
        position,           // Eye position (vị trí camera)
        position + front,   // Target (điểm nhìn)
        up                  // World up vector
    );
}

// Perspective Projection Matrix (dòng ~41-43)
glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(
        glm::radians(getFov()),                    // FOV = 60°
        (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT, // Aspect ratio
        NEAR_PLANE,                                 // Near = 1.0f
        FAR_PLANE                                   // Far = 1000.0f
    );
}
```

#### **Camera Follow Bird (Chase Camera)**
File: `src/entities/gameObjects/Camera.cc` (dòng ~80-100)

```cpp
void Camera::update() {
    Bird* bird = Bird::theOne();
    
    // Smooth camera follow algorithm
    glm::vec3 targetPos = bird->getPosition() + glm::vec3(0, 5, 20);
    position = glm::mix(position, targetPos, 0.1f);  // Lerp smoothing
    
    // LookAt bird
    glm::vec3 direction = glm::normalize(bird->getPosition() - position);
    front = direction;
}
```

#### **Vertex Shader MVP Transform**
File: `shaders/entity.vert` (dòng ~20-30)

```glsl
uniform mat4 projectionMatrix;  // Perspective projection
uniform mat4 viewMatrix;        // Camera view (LookAt)
uniform mat4 modelMatrix;       // Object transformation

void main() {
    // MVP Transform: P × V × M × vertex
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    vec4 viewPos = viewMatrix * worldPos;
    gl_Position = projectionMatrix * viewPos;
    
    // Tương đương:
    // gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
```

#### **Multiple Viewports (Optional)**
File: `src/renderEngine/DisplayManager.cc`

```cpp
// Main viewport
glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
```

### 🎯 Kết quả
- Camera tự động theo chim (chase camera với lerp smoothing)
- Perspective projection tạo cảm giác 3D
- FOV 60° cho góc nhìn rộng
- Near/far plane clipping loại bỏ vật thể quá gần/xa

---

## 7️⃣ VẼ ĐƯỜNG/MẶT CONG (CURVE DRAWING) - 10/11

### 📖 Lý thuyết thuật toán

#### **Parametric Curves (Đường cong tham số)**
```
P(t) = (x(t), y(t), z(t)),  t ∈ [0, 1]
```

**Ví dụ:**
- Circle: `x = r×cos(t), y = r×sin(t)`
- Helix: `x = r×cos(t), y = r×sin(t), z = t`
- Heart: `x = 16×sin³(t), y = 13×cos(t) - 5×cos(2t) - ...`

#### **Bézier Curves**
```
// Cubic Bézier
B(t) = (1-t)³×P₀ + 3(1-t)²t×P₁ + 3(1-t)t²×P₂ + t³×P₃
```

#### **B-Splines**
- Smooth curve qua nhiều control points
- Local control (di chuyển 1 điểm chỉ ảnh hưởng lân cận)

#### **NURBS (Non-Uniform Rational B-Splines)**
- Mở rộng của B-Splines với weights
- Có thể biểu diễn chính xác circle, ellipse, parabola

#### **Tessellation (Chia nhỏ)**
```
for (t = 0; t < 1; t += step) {
    vertices.push_back(x(t));
    vertices.push_back(y(t));
    vertices.push_back(z(t));
}
```

### 📍 Vị trí cài đặt trong project

#### **Heart Parametric Curve**
File: `src/models/Geometry.cc` (dòng ~250-280)

```cpp
RawModel Geometry::createHeart(float scale) {
    std::vector<float> vertices, normals;
    
    // Parametric heart curve (2D equation)
    for (float t = 0; t < 2 * M_PI; t += 0.05) {  // Tessellation step
        // Heart equation (parametric form)
        float x = scale * 16 * pow(sin(t), 3);
        float y = scale * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));
        
        // Extrude to 3D (add depth)
        for (float depth = -0.5f; depth <= 0.5f; depth += 0.1f) {
            float z = depth * scale;
            vertices.push_back(x);
            vertices.push_back(y);  
            vertices.push_back(z);
            
            // Calculate tangent for normal
            float dx_dt = scale * 48 * pow(sin(t), 2) * cos(t);
            float dy_dt = scale * (-13*sin(t) + 10*sin(2*t) + 6*sin(3*t) + 4*sin(4*t));
            
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

#### **Sphere (Parametric Surface)**
File: `src/models/Geometry.cc` (dòng ~100-150)

```cpp
RawModel Geometry::createSphere(float radius, int slices, int stacks) {
    // Parametric sphere: x = r×sin(φ)×cos(θ), y = r×cos(φ), z = r×sin(φ)×sin(θ)
    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;  // φ ∈ [0, π]
        
        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;  // θ ∈ [0, 2π]
            
            // Parametric equation
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal = normalize(position) for sphere
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
}
```

#### **Terrain Wave (Sine Wave)**
File: `src/models/Geometry.cc` (dòng ~180-200)

```cpp
RawModel Geometry::createSea(float width, float depth, int segments) {
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            float x = (i / (float)segments - 0.5f) * width;
            float z = (j / (float)segments - 0.5f) * depth;
            
            // Sine wave for terrain
            float y = 0.5f * sin(x * 0.5f) * cos(z * 0.5f);
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
}
```

#### **Wing Flapping (Sine Animation Curve)**
File: `src/entities/gameObjects/Bird.cc` (dòng ~180-190)

```cpp
void Bird::updateWings() {
    float time = glfwGetTime();
    
    // Sine wave for smooth flapping motion
    float angle = sin(time * WING_SPEED) * WING_AMPLITUDE;  // [-30°, +30°]
    
    leftWing->setRotationZ(angle);
    rightWing->setRotationZ(-angle);  // Mirror
}
```

### 🎯 Kết quả
- Trái tim 3D từ parametric curve (heart equation)
- Chim với cánh vỗ theo sine wave
- Mặt đất địa hình với wave pattern
- Sphere/cylinder từ parametric equations

---

## 8️⃣ KHỬ KHUẤT (HIDDEN SURFACE REMOVAL) - 17/11

### 📖 Lý thuyết thuật toán

Loại bỏ bề mặt bị che khuất để tránh render vật thể phía sau:

#### **Depth Buffer (Z-Buffer Algorithm)**
```
for each pixel (x, y):
    depth_buffer[x][y] = ∞
    color_buffer[x][y] = background_color

for each polygon:
    for each pixel (x, y) in polygon:
        calculate z_depth at (x, y)
        if z_depth < depth_buffer[x][y]:
            depth_buffer[x][y] = z_depth
            color_buffer[x][y] = polygon_color
```

**Ưu điểm:**
- Đơn giản, dễ cài đặt
- Hardware acceleration (GPU hỗ trợ)
- O(n) complexity

#### **Painter's Algorithm (Depth Sorting)**
```
1. Sort all polygons by depth (far to near)
2. Render from back to front
3. Nearer polygons paint over farther ones
```

**Nhược điểm:**
- Không xử lý được overlapping cycles
- Cần sort mỗi frame

#### **Back-Face Culling**
```
if dot(normal, view_direction) > 0:
    discard polygon  // Facing away from camera
```

#### **BSP Tree (Binary Space Partitioning)**
- Chia không gian thành tree structure
- Traverse tree theo view direction
- Render back-to-front tự động

### 📍 Vị trí cài đặt trong project

#### **Z-Buffer (Depth Testing)**
File: `src/renderEngine/DisplayManager.cc` (dòng ~50-60)

```cpp
void DisplayManager::init() {
    // Enable depth testing (Z-buffer)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // Pass if depth < current depth
    
    // Clear depth buffer mỗi frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
```

#### **Depth Buffer Clear**
File: `src/renderEngine/Renderer.cc` (dòng ~20-25)

```cpp
void Renderer::render() {
    // Clear depth buffer về ∞ (1.0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render all entities - GPU tự động Z-buffer test
    entityShader.render();
    seaShader.render();
}
```

#### **Back-Face Culling**
File: `src/renderEngine/DisplayManager.cc` (dòng ~55-58)

```cpp
void DisplayManager::init() {
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);   // Loại bỏ mặt sau
    glFrontFace(GL_CCW);   // Counter-clockwise = front face
}
```

#### **Fragment Shader Depth Write**
File: `shaders/entity.frag`

```glsl
void main() {
    // GPU tự động ghi gl_FragDepth vào depth buffer
    // gl_FragDepth = gl_FragCoord.z (tự động)
    
    vec3 fragColor = (ambient + diffuse + specular) * color;
    colorTexture = vec4(fragColor, 1.0);
    
    // Depth test: if (gl_FragCoord.z < depthBuffer[x][y])
}
```

#### **Early Z-Test Optimization**
File: OpenGL driver (hardware)

```cpp
// GPU thực hiện early depth test trước fragment shader
// Nếu depth test fail → skip fragment shader (tối ưu hiệu năng)
```

#### **Frustum Culling (View Frustum Test)**
File: `src/entities/gameObjects/HeartHolder.cc` (dòng ~65-70)

```cpp
void HeartHolder::update() {
    for (auto it = hearts.begin(); it != hearts.end();) {
        // Frustum culling - loại bỏ vật thể ngoài view frustum
        if ((*it)->getZ() > Camera::theOne()->getZ() + 50) {
            delete *it;
            it = hearts.erase(it);
        }
    }
}
```

### 🎯 Kết quả
- Z-buffer tự động loại bỏ pixel bị che khuất
- Back-face culling giảm 50% polygon cần render
- Frustum culling loại bỏ vật thể ngoài màn hình
- Early Z-test tối ưu hiệu năng GPU

---

## 9️⃣ TÔ BÓNG GOURAUD (GOURAUD SHADING) - 24/11

### 📖 Lý thuyết thuật toán

**Gouraud Shading** tính lighting tại đỉnh (vertex) rồi nội suy màu cho fragment:

```
1. Tính lighting tại mỗi đỉnh (vertex shader):
   - Ambient: I_a = k_a × L_a
   - Diffuse: I_d = k_d × L_d × max(N·L, 0)
   - Specular: I_s = k_s × L_s × max(R·V, 0)^n
   - I_vertex = I_a + I_d + I_s

2. Nội suy màu (rasterization):
   - I_fragment = lerp(I_v0, I_v1, I_v2, barycentric_coords)
```

**So sánh với Phong Shading:**
| | Gouraud | Phong |
|---|---|---|
| Tính toán | Tại đỉnh | Tại mỗi pixel |
| Hiệu năng | Nhanh hơn | Chậm hơn |
| Chất lượng | Mất mát specular highlights | Chính xác hơn |
| Mach banding | Có thể xảy ra | Không có |

### 📍 Vị trí cài đặt trong project

⚠️ **Lưu ý**: Project hiện tại sử dụng **Phong Shading** (per-pixel lighting), không phải Gouraud!

#### **Phong Shading (Current Implementation)**
File: `shaders/entity.frag` (dòng ~40-80)

```glsl
// Fragment Shader - Phong Shading (per-pixel)
void main() {
    // Ambient
    float ambientStrength = 0.15 * ambientLightIntensity;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse (tính tại mỗi fragment)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(FragPos - lightPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular (tính tại mỗi fragment)
    vec3 viewDir = normalize(ViewSpace.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * specularFactor * lightColor;
    
    // Kết hợp
    vec3 fragColor = (ambient + diffuse + specular) * color;
    colorTexture = vec4(fragColor, 1.0);
}
```

#### **Gouraud Shading Example (Nếu muốn chuyển sang)**

**Vertex Shader (entity.vert):**
```glsl
out vec3 vertexColor;  // Output color từ vertex shader

void main() {
    // Tính lighting tại đỉnh
    vec3 norm = normalize(normalMatrix * normal);
    vec3 fragPos = vec3(modelMatrix * vec4(position, 1.0));
    vec3 lightDir = normalize(fragPos - lightPos);
    
    // Phong lighting tại đỉnh (Gouraud)
    vec3 ambient = 0.15 * lightColor;
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;
    
    vertexColor = (ambient + diffuse + specular) * color;  // Tính màu tại đỉnh
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
```

**Fragment Shader (entity.frag):**
```glsl
in vec3 vertexColor;  // Nhận màu đã nội suy từ vertex shader

void main() {
    colorTexture = vec4(vertexColor, 1.0);  // Dùng màu nội suy
}
```

#### **Comparison Code**

**Current (Phong) vs Gouraud:**
```cpp
// PHONG: Tính lighting tại fragment shader
// - Chất lượng cao hơn
// - Specular highlights chính xác
// - Tốn nhiều GPU hơn

// GOURAUD: Tính lighting tại vertex shader  
// - Hiệu năng tốt hơn
// - Mất mát chi tiết specular
// - Có thể bị Mach banding
```

### 🎯 Kết quả
- **Hiện tại**: Phong shading cho chất lượng cao
- **Nếu dùng Gouraud**: Nhanh hơn ~30% nhưng mất specular highlights
- Phù hợp cho low-poly models hoặc mobile devices

---

## 🔧 CÁC KỸ THUẬT ĐỒ HỌA NÂNG CAO KHÁC

### 10. **Motion Blur (Làm mờ chuyển động)**

File: `shaders/motionBlur.frag`, `src/renderEngine/Renderer.cc`

```glsl
// Sử dụng velocity buffer để tạo motion blur
vec2 velocity = texture(velocityTexture, TexCoords).xy;
vec3 blurredColor = texture(sceneTexture, TexCoords - velocity).rgb;
```

### 11. **Fog (Sương mù)**

File: `shaders/entity.frag` (dòng ~75-80)

```glsl
// Distance-based fog
float dist = abs(ViewSpace.z);
float fogFactor = (FAR - dist) / (FAR - NEAR);
fogFactor = clamp(fogFactor, 0.0, 1.0);

vec3 finalColor = mix(fogColor, fragColor, fogFactor);
```

### 12. **Phong Lighting Model**

File: `shaders/entity.frag` (dòng ~50-70)

```glsl
// Ambient + Diffuse + Specular
vec3 ambient = k_a * lightColor;
vec3 diffuse = k_d * max(dot(N, L), 0.0) * lightColor;
vec3 specular = k_s * pow(max(dot(R, V), 0.0), shininess) * lightColor;
```

### 13. **Texture Mapping**

File: `src/textures/Texture.cc`

```cpp
// UV coordinates mapping
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
```

### 14. **Framebuffer Object (FBO)**

File: `src/renderEngine/Renderer.cc`

```cpp
// Render to texture
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// Render scene...
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

### 15. **Alpha Blending (Trong suốt)**

File: `src/renderEngine/DisplayManager.cc`

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

### 16. **Collision Detection (Sphere-Sphere)**

File: `src/gameEngine/Collision.cc`

```cpp
float distance = glm::length(obj1->getPosition() - obj2->getPosition());
if (distance < obj1->getRadius() + obj2->getRadius()) {
    // Collision detected
}
```

---

## 📂 CẤU TRÚC THƯ MỤC DỰ ÁN

```
DHMT/
├── src/
│   ├── main.cc                      # Entry point
│   ├── common.h                     # Constants, colors
│   ├── entities/
│   │   ├── Entity.{cc,h}           # Base entity class
│   │   ├── DynamicEntity.{cc,h}    # Physics entity
│   │   └── gameObjects/
│   │       ├── Bird.{cc,h}         # Player character
│   │       ├── HeartHolder.{cc,h}  # Collectibles manager
│   │       ├── TriangleHolder.{cc,h} # Obstacles manager
│   │       └── Camera.{cc,h}       # Camera system
│   ├── models/
│   │   ├── Geometry.{cc,h}         # 3D shape generation
│   │   ├── Loader.{cc,h}           # VAO/VBO loading
│   │   └── RawModel.{cc,h}         # Model data structure
│   ├── renderEngine/
│   │   ├── Renderer.{cc,h}         # Main renderer
│   │   └── DisplayManager.{cc,h}   # Window/OpenGL setup
│   ├── shaders/
│   │   ├── ShaderProgram.{cc,h}    # Shader base class
│   │   ├── EntityShader.{cc,h}     # Entity rendering
│   │   ├── SeaShader.{cc,h}        # Terrain rendering
│   │   └── MotionBlurShader.{cc,h} # Post-processing
│   ├── gameEngine/
│   │   ├── Game.{cc,h}             # Game loop
│   │   └── Collision.{cc,h}        # Physics/collision
│   ├── maths/
│   │   ├── Maths.{cc,h}            # Math utilities
│   │   └── Object3D.{cc,h}         # 3D transformations
│   └── io/
│       ├── KeyboardManager.{cc,h}  # Keyboard input
│       └── MouseManager.{cc,h}     # Mouse input
├── shaders/
│   ├── entity.vert                  # Entity vertex shader
│   ├── entity.frag                  # Entity fragment shader (Phong)
│   ├── sea.vert/frag                # Terrain shaders
│   ├── ui.vert/frag                 # UI shaders
│   └── motionBlur.vert/frag         # Motion blur shaders
├── external/
│   ├── glad/                        # OpenGL loader
│   ├── glfw/                        # Window/input library
│   └── glm/                         # Math library (vectors/matrices)
└── CMakeLists.txt                   # Build configuration
```**Kết hợp PV Matrix (Dòng 49-51):**

```cpp
glm::mat4 Camera::getPVMatrix() {
    return getProjectionMatrix() * getViewMatrix();
}
```

#### File: `shaders/entity.vert` (Dòng 20-26)

```glsl
void main() {
    vec4 position4 = vec4(position, 1.0);
    vec4 worldPosition = transformationMatrix * position4;      // Model → World
    ViewSpace = viewMatrix * worldPosition;                     // World → View
    CurPos = projectionMatrix * ViewSpace;                      // View → Clip
    gl_Position = CurPos;
}
```

**Chuyển đổi tọa độ world sang màn hình (Dòng 81-86):**

```cpp
glm::vec2 Camera::screenPos(glm::vec4 worldPos) {
    glm::vec4 screenPos = getProjectionMatrix() * getViewMatrix() * worldPos;
    float x = (1.0f + screenPos.x) / 2.0f * (float)ACTUAL_WIDTH;
    float y = (1.0f - screenPos.y) / 2.0f * (float)ACTUAL_HEIGHT;
    return glm::vec2(x, y);
}
```

---

## 3. BIẾN ĐỔI 2D/3D VÀ KẾT HỢP PHÉP BIẾN ĐỔI

### 📖 Lý thuyết

- **Translation (Tịnh tiến)**: Di chuyển đối tượng
- **Rotation (Quay)**: Xoay quanh trục
- **Scale (Tỷ lệ)**: Phóng to/thu nhỏ
- **Kết hợp**: Nhân nhiều ma trận theo thứ tự: T × R × S

### 📍 Vị trí sử dụng

#### File: `src/maths/Maths.cc`

**Ma trận tịnh tiến (Dòng 31-36):**

```cpp
glm::mat4 Maths::calculateTranslationMatrix(float x, float y, float z) {
    glm::vec3 delta = glm::vec3(x, y, z);
    glm::mat4 translation(1.0f);
    translation = glm::translate(translation, delta);
    return translation;
}
```

**Ma trận quay với kết hợp (Dòng 38-49):**

```cpp
glm::mat4 Maths::calculateRotationMatrix(float x, float y, float z, glm::vec3 center) {
    glm::mat4 rotationMatrix(1.0f), T(1.0f), T_1(1.0f);

    // Bước 1: Tịnh tiến về gốc tọa độ
    T = glm::translate(T, -center);

    // Bước 2: Quay quanh các trục
    rotationMatrix = glm::rotate(rotationMatrix, x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, z, glm::vec3(0.0f, 0.0f, 1.0f));

    // Bước 3: Tịnh tiến về vị trí ban đầu
    T_1 = glm::translate(T_1, center);

    // Kết hợp: T_1 × R × T
    rotationMatrix = T_1 * rotationMatrix * T;
    return rotationMatrix;
}
```

**Quay quanh trục bất kỳ (Dòng 51-59):**

```cpp
glm::mat4 Maths::rotateAroundAxis(glm::vec3 axis, float angle, glm::vec3 center) {
    glm::mat4 rotationMatrix(1.0f), T(1.0f), T_1(1.0f);
    T = glm::translate(T, -center);
    T_1 = glm::translate(T_1, center);

    rotationMatrix = glm::rotate(rotationMatrix, angle, axis);

    return T_1 * rotationMatrix * T;
}
```

#### File: `src/maths/Object3D.cc`

```cpp
// Áp dụng các phép biến đổi cho đối tượng
void Object3D::updateTransformation() {
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 R = glm::mat4_cast(rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

    transformationMatrix = T * R * S;  // Kết hợp các phép biến đổi
}
```

#### File: `src/entities/gameObjects/Camera.cc` (Dòng 19-27)

```cpp
void Camera::changePosition(float degree) {
    // Kết hợp các phép biến đổi để xoay camera quanh điểm
    glm::mat4 r, t, t_1;
    t = glm::translate(t, glm::vec3(0.0f, -position.y, 0.0f));
    t_1 = glm::translate(t_1, glm::vec3(0.0f, position.y, 0.0f));
    r = glm::rotate(r, degree, glm::vec3(0.0f, 1.0f, 0.0f));
    r = t_1 * r * t;  // Kết hợp: T_inverse × R × T
    position = glm::vec3(r * glm::vec4(position, 1.0f));
}
```

---

## 4. KHỬ KHUẤT (HIDDEN SURFACE REMOVAL)

### 📖 Lý thuyết

- **Depth Testing (Z-Buffer)**: So sánh độ sâu để xác định pixel nào gần nhất
- **Face Culling**: Loại bỏ các mặt quay mặt sau
- **Shadow Mapping**: Dùng depth map để tạo bóng đổ

### 📍 Vị trí sử dụng

#### File: `src/shaders/EntityShader.cc` (Dòng 38-40)

```cpp
void EntityShader::render() {
    start();
    glEnable(GL_DEPTH_TEST);      // Bật depth testing
    glEnable(GL_BLEND);           // Bật alpha blending
    glEnable(GL_CULL_FACE);       // Bật face culling
    // ...
}
```

#### File: `src/renderEngine/Renderer.cc`

**Shadow Mapping - Pass 1: Render depth map (Dòng 46-54):**

```cpp
void Renderer::render() {
    // Bước 1: Render scene từ góc nhìn của ánh sáng để tạo depth map
    glViewport(0, 0, SHADOW::WIDTH, SHADOW::HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowShader::getFboID());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);         // Cull front faces (giảm shadow acne)
    seaShadowShader.render();
    entityShadowShader.render();
    glCullFace(GL_BACK);          // Trở lại cull back faces
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
```

**Pass 2: Render scene chính (Dòng 56-65):**

```cpp
    // Bước 2: Render scene thật với shadow map
    glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap().getID());  // Bind depth map
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    backgroundShader.render();
    entityShader.render();
    seaShader.render();
```

#### File: `shaders/entity.frag` (Dòng 21-39)

```glsl
// Tính toán shadow bằng shadow mapping
float shadowCalculation(vec4 lightSpaceFragPos) {
    // Chuyển sang normalized device coordinates
    vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Lấy depth từ shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Nếu fragment ở ngoài frustum
    if (currentDepth > 1.0)
        return 0;

    float shadow = 0.0;
    float bias = max(0.002 * (1.0 - dot(Normal, normalize(lightPos))), 0.0005);

    // PCF (Percentage Closer Filtering) để làm mềm bóng
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;  // Average
}
```

#### File: `src/entities/gameObjects/Camera.cc` (Dòng 53-58)

```cpp
// Tạo ma trận Light Space cho shadow mapping
glm::mat4 Camera::getLightSpaceMatrix() {
    glm::vec3 lightPos(Light::theOne().getPosition());
    glm::mat4 viewMatrix = glm::lookAt(lightPos, target, up);
    glm::mat4 projectionMatrix = glm::perspective(fov, aspect, near, far);
    return projectionMatrix * viewMatrix;
}
```

---

## 5. TÔ BÓNG (SHADING)

### 📖 Lý thuyết

- **Phong Lighting Model** gồm 3 thành phần:
  - **Ambient**: Ánh sáng môi trường
  - **Diffuse**: Ánh sáng khuếch tán (phụ thuộc góc tới)
  - **Specular**: Ánh sáng phản chiếu (điểm sáng)
- Sử dụng pháp tuyến (normal vectors) để tính toán

### 📍 Vị trí sử dụng

#### File: `shaders/entity.frag` (Dòng 41-79)

**Phong Lighting Model đầy đủ:**

```glsl
void main() {
    vec3 fogColor = vec3(0.968, 0.851, 0.667);
    vec3 unitNormal = normalize(Normal);
    vec3 unitToCameraVector = normalize(ToCameraVector);

    // Ánh sáng trực tiếp (directional light)
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightDir = normalize(lightPos - FragPos);

    // 1. AMBIENT LIGHT (Ánh sáng môi trường)
    float ambientStrength = 0.4 * ambientLightIntensity;
    vec3 ambient = ambientStrength * lightColor;

    // 2. DIFFUSE LIGHT (Ánh sáng khuếch tán - Lambert)
    float diff = max(dot(unitNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 3. SPECULAR LIGHT (Ánh sáng phản chiếu - Phong)
    float specularStrength = 0.5;
    vec3 reflectedLightDir = reflect(-lightDir, unitNormal);
    float specularFactor = pow(max(dot(reflectedLightDir, unitToCameraVector), 0.0), 64);
    vec3 specular = specularStrength * specularFactor * lightColor;

    // 4. SHADOW (Bóng đổ)
    float visibility = 1.0;
    float shadow = 0.0;
    if (receiveShadow == 1)
        shadow = visibility * shadowCalculation(LightSpaceFragPos);

    // Kết hợp: Ambient + (1 - Shadow) × (Diffuse + Specular)
    vec3 fragColor = (ambient + (1 - shadow) * (diffuse + specular)) * color;

    // 5. FOG (Sương mù)
    float dist = abs(ViewSpace.z);
    float fogFactor = (far - dist)/(far - near);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = (1.0 - fogFactor) * fogColor + fogFactor * fragColor;
    colorTexture = vec4(finalColor, opacity);
}
```

#### File: `shaders/entity.vert` (Dòng 28-31)

```glsl
void main() {
    // ...

    // Transform normal vector từ object space sang world space
    Normal = normalize(mat3(transpose(inverse(transformationMatrix))) * normal);

    // Vector từ fragment đến camera (cho specular)
    ToCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;

    // Position của fragment trong light space (cho shadow)
    LightSpaceFragPos = lightSpaceMatrix * worldPosition;
}
```

**Giải thích các thành phần:**

1. **Ambient (Dòng 50-52)**: Ánh sáng nền cơ bản

   ```glsl
   ambient = 0.4 × intensity × lightColor
   ```

2. **Diffuse (Dòng 54-56)**: Phụ thuộc góc giữa normal và light direction

   ```glsl
   diffuse = max(dot(N, L), 0) × lightColor
   ```

3. **Specular (Dòng 58-62)**: Điểm sáng phản chiếu
   ```glsl
   specular = strength × pow(max(dot(R, V), 0), shininess) × lightColor
   ```
   - R: Reflected light direction
   - V: View direction
   - Shininess = 64 (độ bóng)

---

## 6. VẼ ĐƯỜNG/MẶT CONG

### 📖 Lý thuyết

- Sử dụng **Geometry Shader** để tạo geometry động
- Tạo bề mặt cong bằng cách chia nhỏ và nội suy

### 📍 Vị trí sử dụng

#### File: `shaders/sea.geom`

```glsl
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Geometry shader để tạo bề mặt biển động
void main() {
    // Nhận 3 đỉnh của triangle
    for(int i = 0; i < 3; i++) {
        // Biến đổi và tạo wave effect
        gl_Position = gl_in[i].gl_Position + wave_offset;
        EmitVertex();
    }
    EndPrimitive();
}
```

#### File: `src/models/Geometry.cc`

**Tạo sphere bằng parametric equations (dòng ~50-80):**

```cpp
RawModel* Geometry::sphere(float radius, int slices, int stacks) {
    vector<float> vertices;
    vector<float> normals;

    // Dùng tọa độ cầu để tạo surface
    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;  // Góc dọc [0, π]

        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;  // Góc ngang [0, 2π]

            // Parametric sphere equations
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal tại mỗi điểm = (x, y, z) / radius
            normals.push_back(x / radius);
            normals.push_back(y / radius);
            normals.push_back(z / radius);
        }
    }

    return Loader::loadToVAO(vertices, normals);
}
```

**Tạo cylinder (dòng ~100-130):**

```cpp
RawModel* Geometry::cylinder(float radius, float height, int sides) {
    vector<float> vertices;

    for (int i = 0; i < sides; ++i) {
        float angle1 = 2 * M_PI * i / sides;
        float angle2 = 2 * M_PI * (i + 1) / sides;

        // Vẽ mặt bên của cylinder
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);

        // 2 triangles cho mỗi segment
        // Triangle 1
        vertices.push_back(x1); vertices.push_back(0); vertices.push_back(z1);
        vertices.push_back(x2); vertices.push_back(0); vertices.push_back(z2);
        vertices.push_back(x1); vertices.push_back(height); vertices.push_back(z1);

        // Triangle 2
        vertices.push_back(x2); vertices.push_back(0); vertices.push_back(z2);
        vertices.push_back(x2); vertices.push_back(height); vertices.push_back(z2);
        vertices.push_back(x1); vertices.push_back(height); vertices.push_back(z1);
    }

    return Loader::loadToVAO(vertices);
}
```

#### File: `shaders/sea.vert` & `shaders/sea.frag`

- Tạo sóng biển bằng sine waves
- Geometry shader modify vertices để tạo hiệu ứng động

---

## 7. XÉN HÌNH (CLIPPING)

### 📖 Lý thuyết

- **View Frustum Clipping**: Loại bỏ geometry nằm ngoài view frustum
- OpenGL tự động thực hiện trong pipeline
- Near plane và far plane định nghĩa vùng nhìn thấy

### 📍 Vị trí sử dụng

#### File: `src/entities/gameObjects/Camera.cc` (Dòng 42)

```cpp
glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(
        glm::radians(getFov()),
        (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT,
        NEAR_PLANE,    // Near clipping plane - Gần nhất: 0.1
        FAR_PLANE      // Far clipping plane - Xa nhất: 1000.0
    );
}
```

#### File: `common.h`

```cpp
// Định nghĩa near và far plane
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 1000.0f;
```

#### File: `shaders/entity.frag` (Dòng 26-27)

```glsl
float shadowCalculation(vec4 lightSpaceFragPos) {
    // ...
    float currentDepth = projCoords.z;

    // Clipping: Nếu fragment nằm ngoài shadow map frustum
    if (currentDepth > 1.0)
        return 0;  // Không có shadow
    // ...
}
```

#### File: `shaders/entity.frag` (Dòng 71-77)

```glsl
// Fog clipping dựa trên khoảng cách
float dist = abs(ViewSpace.z);
float near = 100.0;
float far = 500.0;
float fogFactor = (far - dist)/(far - near);
fogFactor = clamp(fogFactor, 0.0, 1.0);  // Clamp vào [0, 1]
```

**OpenGL Clipping Pipeline tự động:**

1. Vertex Shader output → Clip Space ([-w, w] cho x, y, z)
2. OpenGL tự động clip các primitives ngoài view frustum
3. Perspective divide: (x/w, y/w, z/w) → NDC [-1, 1]
4. Viewport transform → Screen coordinates

---

## 8. CÁC KỸ THUẬT NÂNG CAO KHÁC

### 8.1. MOTION BLUR

#### 📖 Lý thuyết

- Per-object motion blur
- Tính velocity của mỗi object
- Sample và blur theo hướng chuyển động

#### 📍 Vị trí sử dụng

**File: `shaders/entity.frag` (Dòng 81-88)**

```glsl
void main() {
    // ... lighting calculations ...

    // Tính velocity vector cho motion blur
    vec2 a = (CurPos.xy / CurPos.w) * 0.5 + 0.5;      // Current screen position
    vec2 b = (PrevPos.xy / PrevPos.w) * 0.5 + 0.5;    // Previous screen position
    vec2 difference = (a - b);                         // Velocity

    // Enhance velocity với power function
    difference.x = pow(difference.x, 3.0);
    difference.y = pow(difference.y, 3.0);

    velocityTexture = vec4(difference, 0.0, 1.0);     // Save to velocity map
}
```

**File: `shaders/entity.vert` (Dòng 18, 24)**

```glsl
uniform mat4 prevPVM;  // Previous frame's PVM matrix

void main() {
    // ...
    PrevPos = prevPVM * position4;   // Position ở frame trước
    CurPos = projectionMatrix * ViewSpace;  // Position hiện tại
    // ...
}
```

**File: `shaders/motionBlur.frag`**

```glsl
// Sample colors dọc theo velocity vector
vec2 velocity = texture(velocityTexture, TexCoords).xy;

vec4 color = vec4(0.0);
int samples = 10;

// Average colors along motion path
for (int i = 0; i < samples; ++i) {
    vec2 offset = velocity * (float(i) / float(samples - 1) - 0.5);
    color += texture(colorTexture, TexCoords + offset);
}

FragColor = color / float(samples);
```

**File: `src/shaders/EntityShader.cc` (Dòng 101-119)**

```cpp
// Update previous transformation cho frame sau
void EntityShader::updateEntityVelocity() {
    for (auto& entry: staticEntities) {
        vector<Entity*>& entities = entry.second;
        for (int i = 0; i < entities.size(); ++i) {
            entities.at(i)->updatePrevTransformation();
        }
    }
    // ... particles ...
}
```

---

### 8.2. PARTICLE SYSTEM

#### 📍 Vị trí sử dụng

**File: `src/entities/gameObjects/ParticleHolder.h`**

```cpp
class ParticleHolder {
    static vector<DynamicEntity*> particles;

    // Spawn, update, remove particles
    static void spawnParticle();
    static void updateParticles(float dt);
};
```

---

### 8.3. COLLISION DETECTION

#### 📍 Vị trí sử dụng

**File: `src/gameEngine/Collision.cc`**

```cpp
// Bounding sphere collision
bool Collision::sphereIntersect(Entity* a, Entity* b) {
    float distance = glm::length(a->getPosition() - b->getPosition());
    float radiusSum = a->getRadius() + b->getRadius();
    return distance < radiusSum;
}
```

---

### 8.4. LENS FLARE

#### 📖 Lý thuyết

- Tính screen position của mặt trời
- Render textures dọc theo line từ sun → screen center

#### 📍 Vị trí sử dụng

**File: `src/entities/gameObjects/Camera.cc` (Dòng 81-86)**

```cpp
// Chuyển đổi world position sang screen position
glm::vec2 Camera::screenPos(glm::vec4 worldPos) {
    glm::vec4 screenPos = getProjectionMatrix() * getViewMatrix() * worldPos;
    float x = (1.0f + screenPos.x) / 2.0f * (float)ACTUAL_WIDTH;
    float y = (1.0f - screenPos.y) / 2.0f * (float)ACTUAL_HEIGHT;
    return glm::vec2(x, y);
}
```

---

## 📊 TỔNG KẾT KIẾN THỨC

| STT | Kiến thức              | Sử dụng | File chính                                        |
| --- | ---------------------- | ------- | ------------------------------------------------- |
| 1   | Biểu diễn đối tượng 3D | ✅      | `models/Geometry.cc`, `models/RawModel.cc`        |
| 2   | Quan sát đối tượng 3D  | ✅      | `gameObjects/Camera.cc`                           |
| 3   | Biến đổi 2D/3D         | ✅      | `maths/Maths.cc`, `maths/Object3D.cc`             |
| 4   | Khử khuất              | ✅      | `renderEngine/Renderer.cc`, `shaders/entity.frag` |
| 5   | Tô bóng Phong/Gouraud  | ✅      | `shaders/entity.frag`, `shaders/entity.vert`      |
| 6   | Vẽ đường/Mặt cong      | ✅      | `models/Geometry.cc`, `shaders/sea.geom`          |
| 7   | Xén hình               | ✅      | `gameObjects/Camera.cc`, OpenGL pipeline          |
| 8   | Tô màu (Flood Fill)    | ❌      | Không sử dụng (dùng modern rendering)             |

---

## 🎯 ĐIỂM NỔI BẬT CỦA DỰ ÁN

1. **Modern OpenGL 3.3+**: Sử dụng shader-based rendering
2. **Advanced Lighting**: Phong model + Shadow mapping + Fog
3. **Post-processing**: Motion blur với velocity buffer
4. **Geometry Generation**: Procedural sphere, cylinder, box
5. **Game Engine**: Complete với physics, collision, particle system

---

## 📚 TÀI LIỆU THAM KHẢO

- OpenGL Documentation: https://www.opengl.org/
- Learn OpenGL: https://learnopengl.com/
- GLM Math Library: https://glm.g-truc.net/
- Original Three.js Tutorial: https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/

---

**Tác giả**: Dự án The Aviator  
**Ngày cập nhật**: 01/12/2025  
**Mục đích**: Tài liệu học tập môn Đồ Họa Máy Tính
