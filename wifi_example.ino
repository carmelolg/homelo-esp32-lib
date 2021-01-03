#include <Homelo.h>

static const String USER          = "esp32-test";
static const String PASSWORD      = "esp32-test";
static const char* SSID_WIFI      = "Vodafone-58160126";
static const char* SSID_PASSWORD  = "udkdjh2h92kaj5m";
String ROOM                       = "";
String HOUSE                      = "";

Homelo homelo(SSID_WIFI, SSID_PASSWORD, USER, PASSWORD);

void setup() {

  Serial.begin(9600);

  // Connect to wifi
  homelo.wifiConnect();

  // Init and retrieve general info
  homelo.init();

  // Get current room and house
  ROOM = homelo.getRoom();
  HOUSE = homelo.getHouse();

  Serial.print("Alarm enabled: ");
  Serial.println(homelo.isAlarmEnabled());

  Serial.println("I'm in the house: " + HOUSE);
  Serial.println("I'm in the room: " + ROOM);
}

void loop() {

  // Check wifi
  homelo.wifiCheck();

  // Re-auth every 50 minutes
  _refreshTokenLoop();

  Serial.println("I'm alive");

  delay(1000);
}

unsigned long lastMillis;
void _refreshTokenLoop() {
  if (millis() - lastMillis >= 50 * 60 * 1000UL) {
    lastMillis = millis();
    String _jwt = homelo.auth();
    Serial.println("Refresh token. New token: " + _jwt);
  }
}
