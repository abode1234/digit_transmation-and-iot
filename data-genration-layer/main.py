import json, random, time
import paho.mqtt.client as mqtt

BROKER = "localhost"
PORT = 1883
TOPIC = "t"

def generate_payload():
    data = {
        "timestamp": int(time.time()),
        "temperature": round(random.uniform(20, 80), 2),
        "pressure": round(random.uniform(900, 1100), 2),
        "humidity": round(random.uniform(0, 100), 2)
    }
    return json.dumps(data)


def main():
    client = mqtt.Client()
    client.connect(BROKER, PORT)
    while True:
        payload = generate_payload()
        client.publish(TOPIC, payload)
        print(f"Published: {payload}")
        time.sleep(5)

if __name__ == "__main__":
    main()
