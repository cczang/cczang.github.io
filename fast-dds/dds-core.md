## DDS Entity

### Entity 类型
- DomainParticipant: 此实体是服务的入口点，充当发布者、订阅者和主题的工厂。  
- Publisher: 它作为一个可以创建任意数量 DataWriters 的工厂。  
- Subscriber: 它作为一个可以创建任意数量 DataReaders 的工厂。  
- Topic: 该实体位于发布和订阅实体之间，充当通道。    
- DataWriter: 负责数据分发。    
- DataReader: 用于访问接收到的数据。  
  
![entity_diagram](https://github.com/user-attachments/assets/6eacf70e-1a95-4199-ab9d-d0a627649df2)  
**<p align="center"> DDS 实体之间的层次结构 </p>**

### Entity 通用特性
#### 标识符
   > 每个实体都通过一个唯一的ID进行标识, 并且该ID和对应的RTPS实体共享。通过get_instance_handle() 访问。
#### QoS policy
   > 每个实体的行为可以通过一组配置策略进行配置。对于每种实体类型，都有一个相应的服务质量（QoS）类，该类将影响该实体类型的所有策略分组。通过set_qos()设置。

   - DeadlineQosPolicy (仅适用于DataWriter、DataReader、Topic)
     - 此 QoS 策略当新样本的频率低于某个阈值时发出警报。它适用于预期数据将定期更新的情况.    
     - 在DataWriter方面，截止日期定义了应用预期提供新样本的最大期限。在DataReader方面，它定义了接收新样本的最大期限。对于具有Key的Topic，此 QoS 按Key应用。  
     - **为了保持 DataReaders 和 DataWriters 中 DeadlineQosPolicy 的兼容性，提供的截止期限（在 DataWriter 上配置）必须小于或等于请求的截止期限（在 DataReader 上配置），
       否则，这些实体被视为不兼容。**  
      ```c++
      // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities  
      DataWriterQos writer_qos;
      // The DeadlineQosPolicy is constructed with an infinite period by default
      // Change the period to 1 second
      writer_qos.deadline().period.seconds = 1;
      writer_qos.deadline().period.nanosec = 0;
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
  - DestinationOrderQosPolicy (仅适用于DataWriter、DataReader、Topic)  
      多个 DataWriters 可以使用相同的 key 在同一个主题中发送消息，在 DataReader 端，所有这些消息都存储在相同的数据实例中。
      此 QoS 策略控制确定这些消息逻辑顺序的准则。系统的行为取决于 DestinationOrderQosPolicyKind 的值。    
      常见取值：
      - BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS（default）
        这表示数据是根据每个 DataReader 的接收时间进行排序的，这意味着最后接收的值应该被保留。这个选项可能会导致每个 DataReader
        最终得到不同的值，因为 DataReader 可能会在不同时间接收到数据。
      - BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS
         这表示数据是根据消息发送时的 DataWriter 时间戳进行排序的。这个选项保证了最终值的连续性。
        > **注意：Both options depend on the values of the OwnershipQosPolicy and OwnershipStrengthQosPolicy, meaning that if the
      Ownership is set to EXCLUSIVE and the last value came from a DataWriter with low ownership strength, it will be discarded.**
        - 兼容性要求(**DataWriter >= DataReader**)：
        > BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS < BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS.
   - DurabilityQosPo cy (仅适用于DataWriter、DataReader、Topic，已启用实体无法修改)  
      DataWriter 可以在没有 DataReaders 在网络上时，在整个 Topic 中发送消息。
      DurabilityQoSPolicy 定义了系统在 DataReader 加入之前存在于 Topic 上的那些样本方面的行为。
      - VOLATILE_DURABILITY_QOS : 忽略过去的数据样本，连接的 DataReader 接收匹配时刻之后生成的样本。
      - TRANSIENT_LOCAL_DURABILITY_QOS : 当一个新的 DataReader 加入时，其历史中充满了过去的样本。
      - TRANSIENT_DURABILITY_QOS : 当一个新的 DataReader 加入时，其历史记录中填充了过去的样本，这些样本存储在持久存储中.
      - PERSISTENT_DURABILITY_QOS : 当一个新的 DataReader 加入时，其历史记录中填充了过去的样本，这些样本存储在持久存储中。
      - 兼容性要求(**DataWriter >= DataReader**)：
        > VOLATILE_DURABILITY_QOS < TRANSIENT_LOCAL_DURABILITY_QOS < TRANSIENT_DURABILITY_QOS < PERSISTENT_DURABILITY_QOS.  
        > **为了在 DataReader 中接收过去的样本，除了设置此 Qos 策略外，还需要将 ReliabilityQosPolicy 设置为 RELIABLE_RELIABILITY_QOS**
    - HistoryQosPolicy (仅适用于DataWriter、DataReader、Topic，已启用实体无法修改)  
      此 QoS 策略控制当实例的值在成功传达给现有 DataReader 实体之前改变一次或多次时系统的行为。
      ``` C++
      // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities
      DataWriterQos writer_qos;
      // The HistoryQosPolicy is constructed with kind = KEEP_LAST and depth = 1 by default
      // It is possible to adjust the depth and keep the kind as KEEP_LAST
      writer_qos.history().depth = 20; //combine with kind = KEEP_LAST_HISTORY_QOS if not keep all
      // Or you can also change the kind to KEEP_ALL (depth will not be used).
      writer_qos.history().kind = KEEP_ALL_HISTORY_QOS;
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
      - 兼容性要求：
        depth <= max_samples_per_instance (ResourceLimitsQosPolicy)
  - LifespanQosPolicy(仅适用于DataWriter、DataReader、Topic，已启用实体可以修改)  
      每个数据写入器写入的数据样本都有一个相关的过期时间，在此时间之后，数据将从数据写入器和数据读取器的历史记录以及从临时和持久信息缓存中删除，
      默认持续时间是无限的。
      ``` c++
      // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities
      DataWriterQos writer_qos;
      // The LifespanQosPolicy is constructed with duration set to infinite by default
      // Change the duration to 5 s
      writer_qos.lifespan().duration = {5, 0};
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
 - LivelinessQosPolicy(仅适用于DataWriter、DataReader、Topic，已启用实体无法修改)  
      此 QoS 策略控制服务使用的机制，以确保网络上的特定实体仍然存活。
      ``` c++
      // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities
      DataWriterQos writer_qos;
      // The LivelinessQosPolicy is constructed with kind = AUTOMATIC by default
      // Change the kind to MANUAL_BY_PARTICIPANT
      writer_qos.liveliness().kind = MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
      // The LivelinessQosPolicy is constructed with lease_duration set to infinite by default
      // Change the lease_duration to 1 second
      writer_qos.liveliness().lease_duration = {1, 0};
      // The LivelinessQosPolicy is constructed with announcement_period set to infinite by default
      // Change the announcement_period to 1 ms
      writer_qos.liveliness().announcement_period = {0, 1000000};
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
  - OwnershipQosPolicy(仅适用于DataWriter、DataReader、Topic，已启用实体无法修改)  
      ``` C++
      // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities
      DataWriterQos writer_qos;
      // The OwnershipQosPolicy is constructed with kind = SHARED by default
      // Change the kind to EXCLUSIVE
      writer_qos.ownership().kind = EXCLUSIVE_OWNERSHIP_QOS;
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
 - OwnershipStrengthQosPolicy(仅适用于DataWriter,已启用实体可以修改)  
      此 QoS 策略指定用于在多个尝试修改同一数据实例的 DataWriter 之间进行仲裁的强度值。仅在 OwnershipQosPolicy 类型设置为 EXCLUSIVE_OWNERSHIP_QOS 时适用。
      ``` C++
      // This example only applies to DataWriter entities
      DataWriterQos writer_qos;
      // The OwnershipStrengthQosPolicy is constructed with value 0 by default
      // Change the strength to 10
      writer_qos.ownership_strength().value = 10;
      // Use modified QoS in the creation of the corresponding DataWriter
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
  - PartitionQosPolicy(仅适用于DataWriter、DataReader,已启用实体可以修改)  
      此 QoS 策略允许在由域引入的物理分区内部引入一个逻辑分区。对于 DataReader 要看到 DataWriter 所做的更改，不仅主题必须匹配，而且它们必须共享至少一个逻辑分区.
      ``` c++
      // This example uses a Publisher, but it can also be applied to Subscriber entities
      PublisherQos publisher_qos;
      // The PartitionsQosPolicy is constructed with max_size = 0 by default
      // Max_size is a private member so you need to use getters and setters to access
      // Change the max_size to 20
      publisher_qos.partition().set_max_size(20); // Setter function
      // The PartitionsQosPolicy is constructed with an empty list of partitions by default
      // Partitions is a private member so you need to use getters and setters to access
      
      // Add new partitions in initialization
      std::vector<std::string> part;
      part.push_back("part1");
      part.push_back("part2");
      publisher_qos.partition().names(part); // Setter function
      // Use modified QoS in the creation of the corresponding entity
      publisher_ = participant_->create_publisher(publisher_qos);
      
      // Add data to the collection at runtime
      part = publisher_qos.partition().names(); // Getter to keep old values
      part.push_back("part3");
      publisher_qos.partition().names(part); // Setter function
      // Update the QoS in the corresponding entity
      publisher_->set_qos(publisher_qos);
      ```
  - ReliabilityQosPolicy (仅适用于DataWriter、DataReader、Topic，已启用实体无法修改)  
      - BEST_EFFORT_RELIABILITY_QOS ：表示可以接受不重传丢失的样本，因此消息将发送而不等待到达确认。
      - RELIABLE_RELIABILITY_QOS ：表示服务将尝试交付 DataWriter 历史的所有样本，并期望从 DataReader 收到到达确认。
        如果存在尚未接收的先前样本，则无法将相同 DataWriter 发送的数据样本提供给 DataReader。服务将重传丢失的数据样本，
        以便在 DataReader 可以访问之前重建 DataWriter 历史的正确快照。
        此选项可能会阻止写操作，因此设置了 max_blocking_time ，一旦时间到期，它将解除阻塞。但如果在数据发送之前 max_blocking_time 已过期，写操作将返回错误。  
      **注意：** 将此 QoS 策略设置为 BEST_EFFORT_RELIABILITY_QOS 会影响 DurabilityQosPolicy，使端点表现得像 VOLATILE_DURABILITY_QOS 。
        ``` c++
        // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities
        DataWriterQos writer_qos;
        // The ReliabilityQosPolicy is constructed with kind = BEST_EFFORT by default
        // Change the kind to RELIABLE
        writer_qos.reliability().kind = RELIABLE_RELIABILITY_QOS;
        // The ReliabilityQosPolicy is constructed with max_blocking_time = 100ms by default
        // Change the max_blocking_time to 1s
        writer_qos.reliability().max_blocking_time = {1, 0};
        // Use modified QoS in the creation of the corresponding entity
        writer_ = publisher_->create_datawriter(topic_, writer_qos);
        ```  
  - ResourceLimitsQosPolicy(仅适用于DataWriter、DataReader、Topic，已启用实体无法修改)  
      <img width="605" height="161" alt="image" src="https://github.com/user-attachments/assets/4e5e94ed-e4d8-4a5a-ac50-ec72ef5a95e1" />
      ```c++
      // This example uses a DataWriter, but it can also be applied to DataReader and Topic entities
      DataWriterQos writer_qos;
      // The ResourceLimitsQosPolicy is constructed with max_samples = 5000 by default
      // Change max_samples to 2000
      writer_qos.resource_limits().max_samples = 2000;
      // The ResourceLimitsQosPolicy is constructed with max_instances = 10 by default
      // Change max_instances to 20
      writer_qos.resource_limits().max_instances = 20;
      // The ResourceLimitsQosPolicy is constructed with max_samples_per_instance = 400 by default
      // Change max_samples_per_instance to 100
      writer_qos.resource_limits().max_samples_per_instance = 100;
      // The ResourceLimitsQosPolicy is constructed with allocated_samples = 100 by default
      // Change allocated_samples to 50
      writer_qos.resource_limits().allocated_samples = 50;
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
  - TopicDataQosPolicy(仅适用于Topic，可修改）  
      允许应用程序将附加信息附加到创建的主题中，以便当它被远程应用程序发现时，可以访问数据并使用它。  
    
      ```c++
      // This example only applies to Topic entities
      TopicQos topic_qos;
      // The TopicDataQosPolicy is constructed with an empty vector by default
      std::vector<eprosima::fastdds::rtps::octet> vec;
      // Add two new octets to topic data vector
      eprosima::fastdds::rtps::octet val = 3;
      vec.push_back(val);
      val = 10;
      vec.push_back(val);
      topic_qos.topic_data().data_vec(vec); // Setter Function
      // Use modified QoS in the creation of the corresponding Topic
      topic_ = participant_->create_topic("<topic_name>", "<type_name>", topic_qos);
      ```
  - UserDataQosPolicy(DomainParticipant, DataWriter and DataReader,可修改)  
    
      ```c++
      // This example uses a DataWriter, but it can also be applied to DomainParticipant and DataReader entities
      DataWriterQos writer_qos;
      std::vector<eprosima::fastdds::rtps::octet> vec;
      // Add two new octets to user data vector
      eprosima::fastdds::rtps::octet val = 3;
      vec.push_back(val);
      val = 10;
      vec.push_back(val);
      writer_qos.user_data().data_vec(vec); // Setter Function
      // Use modified QoS in the creation of the corresponding entity
      writer_ = publisher_->create_datawriter(topic_, writer_qos);
      ```
    
#### Listener
   > 监听器是一个对象，它包含实体在响应事件时将调用的函数。因此，监听器充当一个异步通知系统，允许实体通知应用程序关于实体状态变化的信息。通过set_listener()设置。

**<p align="center"> Listener 继承图 </p>**
<img width="672" height="672" alt="image" src="https://github.com/user-attachments/assets/3d94af1f-9532-4da2-bf01-b49ac0ba7655" /> 

#### Status
   > 每个实体都与一组状态对象相关联，这些对象的值表示该实体的通信状态。
<img width="563" height="379" alt="image" src="https://github.com/user-attachments/assets/e770f022-8ce4-48e0-acd0-0e04e26d1f03" />


#### 启用
   > 所有实体都可以创建为启用或未启用。默认情况下，工厂被配置为创建启用的实体，但可以通过在启用的工厂上使用 EntityFactoryQosPolicy 来更改。

