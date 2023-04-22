import paho.mqtt.client as mqtt
import json
import os
from influxdb import InfluxDBClient

# Define the MQTT topic and broker address
topic = os.environ['TOPIC']
broker_address = os.environ['MQTT_BROKER']

# Define InfluxDB settings
influx_host = os.environ['INFLUX_HOST']
influx_port = os.environ['INFLUX_PORT']
influx_user = os.environ['INFLUX_USER']
influx_pass = os.environ['INFLUX_PASS']
influx_db = os.environ['INFLUX_DATABASE']

# Create an InfluxDB client instance
influx_client = InfluxDBClient(host=influx_host, port=influx_port, username=influx_user, password=influx_pass, database=influx_db)

# Define a callback function to handle incoming MQTT messages
def on_message(client, userdata, message):
    # Convert the message payload from bytes to string
    json_data = message.payload.decode('utf-8')
    # Load the JSON data into a Python dictionary
    data = json.loads(json_data)
    # Count the number of devices in the JSON data
    num_devices = len(data["Results"])
    print("There are", num_devices, "devices around.")
    # Extract the RSSI values for each device in the JSON data
    rssi_values = []
    for device in data["Results"]:
        rssi = data["Results"][device].get("rssi")
        if rssi is not None:
            rssi_values.append(rssi)
            print("RSSI value for device", device, ":", rssi)
    # Calculate the highest, lowest, and average RSSI
    if rssi_values:
        highest_rssi = max(rssi_values)
        lowest_rssi = min(rssi_values)
        avg_rssi = sum(rssi_values) / len(rssi_values)
        print("Highest RSSI:", highest_rssi)
        print("Lowest RSSI:", lowest_rssi)
        print("Average RSSI:", avg_rssi)
        # Write the data to InfluxDB
        data_points = [
            {
                "measurement": "device_count",
                "fields": {
                    "value": num_devices
                }
            },
            {
                "measurement": "rssi_min",
                "fields": {
                    "value": lowest_rssi,
                }
            },
            {
                "measurement": "rssi_avg",
                "fields": {
                    "value": avg_rssi,
                }
            },
            {
                "measurement": "rssi_max",
                "fields": {
                    "value": highest_rssi,
                }
            }
        ]
        influx_client.write_points(data_points)

# Create a new MQTT client instance
client = mqtt.Client()

# Set the on_message callback function for the client
client.on_message = on_message

# Connect to the MQTT broker
client.connect(broker_address)

# Subscribe to the MQTT topic
client.subscribe(topic)

# Start the MQTT client loop to listen for incoming messages
client.loop_forever()
