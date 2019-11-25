import time
import ttn

app_id = "theultimateapplication"
access_key = "ttn-account-v2.gGuc6mi3k6sF6W19mIYw1cOvkMjfMLlseSOT6LPtXRw"

def uplink_callback(msg, client):
    print("Received uplink from ", msg.dev_id)
    print("counter: ", msg.counter)
    print("Humidity is ", msg.payload_fields.humid)
    print("Teampriture is ", msg.payload_fields.temp)

handler = ttn.HandlerClient(app_id, access_key)

mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()
time.sleep(60)
mqtt_client.close()

