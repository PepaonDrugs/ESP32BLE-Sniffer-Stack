FROM python:alpine

ARG VERSION
ARG BUILD_DATE


ENV TOPIC=""
ENV MQTT_BROKER=""
ENV INFLUX_HOST=""
ENV INFLUX_PORT=""
ENV INFLUX_USER=""
ENV INFLUX_PASS=""
ENV INFLUX_DATABASE=""

RUN apt-get update && apt-get install -y libpq-dev build-essential


COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY ESP32BLE-Sniffer.py ./

CMD [ "python", "./ESP32BLE-Sniffer.py" ]
