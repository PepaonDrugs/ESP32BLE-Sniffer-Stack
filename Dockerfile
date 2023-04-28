FROM python:alpine

ARG VERSION
ARG BUILD_DATE

ENV LOG_LEVEL="DEBUG"


ENV TOPIC="ble/report"
ENV MQTT_BROKER=""
ENV INFLUX_HOST=""
ENV INFLUX_PORT=""
ENV INFLUX_USER=""
ENV INFLUX_PASS=""
ENV INFLUX_DATABASE=""
ENV MQTT_PORT="1883"
ENV MQTT_USER=""
ENV MQTT_PASSWORD=""




COPY requirements.txt ./
RUN pip3 install --no-cache-dir -r requirements.txt

COPY ESP32BLE-Sniffer.py ./

CMD [ "python", "./ESP32BLE-Sniffer.py" ]
