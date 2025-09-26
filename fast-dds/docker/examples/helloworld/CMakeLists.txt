cmake_minimum_required(VERSION 3.16)

project(HelloWorld)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

# 查找Fast-DDS
find_package(fastdds REQUIRED)

# 查找FastCDR
find_package(fastcdr REQUIRED)

# 设置IDL文件路径
set(IDL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld.idl)

# 生成IDL相关文件
set(GENERATED_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldPubSubTypes.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldPubSubTypes.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorld.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldTypeObjectSupport.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldTypeObjectSupport.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldCdrAux.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldCdrAux.ipp
)

# 使用fastddsgen生成代码
add_custom_command(
    OUTPUT ${GENERATED_FILES}
    COMMAND fastddsgen -replace -d ${CMAKE_CURRENT_BINARY_DIR} ${IDL_FILE}
    DEPENDS ${IDL_FILE}
    COMMENT "Generating Fast-DDS code from IDL"
    VERBATIM
)

# 创建生成文件的库
add_library(${PROJECT_NAME}_types 
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldPubSubTypes.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/HelloWorldTypeObjectSupport.cxx
)

# 设置包含目录
target_include_directories(${PROJECT_NAME}_types PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FastDDS_INCLUDE_DIRS}
    ${fastcdr_INCLUDE_DIRS}
)

# 链接Fast-DDS和FastCDR库
target_link_libraries(${PROJECT_NAME}_types
    fastdds
    fastcdr
)

# 创建Publisher可执行文件
add_executable(HelloWorldPublisher
    publisher.cpp
)

target_link_libraries(HelloWorldPublisher
    ${PROJECT_NAME}_types
    fastdds
    fastcdr
)

target_include_directories(HelloWorldPublisher PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FastDDS_INCLUDE_DIRS}
    ${fastcdr_INCLUDE_DIRS}
)

# 创建Subscriber可执行文件
add_executable(HelloWorldSubscriber
    subscriber.cpp
)

target_link_libraries(HelloWorldSubscriber
    ${PROJECT_NAME}_types
    fastdds
    fastcdr
)

target_include_directories(HelloWorldSubscriber PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FastDDS_INCLUDE_DIRS}
    ${fastcdr_INCLUDE_DIRS}
)

# Copy the XML files over to the build directory
file(GLOB_RECURSE XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.xml)

message("Found XML files: ${XML_FILES}")

# for each xml file detected
foreach(XML_FILE_COMPLETE_PATH ${XML_FILES})
    # obtain the file name
    get_filename_component(XML_FILE ${XML_FILE_COMPLETE_PATH} NAME_WE)
    message("Copy XML file to : ${CMAKE_INSTALL_PREFIX}")
    # copy the file from src to build folders
    configure_file(
        ${XML_FILE_COMPLETE_PATH}                   # from full src path
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${XML_FILE}.xml # to relative build path
        COPYONLY)
    install(FILES ${XML_FILE_COMPLETE_PATH} DESTINATION bin)
endforeach()

# 设置编译选项
target_compile_options(${PROJECT_NAME}_types PRIVATE -Wall -Wextra)
target_compile_options(HelloWorldPublisher PRIVATE -Wall -Wextra)
target_compile_options(HelloWorldSubscriber PRIVATE -Wall -Wextra)

# 安装目标
install(TARGETS HelloWorldPublisher HelloWorldSubscriber
    RUNTIME DESTINATION bin
)

