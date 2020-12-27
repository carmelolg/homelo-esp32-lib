#include "Homelo.h"

Homelo::Homelo(const char* ssid, const char* ssidPwd, String username, String password){
  _ssid = ssid;
  _ssidPwd = ssidPwd;
  _username = username;
  _password = password;
}

void Homelo::send(String json){
	HTTPClient http;
	String url = "https://homelo-iot-receiver.herokuapp.com/detection";
	http.begin(url.c_str());
	  
	http.addHeader("Content-Type", "application/json");
	http.addHeader("Authorization", "Bearer " + _jwt);

	Serial.println(json);
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

String Homelo::getRoom() {
	HTTPClient http;
	String url = "https://homelo-iot-receiver.herokuapp.com/sensor?name=" + _username;
	http.begin(url.c_str());

	http.addHeader("Authorization", "Bearer " + _jwt);
	int httpResponseCode = http.GET();

	if (httpResponseCode < 400) {
		String payload = http.getString();

		StaticJsonDocument<500> doc;
		deserializeJson(doc, payload);

		Serial.println(payload);
    
		const char* _room = doc[0]["room"];
		Serial.print("I'm in the room: ");
		Serial.println(_room);
		
		return _room;
	} else {
		Serial.println("Sensor data are not available");
	}
}

String Homelo::auth() {
	HTTPClient http;
	String url = "https://homelo-iot-receiver.herokuapp.com/auth?username=" + _username + "&password=" + _password;
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