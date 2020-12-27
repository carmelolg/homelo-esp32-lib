#ifndef Homelo_h
#define Homelo_h

#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <stdio.h>
#include <string.h>

class Homelo {
  public:
    Homelo(const char* ssid, const char* ssidPwd, String username, String password);
    void send(String json);
    String getRoom();
    String auth();
    void wifiCheck();
    void wifiConnect();
  private:
    // wifi ssid
    const char* _ssid;
    // wifi password
    const char* _ssidPwd;
    // shield username
    String _username;
    // shield password
    String _password;
    // Auth JWT
    String _jwt;
	
	// get jwt from http result
	String getJwt(String payload);
};

#endif
