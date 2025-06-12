cmake_minimum_required(VERSION 3.10)
project(oilsense_edge)

find_package(PahoMqttCpp REQUIRED)

# Locate Azure IoT C SDK headers
find_path(AZ_IOT_INC_DIR
    NAMES iothub_device_client_ll.h
    HINTS /usr/local/include/azureiot /usr/local/include /usr/include
)
if (NOT AZ_IOT_INC_DIR)
  message(FATAL_ERROR "Azure IoT C SDK headers not found.")
endif()
include_directories(${AZ_IOT_INC_DIR})

# Locate the Azure IoT SDK library
find_library(IOTHUB_CLIENT_LIB
    NAMES iothub_client
    HINTS /usr/local/lib /usr/lib
)
if (NOT IOTHUB_CLIENT_LIB)
  message(FATAL_ERROR "libiothub_client not found.")
endif()
link_directories(/usr/local/lib)

add_executable(mqtt_forward mqtt_forward.cpp)

target_link_libraries(mqtt_forward
    PahoMqttCpp::paho-mqttpp3
    ${IOTHUB_CLIENT_LIB}
    uamqp
    aziotsharedutil
)

