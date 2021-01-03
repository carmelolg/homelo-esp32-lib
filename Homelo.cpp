#include "Homelo.h"

static String BASE_URL = "https://homelo-api-gateway.herokuapp.com/";

Homelo::Homelo(const char* ssid, const char* ssidPwd, String username, String password){
  _ssid = ssid;
  _ssidPwd = ssidPwd;
  _username = username;
  _password = password;
}

void Homelo::send(String json){
	HTTPClient http;
	String url = BASE_URL + "detection";
	http.begin(url.c_str());
	  
	http.addHeader("Content-Type", "application/json");
	http.addHeader("Authorization", "Bearer " + _jwt);

	Serial.println("Sending data: " + json);
	int httpResponseCode = http.POST(json);
	if (httpResponseCode < 400) {
		Serial.println("Data sent.");
	} else {
		Serial.print("Error code: ");
		Serial.println(httpResponseCode);
	}

	// Free resources
	http.end();
}

void Homelo::init() {
	HTTPClient http;
	String url = BASE_URL + "sensor?name=" + _username;
	http.begin(url.c_str());

	http.addHeader("Authorization", "Bearer " + _jwt);
	int httpResponseCode = http.GET();

	if (httpResponseCode < 400) {
		String payload = http.getString();

		StaticJsonDocument<500> doc;
		deserializeJson(doc, payload);
    
		const char* _room = doc[0]["room"];
		const char* _house = doc[0]["house"];
		
		room = _room;
		house = _house;
		
	} else {
		Serial.println("Sensor data are not available");
	}
}

bool Homelo::isAlarmEnabled() {
	HTTPClient http;
	String url = BASE_URL + "alarm?house=" + house;
	http.begin(url.c_str());

	http.addHeader("Authorization", "Bearer " + _jwt);
	int httpResponseCode = http.GET();

	if (httpResponseCode < 400) {
		String payload = http.getString();

		if(payload == "true") {
			return true;
		}else {
			return false;
		}
		
	} else {
		Serial.println("Alarm enabled retrieve error");
	}
}

String Homelo::getRoom() {
	return room;
}

String Homelo::getHouse() {
	return house;
}

String Homelo::auth() {
	HTTPClient http;
	String url = BASE_URL + "auth?username=" + _username + "&password=" + _password;
	http.begin(url.c_str());	
	http.addHeader("Content-Type", "application/json");
	
	int httpResponseCode = http.POST("");
	
	if (httpResponseCode < 400) {
		String payload = http.getString();
		
		_jwt = getJwt(payload);
		Serial.println(_username + " authenticated correctly to Homelo suit. The token is: " + _jwt);
		return _jwt;
	} else {
		Serial.println("Authentication to Homelo suite failed");
	}
}

String Homelo::getJwt(String payload){
  
  char* _skip = " ,;:\"'?!{}";
  int len = payload.length() + 1; 
  char _buffer[len];
  payload.toCharArray(_buffer, len);
  
  char* token = strtok(_buffer,_skip);
  return strtok(NULL,_skip);
}

void Homelo::wifiCheck(){
	if (WiFi.status() != WL_CONNECTED) {
		wifiConnect();
	}
}
    
void Homelo::wifiConnect() {
	
	Serial.print("Connecting to ");
	Serial.print(_ssid);
	Serial.print(" ");
	WiFi.begin(_ssid, _ssidPwd);
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	
	// Auth
	auth();
}