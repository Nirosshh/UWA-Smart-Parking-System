from flask import Flask, render_template
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt

app = Flask(__name__)
socketio = SocketIO(app)

# MQTT Settings
MQTT_BROKER = "mqtt.eclipseprojects.io"
MQTT_PORT = 1883
MQTT_TOPIC = "nirosshh/status"

# Store the status of parking slots
parking_status = {"1": "green", "2": "green"}

# MQTT callback
def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        print(f"Received: {payload}")
        slave_id, value = payload.strip().split(":")
        status = "red" if value == "1" else "green"
        parking_status[slave_id] = status
        socketio.emit("status_update", parking_status)
    except Exception as e:
        print("Error:", e)

# MQTT Setup
mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.subscribe(MQTT_TOPIC)
mqtt_client.loop_start()

@app.route("/")
def index():
    return render_template("index.html", status=parking_status)

if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=5000)
