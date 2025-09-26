#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

#include "HelloWorldPubSubTypes.hpp"
#include "HelloWorld.hpp"

#include <chrono>
#include <thread>
#include <iostream>
#include <string>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

class HelloWorldPublisher
{
private:

    HelloWorld::HelloWorld hello_;

    DomainParticipant* participant_;

    Publisher* publisher_;

    Topic* topic_;

    DataWriter* writer_;

    TypeSupport type_;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener()
            : matched_(0)
            , first_connected_(false)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                first_connected_ = true;
                std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
            }
        }

        std::atomic_int matched_;

        bool first_connected_;

    } listener_;

public:

    HelloWorldPublisher()
        : participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , writer_(nullptr)
        , type_(new HelloWorld::HelloWorldPubSubType())
    {
    }

    virtual ~HelloWorldPublisher()
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the publisher
    bool init()
    {
        hello_.index(0);
        hello_.message("Hello World!");

        // 从配置文件加载配置
        std::string profile_file = "profile.xml";

        if (DomainParticipantFactory::get_instance()->load_XML_profiles_file(profile_file) != RETCODE_OK)
        {
            std::cerr << "Failed to load XML profiles file" << std::endl;
            return false;
        }
        std::cout << "Loading configuration from: " << profile_file << std::endl;
        
        // 使用配置文件创建参与者
        participant_ = DomainParticipantFactory::get_instance()->create_participant_with_profile(0, "HelloWorldPublisherProfile");

        if (participant_ == nullptr)
        {
            std::cerr << "Failed to create participant with profile" << std::endl;
            return false;
        }

        std::cout << "Participant created successfully with profile" << std::endl;

        // Register the Type
        type_.register_type(participant_);

        // Create the publications Topic
        topic_ = participant_->create_topic("HelloWorldTopic", type_.get_type_name(), TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            std::cerr << "Failed to create topic" << std::endl;
            return false;
        }

        std::cout << "Topic created successfully" << std::endl;

        // Create the Publisher using profile
        publisher_ = participant_->create_publisher_with_profile("HelloWorldPublisher");

        if (publisher_ == nullptr)
        {
            std::cerr << "Failed to create publisher with profile" << std::endl;
            return false;
        }

        std::cout << "Publisher created successfully with profile" << std::endl;

        // Create the DataWriter using profile
        writer_ = publisher_->create_datawriter_with_profile(topic_, "HelloWorldDataWriter", &listener_);

        if (writer_ == nullptr)
        {
            std::cerr << "Failed to create datawriter with profile" << std::endl;
            return false;
        }

        std::cout << "DataWriter created successfully with profile" << std::endl;
        return true;
    }

    //!Send a publication
    bool publish()
    {
        if (listener_.first_connected_ || listener_.matched_ > 0)
        {
            hello_.index(hello_.index() + 1);
            hello_.message("Hello World! Message #" + std::to_string(hello_.index()));
            int ret = writer_->write(&hello_);
            if (ret != RETCODE_OK)
            {
                std::cout << "Failed to write sample!" << std::endl;
                return false;
            }
            std::cout << "Sending: " << hello_.message() << " (index: " << hello_.index() << ")" << std::endl;
            return true;
        }
        return false;
    }

    //!Run the Publisher
    void run(uint32_t samples)
    {
        uint32_t samples_sent = 0;
        while (samples_sent < samples)
        {
            if (publish())
            {
                samples_sent++;
                std::cout << "Publishing sample " << samples_sent << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

int main(int argc, char** argv)
{
    std::cout << "Starting publisher." << std::endl;
    int samples = 10;

    if (argc > 1)
    {
        samples = atoi(argv[1]);
    }

    HelloWorldPublisher* mypub = new HelloWorldPublisher();
    if (mypub->init())
    {
        mypub->run(static_cast<uint32_t>(samples));
    }

    delete mypub;
    return 0;
}
