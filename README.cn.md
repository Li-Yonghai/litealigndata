

# LiteAlignData

LiteAlignData 是一个用于生成测试数据的 C++ 数据生成框架。该项目主要用于网络协议数据分析场景，支持多种数据格式的生成和处理。

## 项目简介

LiteAlignData 是一个轻量级的数据对齐和生成工具，主要功能包括：

- **多格式数据支持**：支持 Ctrldata、Example 等多种数据格式的解析和生成
- **灵活的数据处理**：提供随机函数、累加函数、唯一随机函数等多种数据处理机制
- **网络流服务**：基于 libevent 的高性能网络服务器，支持流式数据输出
- **XML 配置**：通过 XML 配置文件定义数据结构，支持复杂的表关系和依赖
- **维度文件支持**：支持从维度文件中读取数据并进行映射处理

## 目录结构

```
litealigndata/
├── src/                          # 源代码目录
│   ├── com/                      # 公共组件
│   │   ├── AlignDataTypeDef.cc   # 数据类型定义
│   │   ├── AlignDataUtils.cc    # 工具函数
│   │   └── Comm.cc               # 公共通信和类型映射
│   ├── conf/                     # 配置模块
│   │   ├── AlignConfig.cc        # 配置管理
│   │   └── AligndataConfig.cc    # 数据配置解析
│   ├── file/                     # 文件格式处理
│   │   ├── BaseFileFormat.cc     # 基础文件格式
│   │   ├── Ctrldata/              # Ctrldata 格式处理
│   │   └── Example/                # Example 格式处理
│   ├── fun/                      # 函数处理
│   │   ├── BaseFuncClass.cc       # 基础函数类
│   │   └── FunctionObject.cc      # 函数对象处理
│   ├── log/                      # 日志模块
│   │   └── LogHandler.cc          # 日志处理
│   ├── stream/                   # 网络流服务
│   │   ├── Acceptor.cc            # 连接接受器
│   │   ├── Server.cpp              # 服务器主程序
│   │   ├── ThreadManager.cc        # 线程管理
│   │   └── WorkThread.cc           # 工作线程
│   ├── struct/                   # 数据结构
│   │   ├── AlignDataObject.cc     # 数据对象基类
│   │   ├── ColumnObject.cc         # 列对象
│   │   ├── TableObject.cc          # 表对象
│   │   └── DataStructure.cc        # 数据结构定义
│   └── main.cc                   # 主程序入口
├── testcase/                     # 测试用例
│   ├── case/                     # 测试案例
│   └── apimock/                  # API 模拟
├── tools/                        # 工具程序
│   ├── Client/                   # 客户端工具
│   └── DeEncode/                 # 编解码工具
├── seedfile/                     # 种子文件
│   ├── Ctrldata/                 # 控制数据
│   └── Example/                  # 示例数据
├── stacks/                       # 第三方依赖库
│   └── tarballs/                 # 依赖包
├── DockerBuild/                  # Docker 构建
├── DockerJenkins/                # Jenkins Docker
├── build_all.sh                  # 完整构建脚本
├── build_src.sh                  # 源代码构建脚本
└── clean_all.sh                  # 清理脚本
```

## 核心功能

### 数据类型支持

项目支持丰富的数据类型，包括：
- 整型：UINT64, INT64, UINT32, INT32, UINT16, INT16, UINT8, INT8
- 网络类型：IPv4, IPv6, IPv4V6, IPv4_NET, IPv6_NET
- 字符串：STRING, ByteArray
- 时间类型：DateTime, RealTime, RealTime_ms, TIMEVAL
- 特殊编码：BCD, IMSIBCD, IMEISV_BCD, SP_U8_4, SP_IPV6

### 函数处理

- **RandomFunc**：随机数据生成，支持多种随机模式
- **AcculateFunc**：累加计算函数
- **MergeFunc**：数据合并函数
- **UniqueRandomFunc**：唯一随机数生成
- **fileReaderFunc**：文件读取函数

### 网络服务

基于 libevent 实现的高性能服务器，支持：
- TCP 连接管理
- 心跳检测
- 流式数据输出
- 多线程处理

## 构建要求

- C++ 编译器 (GCC 7.0+)
- CMake 3.26+
- ACE 7.1.0
- libevent 2.1.12
- Xerces-C 3.1.4
- Boost 1.64.0
- 其他依赖库（见 stacks/tarballs/）

## 快速开始

### 1. 构建项目

请参考INSTALL

### 2. 运行程序

请参考INSTALL

### 3. 客户端连接

请参考INSTALL

## 配置说明

### XML 配置文件

项目使用 XML 文件定义数据结构，包括：
- 表定义 (Table)
- 列定义 (Column)
- 函数定义 (Function)
- 依赖关系 (Dependency)

## Docker 部署

### 构建镜像

```bash
cd DockerBuild
./build.sh
```

### 运行 Jenkins 服务

```bash
cd DockerJenkins
docker-compose up
```

## 测试

项目包含完整的测试用例框架：

```bash
cd testcase/case
./casemain
```

## 许可证

本项目的详细信息请查看 LICENSE 文件。

## 贡献

欢迎提交 Issue 和 Pull Request。