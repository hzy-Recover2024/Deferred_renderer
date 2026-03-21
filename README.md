# myDemo - OpenGL Deferred Rendering Demo

## 项目简介

myDemo 是一个基于 OpenGL 的延迟渲染（Deferred Rendering）演示项目，基于 LearnOpenGL 教程实现。该项目展示了现代图形渲染技术，包括：

- **延迟渲染管线**：将几何信息存储到 G-Buffer，然后在光照通道中计算光照
- **Render Graph 架构**：可扩展的渲染通道管理系统
- **Shaderc 集成**：使用 shaderc 进行着色器预处理和编译

## 项目结构

```
myDemo/
├── src/
│   ├── inc/                    # 头文件
│   │   ├── rendergraph/        # Render Graph 相关类
│   │   └── deferred/           # 延迟渲染相关类
│   ├── rendergraph/            # Render Graph 实现
│   └── deferred/               # 延迟渲染实现
├── shader/
│   ├── common/                 # 公共着色器头文件
│   └── deferred/               # 延迟渲染着色器
├── include/                    # 第三方库头文件
├── thirdParty/                 # 第三方库
└── CMakeLists.txt
```

## 核心功能

### Render Graph 系统

- `RenderResource`: 渲染资源基类
- `Texture`: 纹理资源管理
- `Framebuffer`: 帧缓冲管理
- `RenderPass`: 渲染通道基类
- `RenderGraph`: 渲染图管理

### 延迟渲染系统

- `GBuffer`: G-Buffer 管理（位置、法线、反照率/高光、深度）
- `LightManager`: 光源管理
- `GeometryPass`: 几何通道
- `LightingPass`: 光照通道
- `DeferredRenderer`: 延迟渲染器主类

## 依赖项

- **GLFW**: 窗口和输入管理
- **GLM**: 数学库
- **GLAD**: OpenGL 加载器
- **Assimp**: 模型加载
- **stb_image**: 图像加载
- **shaderc**: 着色器编译

## 构建指南

### 前置要求

- CMake 3.6+
- Visual Studio 2019/2022 (Windows)
- C++17 编译器

### Windows 构建步骤

```bash
# 进入项目目录
cd myDemo

# 创建构建目录
mkdir build
cd build

# 生成项目文件 (Visual Studio 2022)
cmake .. -G "Visual Studio 17 2022" -A x64

# 编译 (Release 模式)
cmake --build . --config Release
```

### 运行

编译完成后，可执行文件位于 `bin/Release/myDemo.exe`。

## 使用示例

```cpp
#include "deferred/DeferredRenderer.h"

// 初始化延迟渲染器
DeferredRenderer renderer;
renderer.Init(SCR_WIDTH, SCR_HEIGHT);

// 设置着色器
renderer.SetGeometryShader(&geometryShader);
renderer.SetLightingShader(&lightingShader);
renderer.SetLightBoxShader(&lightBoxShader);

// 添加光源
renderer.GetLightManager().AddPointLight(position, color);

// 渲染循环
while (!glfwWindowShouldClose(window)) {
    // 几何通道
    renderer.BeginGeometryPass(projection, view);
    // 渲染场景物体...
    renderer.EndGeometryPass();
    
    // 光照通道
    renderer.RenderLightingPass(cameraPos);
    
    // 复制深度缓冲（用于前向渲染）
    renderer.CopyDepthBuffer();
    
    // 渲染光源可视化
    renderer.RenderLightBoxes(projection, view);
}
```

## 参考资料

- [LearnOpenGL - Deferred Shading](https://learnopengl.com/Advanced-Lighting/Deferred-Shading)
- [shaderc - Shader Compilation Library](https://github.com/google/shaderc)

## 许可证

本项目仅供学习参考。
