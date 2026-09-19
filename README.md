# ConsoleGeo

Pure Console-Based Plane Analytic Geometry Computing Platform

## 功能特性

- 二维直角坐标系绘图（支持圆 / 椭圆 / 抛物线 / 双曲线）
- 直线与曲线交点判定（相交 / 相切 / 相离）
- 快捷键：`S` 对数值平方，`R` 开根号
- 鼠标点击获取坐标，方向键导航

## 环境要求

- C++20 兼容编译器（GCC 10+ / Clang 13+ / MSVC 2019+）
- CMake 3.16+
- Windows 10/11 原生 CMD 或 Windows Terminal

## 项目结构

```
ConsoleGeo/
├── CMakeLists.txt           # CMake 构建配置
├── LICENSE                  # MIT 许可证
├── README.md                # 本文件
├── src/                     # 源代码
│   ├── conic.hpp/cpp        # 圆锥曲线（圆/椭圆/抛物线/双曲线）
│   ├── intersection.hpp/cpp # 直线与曲线交点判定算法
│   ├── canvas_draw.hpp/cpp  # 坐标画布与渲染
│   ├── ui_handler.hpp/cpp   # FTXUI 界面与事件处理
│   └── main.cpp             # 程序入口
├── include/                 # FTXUI 头文件（已包含在项目中）
│   └── ftxui/
└── thrid_party/
    └── ftxui/FTXUI-main/    # FTXUI 完整源码（MIT 许可证）
```

## 快速构建（Windows MinGW）

### 1. 安装依赖

**MinGW-w64：**
- 下载 [MinGW-w64](https://www.mingw-w64.org/) （版本：x86_64-posix-seh）
- 将 `mingw64/bin` 加入系统 PATH

**CMake：**
```bash
winget install Kitware.CMake
```

### 2. 构建项目

```bash
# 克隆项目
git clone https://github.com/cpt3-dev/ConsoleGeo.git
cd ConsoleGeo

# 创建构建目录（如果已存在 build 目录且想保留缓存，跳过此步）
mkdir build && cd build

# 配置（MinGW Makefiles）
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release
```

### 3. 运行

```bash
./ConsoleGeo.exe
```

### Ninja 加速构建（可选）

如果安装了 [Ninja](https://ninja-build.org/)，可以使用 Ninja  генератор 加速编译：

```bash
# 警告：更换 генератор 会与旧的 build 目录冲突
# 方法 A：删除旧 build 目录后使用 Ninja
rmdir /s /q build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# 方法 B：复用已有 build 目录继续用 MinGW Makefiles
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

> [!NOTE]
> 如果 build 目录已存在且之前用的是另一个 генератор，CMake 会报错 "Does not match the generator used previously"。此时需删除 build 目录重建，或沿用原 генера 器。

## 详细构建说明

### Windows MSVC + Visual Studio

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Linux (GCC + Make)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Linux (Clang + Ninja)

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
cmake --build .
```

### macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## 常见问题

| 问题 | 解决方案 |
|------|---------|
| CMake 提示 "Could not find CMakeLists.txt" | 确保在项目根目录执行 `mkdir build && cd build` |
| MinGW 报错 "undefined reference" | 确认 MinGW 为 GCC 12+，旧版本 C++20 支持不完整 |
| 运行时终端无颜色 | 升级到 Windows 10 1903+，程序已自动启用虚拟终端序列 |
| 鼠标点击无响应 | 确认终端支持 VT100 鼠标序列，Windows Terminal 已默认支持 |
| 编译提示 ftxui 函数未定义 | 确认 `thrid_party/ftxui/FTXUI-main/` 完整且未被杀毒软件隔离 |

## 操作说明

| 操作 | 按键 |
|------|------|
| 切换曲线类型 | 在右侧面板选择（Tab / 方向键） |
| 移动输入焦点 | Tab / Shift+Tab |
| 平方（S） | 选中数值后按 `S` |
| 开根号（R） | 选中数值后按 `R` |
| 确认输入 | Enter |
| 退出程序 | Esc |

## 许可证

本项目采用 [MIT 许可证](LICENSE) 开源。

FTXUI 库同样采用 MIT 许可证，详情见 [thrid_party/ftxui/FTXUI-main/LICENSE](thrid_party/ftxui/FTXUI-main/LICENSE)。