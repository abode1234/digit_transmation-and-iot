#include <iostream>
#include <cstdlib>                             // getenv
#include <mqtt/async_client.h>
#include <azureiot/iothub.h>                   // IoTHub_Init/Deinit
#include <azureiot/iothub_device_client_ll.h>  // Device client
#include <azureiot/iothubtransportmqtt.h>      // MQTT transport

static const std::string LOCAL_BROKER     = "tcp://localhost:1883";
static const std::string TOPIC            = "oilsense/demo";
static const std::string IOT_HUB_HOSTNAME = std::getenv("IOT_HUB_HOSTNAME");
static const std::string DEVICE_ID        = std::getenv("DEVICE_ID");

// PEM strings linked in via CMake (see CMakeLists.txt)
extern const char* x509_cert;
extern const char* x509_key;
extern const char* root_ca;

void send_to_iothub(const std::string& msg) {
    // Initialize the Azure IoT SDK
    IoTHub_Init();

    // Create the device client using X.509 auth
    IOTHUB_DEVICE_CLIENT_LL_HANDLE client =
        IoTHubDeviceClient_LL_CreateFromDeviceAuth(
            IOT_HUB_HOSTNAME.c_str(),  // must pass C-string
            DEVICE_ID.c_str(),
            MQTT_Protocol
        );

    // Configure TLS certificates
    IoTHubDeviceClient_LL_SetOption(client, "TrustedCerts", root_ca);
    IoTHubDeviceClient_LL_SetOption(client, "x509certificate", x509_cert);
    IoTHubDeviceClient_LL_SetOption(client, "x509privatekey", x509_key);

    // Send the JSON payload
    IOTHUB_MESSAGE_HANDLE message = IoTHubMessage_CreateFromString(msg.c_str());
    IoTHubDeviceClient_LL_SendEventAsync(client, message, nullptr, nullptr);
    IoTHubMessage_Destroy(message);

    // Perform the work loop and clean up
    IoTHubDeviceClient_LL_DoWork(client);
    IoTHubDeviceClient_LL_Destroy(client);
    IoTHub_Deinit();
}

int main() {
    // Connect to local MQTT broker
    mqtt::async_client client(LOCAL_BROKER, "");
    auto connOpts = mqtt::connect_options_builder().clean_start().finalize();
    client.connect(connOpts)->wait();
    client.start_consuming();
    client.subscribe(TOPIC, 1)->wait();

    // Forward loop
    while (true) {
        auto msg = client.consume_message();
        if (msg) {
            auto payload = msg->to_string();
            std::cout << "Received: " << payload << std::endl;
            send_to_iothub(payload);
        }
    }

    client.disconnect()->wait();
    return 0;
}

