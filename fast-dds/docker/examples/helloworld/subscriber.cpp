#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include "HelloWorldPubSubTypes.hpp"
#include "HelloWorld.hpp"

#include <chrono>
#include <thread>
#include <iostream>
#include <string>

using namespace eprosima::fastdds::dds;

class HelloWorldSubscriber
{
private:

    HelloWorld::HelloWorld hello_;

    DomainParticipant* participant_;

    Subscriber* subscriber_;

    DataReader* reader_;

    Topic* topic_;

    TypeSupport type_;

    class SubListener : public DataReaderListener
    {
    public:

        SubListener(HelloWorldSubscriber* subscriber)
            : samples_(0)
            , subscriber_(subscriber)
        {
        }

        ~SubListener() override
        {
        }

        void on_data_available(DataReader* reader) override
        {
            std::cout << "on_data_available called!" << std::endl;
            SampleInfo info;
            if (reader->take_next_sample(&subscriber_->hello_, &info) == RETCODE_OK)
            {
                std::cout << "Sample taken successfully, valid_data: " << info.valid_data << std::endl;
                if (info.valid_data)
                {
                    samples_++;
                    std::cout << "Message: " << subscriber_->hello_.message() << " with index: " << subscriber_->hello_.index()
                            << " RECEIVED." << std::endl;
                }
            }
            else
            {
                std::cout << "Failed to take sample!" << std::endl;
            }
        }

        void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                std::cout << "Subscriber matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                std::cout << "Subscriber unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                          << " is not a valid value for SubscriptionMatchedStatus current_count_change" << std::endl;
            }
        }

        std::atomic_int samples_;
        HelloWorldSubscriber* subscriber_;

    } listener_;

public:

    HelloWorldSubscriber()
        : participant_(nullptr)
        , subscriber_(nullptr)
        , reader_(nullptr)
        , topic_(nullptr)
        , type_(new HelloWorld::HelloWorldPubSubType())
        , listener_(this)
    {
    }

    virtual ~HelloWorldSubscriber()
    {
        if (reader_ != nullptr)
        {
            subscriber_->delete_datareader(reader_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        if (subscriber_ != nullptr)
        {
            participant_->delete_subscriber(subscriber_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the subscriber
    bool init()
    {
        // 从配置文件加载配置
        std::string profile_file = "profile.xml";

        if (DomainParticipantFactory::get_instance()->load_XML_profiles_file(profile_file) != RETCODE_OK)
        {
            std::cerr << "Failed to load XML profiles file" << std::endl;
            return false;
        }
        std::cout << "Loading configuration from: " << profile_file << std::endl;
        
        // 使用配置文件创建参与者
        participant_ = DomainParticipantFactory::get_instance()->create_participant_with_profile(
            0, "HelloWorldSubscriberProfile");

        if (participant_ == nullptr)
        {
            std::cerr << "Failed to create participant with profile" << std::endl;
            return false;
        }

        std::cout << "Participant created successfully with profile" << std::endl;

        // Register the Type
        type_.register_type(participant_);

        // Create the subscriptions Topic
        topic_ = participant_->create_topic("HelloWorldTopic", type_.get_type_name(), TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            std::cerr << "Failed to create topic" << std::endl;
            return false;
        }

        std::cout << "Topic created successfully" << std::endl;

        // Create the Subscriber using profile
        subscriber_ = participant_->create_subscriber_with_profile("HelloWorldSubscriber");

        if (subscriber_ == nullptr)
        {
            std::cerr << "Failed to create subscriber with profile" << std::endl;
            return false;
        }

        std::cout << "Subscriber created successfully with profile" << std::endl;

        // Create the DataReader using profile
        reader_ = subscriber_->create_datareader_with_profile(topic_, "HelloWorldDataReader", &listener_);

        if (reader_ == nullptr)
        {
            std::cerr << "Failed to create datareader with profile" << std::endl;
            return false;
        }

        std::cout << "DataReader created successfully with profile" << std::endl;
        return true;
    }

    //!Run the Subscriber
    void run(uint32_t samples)
    {
        std::cout << "Waiting for " << samples << " samples..." << std::endl;

        while (listener_.samples_ < samples) // 最多等待10秒
        {
            // 主动读取数据
            SampleInfo info;
            ReturnCode_t ret = reader_->take_next_sample(&hello_, &info);
            if (ret == RETCODE_OK)
            {
                if (info.valid_data)
                {
                    listener_.samples_++;
                    std::cout << "Message: " << hello_.message() << " with index: " << hello_.index()
                            << " RECEIVED." << std::endl;
                }
                else
                {
                    std::cout << "Sample received but not valid data" << std::endl;
                }
            }
            else if (ret == RETCODE_NO_DATA)
            {
                // 没有数据，继续等待
            }
            else
            {
                std::cout << "Error taking sample: " << ret << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Received " << listener_.samples_ << " samples. Exiting." << std::endl;
    }
};

int main( int argc, char** argv)
{
    std::cout << "Starting subscriber." << std::endl;
    int samples = 10;

    if (argc > 1)
    {
        samples = atoi(argv[1]);
    }

    HelloWorldSubscriber* mysub = new HelloWorldSubscriber();
    if (mysub->init())
    {
        std::cout << "Subscriber initialized successfully." << std::endl;
        mysub->run(static_cast<uint32_t>(samples));
    }
    else
    {
        std::cout << "Failed to initialize subscriber." << std::endl;
    }

    delete mysub;
    return 0;
}
