# HelloWorld DDS 示例

这是一个基于Fast-DDS的HelloWorld示例，展示了如何使用配置文件来管理DDS通信参数。

## 文件结构

```
helloworld/
├── HelloWorld.idl          # IDL文件，定义数据结构
├── profile.xml             # Fast-DDS配置文件
├── publisher.cpp           # 发布者程序
├── subscriber.cpp          # 订阅者程序
├── CMakeLists.txt          # CMake构建配置
├── build.sh               # 构建脚本
└── README.md              # 本文件
```

## 配置文件说明

### profile.xml

Fast-DDS配置文件包含以下主要配置：

#### 1. 参与者配置 (Participant)
- **域ID**: 设置为0，publisher和subscriber使用相同的域ID
- **发现协议**: 使用SIMPLE发现协议
- **传输配置**: 使用UDPv4传输

#### 2. QoS配置
- **可靠性**: RELIABLE - 确保消息可靠传递
- **持久性**: TRANSIENT_LOCAL - 本地持久化
- **历史**: KEEP_LAST，深度为10
- **资源限制**: 最大样本数、实例数等

#### 3. Topic配置
- **名称**: HelloWorldTopic
- **数据类型**: HelloWorld
- **键类型**: NO_KEY

## 程序修改说明

### Publisher修改
- 使用`create_participant_with_profile()`创建参与者
- 使用`create_publisher_with_profile()`创建发布者
- 使用`create_datawriter_with_profile()`创建数据写入器
- 所有配置从`profile.xml`文件读取

### Subscriber修改
- 使用`create_participant_with_profile()`创建参与者
- 使用`create_subscriber_with_profile()`创建订阅者
- 使用`create_datareader_with_profile()`创建数据读取器
- 所有配置从`profile.xml`文件读取

## 构建和运行

### 1. 构建
```bash
cd /opt/fast-dds/examples/helloworld
./build.sh
```

### 2. 运行
```bash
# 启动订阅者
cd /opt/fast-dds/examples/bin
./HelloWorldSubscriber 10

# 启动发布者（在另一个终端）
./HelloWorldPublisher 10
```

## 配置文件优势

1. **集中管理**: 所有DDS参数集中在一个XML文件中
2. **易于修改**: 无需重新编译代码即可修改配置
3. **环境适配**: 可以为不同环境创建不同的配置文件
4. **参数验证**: Fast-DDS会验证配置文件的正确性
5. **可读性**: XML格式便于理解和维护

## 配置参数说明

### 域ID (Domain ID)
- 用于隔离不同的DDS域
- 相同域ID的参与者可以相互通信
- 不同域ID的参与者无法通信

### QoS参数
- **RELIABLE**: 确保消息可靠传递，适用于关键数据
- **TRANSIENT_LOCAL**: 新加入的订阅者可以接收历史数据
- **KEEP_LAST**: 只保留最新的N个样本

### 传输配置
- **UDPv4**: 使用UDP协议进行通信
- **接口白名单**: 限制网络接口
- **最大消息大小**: 设置消息大小限制

## 故障排除

### 常见问题

1. **配置文件未找到**
   - 确保`profile.xml`文件在程序运行目录中
   - 检查文件路径和权限

2. **配置参数错误**
   - 检查XML语法是否正确
   - 验证QoS参数组合是否有效

3. **通信失败**
   - 确认publisher和subscriber使用相同的域ID
   - 检查网络配置和防火墙设置

### 调试建议

1. 启用Fast-DDS日志输出
2. 检查配置文件加载信息
3. 验证参与者、发布者、订阅者创建状态
4. 监控网络流量和连接状态
