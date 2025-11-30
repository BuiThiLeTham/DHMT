# HƯỚNG DẪN KIẾN THỨC ĐỒ HỌA MÁY TÍNH

## Dự án: The Aviator - OpenGL 3D Game

Dự án OpenGL 3D Game thực hành các kỹ thuật đồ họa máy tính hiện đại.

---

## 📋 MỤC LỤC

1. [Biểu diễn đối tượng 3D](#1-biểu-diễn-đối-tượng-3d)
2. [Quan sát đối tượng 3D (Camera & View)](#2-quan-sát-đối-tượng-3d)
3. [Biến đổi 2D/3D và Kết hợp phép biến đổi](#3-biến-đổi-2d3d-và-kết-hợp-phép-biến-đổi)
4. [Khử khuất (Hidden Surface Removal)](#4-khử-khuất-hidden-surface-removal)
5. [Tô bóng (Shading - Phong/Gouraud)](#5-tô-bóng-shading)
6. [Vẽ đường/Mặt cong](#6-vẽ-đườngmặt-cong)
7. [Xén hình (Clipping)](#7-xén-hình-clipping)
8. [Các kỹ thuật nâng cao khác](#8-các-kỹ-thuật-nâng-cao-khác)

---

## 🚀 HƯỚNG DẪN COMPILE VÀ CHẠY

### Yêu cầu
- CMake 3.10+
- C++ Compiler (GCC/Clang/MSVC)
- OpenGL 3.3+
- GLFW3, GLAD, GLM (đã có trong thư mục external/)

### Compile trên Windows (MSYS2)

```bash
cd /d/tool/c++/dhmt/DHMT
mkdir build
cd build
cmake -G "MSYS Makefiles" ..
make -j4
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

---

## 1. BIỂU DIỄN ĐỐI TƯỢNG 3D

### 📖 Lý thuyết

- Biểu diễn đối tượng 3D bằng các đỉnh (vertices), pháp tuyến (normals)
- Sử dụng mô hình mesh geometry
- Lưu trữ trong VAO/VBO của OpenGL

### 📍 Vị trí sử dụng

#### File: `src/models/RawModel.h` & `src/models/RawModel.cc`

```cpp
// Lưu trữ thông tin mesh 3D
class RawModel {
  private:
    GLuint vaoID;           // Vertex Array Object
    int vertexCount;        // Số đỉnh
};
```

#### File: `src/models/Geometry.h` & `src/models/Geometry.cc`

```cpp
// Tạo các hình học cơ bản 3D
Geometry::sphere(float radius, int slices, int stacks);
Geometry::box(float width, float height, float depth);
Geometry::cylinder(float radius, float height, int sides);
Geometry::tetrahedron();
```

#### File: `src/models/Loader.cc`

- Dòng ~20-50: Load vertices và normals vào GPU

```cpp
// Upload dữ liệu 3D lên GPU
glBindVertexArray(vaoID);
glBindBuffer(GL_ARRAY_BUFFER, vboID);
glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);
```

#### File: `src/entities/Entity.h`

```cpp
// Mỗi entity chứa model 3D và transformation matrix
class Entity {
    RawModel* model;                    // Mô hình 3D
    glm::mat4 transformationMatrix;     // Ma trận biến đổi
};
```

---

## 2. QUAN SÁT ĐỐI TƯỢNG 3D

### 📖 Lý thuyết

- **View Matrix**: Biến đổi từ world space sang camera space
- **Projection Matrix**: Phép chiếu phối cảnh (perspective projection)
- Sử dụng công thức: `gl_Position = Projection × View × Model × vertex`

### 📍 Vị trí sử dụng

#### File: `src/entities/gameObjects/Camera.cc`

**View Matrix (Dòng 45-47):**

```cpp
glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}
```

- `position`: Vị trí camera
- `front`: Hướng nhìn
- `up`: Vector hướng lên

**Projection Matrix (Dòng 41-43):**

```cpp
glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(
        glm::radians(getFov()),                          // Góc nhìn
        (float)ACTUAL_WIDTH / (float)ACTUAL_HEIGHT,      // Tỷ lệ khung hình
        NEAR_PLANE,                                       // Mặt phẳng gần
        FAR_PLANE                                         // Mặt phẳng xa
    );
}
```

**Kết hợp PV Matrix (Dòng 49-51):**

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
