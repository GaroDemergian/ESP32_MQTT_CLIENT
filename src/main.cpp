#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "------";      // name of your WiFi network
const char *password = "-------"; // password of the WiFi network
const char *ID = "esp32";         // Name of our device, must be unique
const char *TOPIC = "button";     // Topic to subcribe to
const int port = 1883;
const byte SWITCH_PIN = 0; // Pin to control the light with
bool state = 0;

IPAddress broker(192, 168, 1, 78); // IP address of your MQTT broker eg. 192.168.1.78
WiFiClient wclient;
PubSubClient client(wclient); // Setup MQTT client

// Connect to WiFi network
void setup_wifi()
{
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED)
  { // Wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to client
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID))
    {
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');
    }
    else
    {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);             // Start serial communication at 115200 baud
  pinMode(SWITCH_PIN, INPUT);     // Configure SWITCH_Pin as an input
  digitalWrite(SWITCH_PIN, HIGH); // enable pull-up resistor (active low)
  delay(100);
  setup_wifi(); // Connect to network
  client.setServer(broker, port);
}

void loop()
{
  if (!client.connected()) // Reconnect if connection is lost
  {
    reconnect();
  }
  client.loop();

  // if the switch is being pressed
  if (digitalRead(SWITCH_PIN) == 0)
  {
    state = !state; //toggle state
    if (state == 1) // ON
    {
      client.publish(TOPIC, "on");
      Serial.println((String)TOPIC + " => on");
    }
    else // OFF
    {
      client.publish(TOPIC, "off");
      Serial.println((String)TOPIC + " => off");
    }

    while (digitalRead(SWITCH_PIN) == 0) // Wait for switch to be released
    {
      // Let the ESP handle some behind the scenes stuff if it needs to
      yield();
      delay(20);
    }
  }
}
