#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define meja1 D1
#define meja2 D2

const char* ssid = "kos1";
const char* password = "kosanmantap@gmail.com";
const char* mqtt_server = "192.168.1.6";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

String subscribe = "iot/meja";

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String payloadString = "";
  for (int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }
  Serial.println(payloadString);

  // meja(deviceId)_{on,off}
  if (payloadString == "meja1_on") {
    digitalWrite(meja1, LOW);
    Serial.println("Meja 1 hidup");
  } else if (payloadString == "meja1_off") {
    digitalWrite(meja1, HIGH);
    Serial.println("Meja 1 mati");
  } else if (payloadString == "meja2_on") {
    digitalWrite(meja2, LOW);
    Serial.println("Meja 2 hidup");
  } else if (payloadString == "meja2_off") {
    digitalWrite(meja2, HIGH);
    Serial.println("Meja 2 mati");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("iot/meja", "meja1_off");
      client.subscribe(subscribe.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(meja1, OUTPUT);
  pinMode(meja2, OUTPUT);
  digitalWrite(meja1, 1);
  digitalWrite(meja2, 1);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  }
}
