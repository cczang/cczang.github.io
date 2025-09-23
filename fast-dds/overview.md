## DDS是什么
Data Distribution Service (DDS)是一种用于分布式软件应用程序通信的数据中心通信协议。它描述了数据提供者和数据消费者之间通信的应用程序编程接口（API）和通信语义。
由于它是一个以数据为中心的发布/订阅（DCPS）模型，其实现中定义了三个关键的应用实体：发布实体，定义信息生成对象及其属性；订阅实体，定义信息消费对象及其属性；
以及配置实体，定义以主题形式传输的信息类型，并创建具有其服务质量（QoS）属性的发布者和订阅者，确保上述实体的正确性能。

## DCPS 概念模型 
**DDS模型**
在 DCPS 模型中，定义了四个基本元素，用于开发一个通信应用程序系统：
- Publisher：发布者。它是 DCPS 实体，负责创建和配置其实现的 DataWriter。DataWriter 是负责实际发布消息的实体。每个发布者都将分配一个主题，消息将在该主题下发布。
- Subscriber：订阅者。它是 DCPS 实体，负责接收其订阅的主题下发布的数据。它服务于一个或多个 DataReader 对象，这些对象负责将新数据的可用性传达给应用程序。
- Topic：主题。它是绑定发布和订阅的实体。在 DDS 域内是唯一的。通过主题描述，它允许发布和订阅的数据类型保持一致性。
- Domain：域。这是用于连接属于一个或多个应用的所有发布者和订阅者的概念，这些应用在不同的主题下交换数据。这些参与域的独立应用被称为域参与者。DDS 域通过域 ID 来识别。
  域参与者定义域 ID 以指定它所属的 DDS 域。具有不同 ID 的两个域参与者不会意识到对方在网络中的存在。因此，可以创建多个通信通道。这在涉及多个 DDS 应用且它们的域参与
  者相互通信但应用之间不应相互干扰的场景中适用。域参与者充当其他 DCPS 实体的容器，充当发布者、订阅者和主题实体的工厂，并在域中提供管理服务。
  
<img width="3110" height="1460" alt="image" src="https://github.com/user-attachments/assets/227bcaf4-0cdb-44dd-867d-9a6707c31794" />

## RTPS是什么
Real-Time Publish Subscribe (RTPS)是为支持 DDS 应用而开发的实时发布/订阅（RTPS）协议，是一种在尽力而为的传输方式（如 UDP/IP）上的发布/订阅通信中间件
，旨在支持单播和多播通信。此外，Fast DDS 还提供了对 TCP 和共享内存（SHM）传输的支持。

**RTPS模型**
<img width="2911" height="1561" alt="image" src="https://github.com/user-attachments/assets/2f649588-29c5-4379-bb5b-5d542d5c24cd" />

**架构**
<img width="1112" height="942" alt="image" src="https://github.com/user-attachments/assets/d17e3361-52ca-44e8-a179-eb3d4d4554ac" />



[FAST-DDS DOC](https://fast-dds.docs.eprosima.com/en/stable/index.html)
