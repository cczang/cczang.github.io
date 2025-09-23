## 基于 DDS
### Publisher 创建流程
- 创建 DomainParticipant
> 用 DomainParticipantQos 定义域 ID、QoS。  
> 使用 DomainParticipantFactory::create_participant() 创建。  
- 注册数据类型
> 用 TypeSupport 包装你的数据类型（IDL 定义或手动结构体 + 序列化）。  
> 调用 DomainParticipant::register_type() 注册。  
- 创建 Publisher
> 用 PublisherQos 定义 QoS（可靠性、历史记录长度等）。  
> 调用 DomainParticipant::create_publisher() 得到 Publisher 实例。  
- 创建 Topic
> 用 DomainParticipant::create_topic() 定义 Topic 名称、数据类型、TopicQos。  
- 创建 DataWriter
> 用 DataWriterQos 定义写端 QoS。  
> 调用 Publisher::create_datawriter() 得到 DataWriter。  
> 用 DataWriter::write() 发送数据。  

``` c++
{
    // Set up the data type with initial values
    hello_.index(0);
    hello_.message("Hello world");

    // Create the participant
    auto factory = DomainParticipantFactory::get_instance();
    participant_ = factory->create_participant_with_default_profile(nullptr, StatusMask::none());
    if (participant_ == nullptr)
    {
        throw std::runtime_error("Participant initialization failed");
    }

    // Register the type
    type_.register_type(participant_);

    // Create the publisher
    PublisherQos pub_qos = PUBLISHER_QOS_DEFAULT;
    participant_->get_default_publisher_qos(pub_qos);
    publisher_ = participant_->create_publisher(pub_qos, nullptr, StatusMask::none());
    if (publisher_ == nullptr)
    {
        throw std::runtime_error("Publisher initialization failed");
    }

    // Create the topic
    TopicQos topic_qos = TOPIC_QOS_DEFAULT;
    participant_->get_default_topic_qos(topic_qos);
    topic_ = participant_->create_topic(topic_name, type_.get_type_name(), topic_qos);
    if (topic_ == nullptr)
    {
        throw std::runtime_error("Topic initialization failed");
    }

    // Create the data writer
    DataWriterQos writer_qos = DATAWRITER_QOS_DEFAULT;
    writer_qos.history().depth = 5;
    publisher_->get_default_datawriter_qos(writer_qos);
    writer_ = publisher_->create_datawriter(topic_, writer_qos, this, StatusMask::all());
    if (writer_ == nullptr)
    {
        throw std::runtime_error("DataWriter initialization failed");
    }
}

bool PublisherApp::publish()
{
    if (!is_stopped())
    {
        hello_.index(hello_.index() + 1);
        ret = (RETCODE_OK == writer_->write(&hello_));
    }
    return ret;
}

```
### Subscriber 创建流程
- 创建 DomainParticipant（同 Publisher）
- 注册数据类型（必须和 Publisher 的数据类型一致）
- 创建 Subscriber
> DomainParticipant::create_subscriber()。
- 创建 Topic（同 Publisher 的 Topic 名称 & 类型）
- 创建 DataReader
> 用 DataReaderQos 定义 QoS（匹配 Publisher QoS）。  
> 调用 Subscriber::create_datareader()。  
> 设置 Listener 回调函数以接收数据on_data_available()。  
- 处理接收数据
> 调用 DataReader::take() 或 read()。
``` c++
//use listener
{
    // Create the participant
    auto factory = DomainParticipantFactory::get_instance();
    participant_ = factory->create_participant_with_default_profile(nullptr, StatusMask::none());
    if (participant_ == nullptr)
    {
        throw std::runtime_error("Participant initialization failed");
    }

    // Register the type
    type_.register_type(participant_);

    // Create the subscriber
    SubscriberQos sub_qos = SUBSCRIBER_QOS_DEFAULT;
    participant_->get_default_subscriber_qos(sub_qos);
    subscriber_ = participant_->create_subscriber(sub_qos, nullptr, StatusMask::none());
    if (subscriber_ == nullptr)
    {
        throw std::runtime_error("Subscriber initialization failed");
    }

    // Create the topic
    TopicQos topic_qos = TOPIC_QOS_DEFAULT;
    participant_->get_default_topic_qos(topic_qos);
    topic_ = participant_->create_topic(topic_name, type_.get_type_name(), topic_qos);
    if (topic_ == nullptr)
    {
        throw std::runtime_error("Topic initialization failed");
    }

    // Create the reader
    DataReaderQos reader_qos = DATAREADER_QOS_DEFAULT;
    subscriber_->get_default_datareader_qos(reader_qos);
    reader_ = subscriber_->create_datareader(topic_, reader_qos, this, StatusMask::all());
    if (reader_ == nullptr)
    {
        throw std::runtime_error("DataReader initialization failed");
    }
}

void ListenerSubscriberApp::on_data_available(
        DataReader* reader)
{
    SampleInfo info;
    while ((!is_stopped()) && (RETCODE_OK == reader->take_next_sample(&hello_, &info)))
    {
        if ((info.instance_state == ALIVE_INSTANCE_STATE) && info.valid_data)
        {
            received_samples_++;
            // Print Hello world message data
            std::cout << "Message: '" << hello_.message() << "' with index: '" << hello_.index()
                      << "' RECEIVED" << std::endl;
            if (samples_ > 0 && (received_samples_ >= samples_))
            {
                stop();
            }
        }
    }
}

```
``` c++
//use waitset
{
    // Create the participant
    auto factory = DomainParticipantFactory::get_instance();
    participant_ = factory->create_participant_with_default_profile(nullptr, StatusMask::none());
    if (participant_ == nullptr)
    {
        throw std::runtime_error("Participant initialization failed");
    }

    // Register the type
    type_.register_type(participant_);

    // Create the subscriber
    SubscriberQos sub_qos = SUBSCRIBER_QOS_DEFAULT;
    participant_->get_default_subscriber_qos(sub_qos);
    subscriber_ = participant_->create_subscriber(sub_qos, nullptr, StatusMask::none());
    if (subscriber_ == nullptr)
    {
        throw std::runtime_error("Subscriber initialization failed");
    }

    // Create the topic
    TopicQos topic_qos = TOPIC_QOS_DEFAULT;
    participant_->get_default_topic_qos(topic_qos);
    topic_ = participant_->create_topic(topic_name, type_.get_type_name(), topic_qos);
    if (topic_ == nullptr)
    {
        throw std::runtime_error("Topic initialization failed");
    }

    // Create the reader
    DataReaderQos reader_qos = DATAREADER_QOS_DEFAULT;
    subscriber_->get_default_datareader_qos(reader_qos);
    reader_ = subscriber_->create_datareader(topic_, reader_qos, nullptr, StatusMask::all());
    if (reader_ == nullptr)
    {
        throw std::runtime_error("DataReader initialization failed");
    }

    // Prepare a wait-set
    wait_set_.attach_condition(reader_->get_statuscondition());
    wait_set_.attach_condition(terminate_condition_);
}

void WaitsetSubscriberApp::run()
{
    while (!is_stopped())
    {
        ConditionSeq triggered_conditions;
        ReturnCode_t ret_code = wait_set_.wait(triggered_conditions, eprosima::fastdds::dds::c_TimeInfinite);
        if (RETCODE_OK != ret_code)
        {
            EPROSIMA_LOG_ERROR(SUBSCRIBER_WAITSET, "Error waiting for conditions");
            continue;
        }
        for (Condition* cond : triggered_conditions)
        {
            StatusCondition* status_cond = dynamic_cast<StatusCondition*>(cond);
            if (nullptr != status_cond)
            {
                Entity* entity = status_cond->get_entity();
                StatusMask changed_statuses = entity->get_status_changes();
                if (changed_statuses.is_active(StatusMask::subscription_matched()))
                {
                    SubscriptionMatchedStatus status_;
                    reader_->get_subscription_matched_status(status_);
                    if (status_.current_count_change == 1)
                    {
                        std::cout << "Waitset Subscriber matched." << std::endl;
                    }
                    else if (status_.current_count_change == -1)
                    {
                        std::cout << "Waitset Subscriber unmatched." << std::endl;
                    }
                    else
                    {
                        std::cout << status_.current_count_change <<
                            " is not a valid value for SubscriptionMatchedStatus current count change" <<
                            std::endl;
                    }
                }
                if (changed_statuses.is_active(StatusMask::data_available()))
                {
                    SampleInfo info;
                    while ((!is_stopped()) &&
                            (RETCODE_OK == reader_->take_next_sample(&hello_, &info)))
                    {
                        if ((info.instance_state == ALIVE_INSTANCE_STATE) && info.valid_data)
                        {
                            received_samples_++;
                            // Print Hello world message data
                            std::cout << "Message: '" << hello_.message() << "' with index: '"
                                      << hello_.index() << "' RECEIVED" << std::endl;
                            if (samples_ > 0 && (received_samples_ >= samples_))
                            {
                                stop();
                            }
                        }
                    }
                }
            }
        }
    }
}

```

### 特点：
> 自动发现对端（SPDP/SEDP）  
> 匹配依据：Topic 名 + 类型 + QoS  
> 数据类型自动序列化（IDL + fastrtpsgen）  
> QoS 自动管理

## 基于 RTPS
### 发送端 (Writer)
- RTPSParticipant  
> RTPSParticipantAttributes  
> RTPSDomain::createParticipant  
> HistoryAttributes  
> WriterAttributes   
- 创建 Writer
> RTPSDomain::createRTPSWriter  
> TopicDescription  
> WriterQos  
> register_writer
- 发送数据
> writer_history_->create_change  
> serialize data
> writer_history_->add_change  
```c++
{
    // Create RTPS Participant
    RTPSParticipantAttributes part_attr;
    part_attr.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol::SIMPLE;
    part_attr.builtin.use_WriterLivelinessProtocol = true;
    rtps_participant_ = RTPSDomain::createParticipant(0, part_attr);

    if (rtps_participant_ == nullptr)
    {
        throw std::runtime_error("RTPS Participant creation failed");
    }

    // Writer History Attributes
    HistoryAttributes hatt;
    hatt.payloadMaxSize = 255;
    hatt.maximumReservedCaches = 50;
    writer_history_ = new WriterHistory(hatt);

    // Create RTPS Writer
    WriterAttributes writer_att;
    writer_att.endpoint.reliabilityKind = RELIABLE;
    writer_att.endpoint.durabilityKind = TRANSIENT_LOCAL;

    rtps_writer_ = RTPSDomain::createRTPSWriter(rtps_participant_, writer_att, writer_history_, this);

    if (rtps_writer_ == nullptr)
    {
        throw std::runtime_error("RTPS Writer creation failed");
    }

    std::cout << "Registering RTPS Writer" << std::endl;

    TopicDescription topic_desc;
    topic_desc.type_name = "HelloWorld";
    topic_desc.topic_name = topic_name;

    eprosima::fastdds::dds::WriterQos writer_qos;
    writer_qos.m_durability.kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    writer_qos.m_reliability.kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;

    // Register entity
    if (!rtps_participant_->register_writer(rtps_writer_, topic_desc, writer_qos))
    {
        throw std::runtime_error("Entity registration failed");
    }
}
//sending data
bool WriterApp::add_change_to_history()
{
    bool ret =  false;

    CacheChange_t* ch = writer_history_->create_change(255, ALIVE);

    // In the case history is full, remove some old changes
    if (writer_history_->isFull())
    {
        writer_history_->remove_min_change();
        ch = writer_history_->create_change(255, ALIVE);
    }

    data_->message("Hello World");
    data_->index(data_->index() + 1);

    if (serialize_payload(data_, ch->serializedPayload))
    {
        if (writer_history_->add_change(ch))
        {
            ++samples_sent_;
            ret = true;
        }
        else
        {
            std::cout << "Fail to add the change to the history!" << std::endl;
        }
    }
    else
    {
        std::cout << "Fail to serialize the payload!" << std::endl;
    }

    return ret;
}

```

### 接收端 (Reader)
- RTPSParticipant
> RTPSParticipantAttributes  
> RTPSDomain::createParticipant     
- 创建 Reader (GUID匹配)
> HistoryAttributes  
> ReaderAttributes  
> RTPSDomain::createRTPSReader  
> TopicDescription  
> rtps_participant_->register_reader  
- 接收数据
> 接收原始 payload   
> 反序列化  
> history()->remove_change

``` c++
{
    // Create RTPS Participant
    RTPSParticipantAttributes part_attr;
    part_attr.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol::SIMPLE;
    part_attr.builtin.use_WriterLivelinessProtocol = true;
    rtps_participant_ = RTPSDomain::createParticipant(0, part_attr);

    if (rtps_participant_ == nullptr)
    {
        throw std::runtime_error("RTPS Participant creation failed");
    }

    // Reader History Attributes
    HistoryAttributes hatt;
    hatt.payloadMaxSize = 255;
    reader_history_ = new ReaderHistory(hatt);

    // Create RTPS Reader
    ReaderAttributes reader_att;
    reader_att.endpoint.reliabilityKind = RELIABLE;
    reader_att.endpoint.durabilityKind = TRANSIENT_LOCAL;

    rtps_reader_ = RTPSDomain::createRTPSReader(rtps_participant_, reader_att, reader_history_, this);

    if (rtps_reader_ == nullptr)
    {
        throw std::runtime_error("RTPS Reader creation failed");
    }

    std::cout << "Registering RTPS Reader" << std::endl;

    TopicDescription topic_desc;
    topic_desc.topic_name = topic_name;
    topic_desc.type_name = "HelloWorld";

    eprosima::fastdds::dds::ReaderQos reader_qos;
    reader_qos.m_durability.kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    reader_qos.m_reliability.kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;

    // Register entity
    if (!rtps_participant_->register_reader(rtps_reader_, topic_desc, reader_qos))
    {
        throw std::runtime_error("Entity registration failed");
    }
}

void ReaderApp::on_new_cache_change_added(
        RTPSReader* reader,
        const CacheChange_t* const change)
{
    if (!is_stopped())
    {
        if (deserialize_payload(change->serializedPayload, data_))
        {
            std::cout << "Message: " << data_->message() << " with index " <<  data_->index() << " RECEIVED" <<
                std::endl;
            samples_received_++;
        }
        else
        {
            std::cout << "Message: not deserialized" << std::endl;
        }

        reader->get_history()->remove_change((CacheChange_t*)change);

        if ((samples_ > 0) && (samples_received_ >= samples_))
        {
            stop();
        }
    }
}

``` 
### 特点：
> 必须手动管理 GUID、Locator（IP+端口）、缓存、可靠性机制  
> 没有 Topic / QoS 自动匹配，需应用层自己控制  
> 更接近 socket 编程（不过依然走 RTPS 协议）  
> 适用于精简嵌入式或自定义实时协议场景


