
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ctime>
#include <string.h> 

using namespace std;

const char *ssid = "Breez";
const char *password = "breezybug133";
const char *ID = "Enequi-Master";
const char *TOPIC = "pbs/NG/A1/CurrentLimit";
const int port = 1883;

IPAddress broker(192,168,1,8);
WiFiClient wclient;
PubSubClient client(wclient);

int L1 = 0;
int L2 = 0;
int L3 = 0;

// Connect to WiFi network
void setup_wifi()
{
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  { 
    // Wait for connection
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
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
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
      delay(5000);
    }
  }
}

int randomize(int lastNumber)
{
  while (true) {
    int randomNumber = (rand() %200) + 1;
    if(randomNumber - lastNumber < 30 || lastNumber - randomNumber < 30)
      return randomNumber;
  }
  return 0;
}

void createAndSendPhases() 
{
  L1 = randomize(L1);
  L2 = randomize(L2);
  L3 = randomize(L3);

  String temp = String(L1);
  temp += ',';
  temp += String(L2);
  temp += ',';
  temp += String(L3);

  char* payload = new char [temp.length() + 1];
  strcpy(payload, temp.c_str());

  client.publish(TOPIC, payload);

}
void setup()
{
  Serial.begin(9600);
  delay(100);
  setup_wifi();
  client.setServer(broker, port);
  srand(time(NULL));
}

void loop()
{
  
  if (!client.connected())
    reconnect();
  
  client.loop();

  createAndSendPhases();
  delay(5000);
}
