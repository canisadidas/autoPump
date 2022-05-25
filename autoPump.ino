#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define pinPump D6 // По восходящему фронту
#define pinSensor D7
#define work 2000 // 2 секунды

bool sensorVal, state = false;
unsigned long periodTimer;
long period = 86400;

const char* ssid = "NodeMCU";
const char* password = "12345678";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  pinMode(pinPump, OUTPUT);
  pinMode(pinSensor, INPUT);

  server.on("/", handle_OnConnect);
  server.on("/changeTimeTenSec", handle_changeTimeTenSec);
  server.on("/changeTimeThirtySec", handle_changeTimeThirtySec);
  server.on("/changeTimeThiryMinutes", handle_changeTimeThiryMinutes);
  server.on("/changeTimeOneDay", handle_changeTimeOneDay);

  server.begin();

  Serial.println("Server started!");
}

void loop() {
  server.handleClient();
  if (!state) {
    if (millis() - periodTimer > period) {
      periodTimer = millis();
      state = true;
      digitalWrite(pinPump, HIGH);
    }
  }
  else {
    if (millis() - periodTimer > work) {
      periodTimer = millis();
      state = false;
      digitalWrite(pinPump, LOW);
    }
  }
}

void handle_OnConnect() {
  sensorVal = digitalRead(pinSensor);
  server.send(200, "text/html", SendHTML(sensorVal, state)); 
}

void handle_changeTimeTenSec() {
  period = 10000;
  server.send(200, "text/html", SendHTML(sensorVal, state)); 
}

void handle_changeTimeThirtySec() {
  period = 30000;
  server.send(200, "text/html", SendHTML(sensorVal, state)); 
}

void handle_changeTimeThiryMinutes() {
  period = 1800000;
  server.send(200, "text/html", SendHTML(sensorVal, state)); 
}

void handle_changeTimeOneDay() {
  period = 86400000;
  server.send(200, "text/html", SendHTML(sensorVal, state)); 
}

String SendHTML(bool sensorValStat, bool stateStat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<meta http-equiv=\"refresh\" content=\"1\">\n";
  ptr +="<title>Pump Status</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +="p {font-size: 20px;color: #888;margin-bottom: 10px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 10px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  if (!sensorValStat) {
    ptr +="<p>Water status: None!</p>\n";
  }
  else {
    ptr +="<p>Water status: Yes!</p>\n";
  }
  
  if (!stateStat) {
    ptr +="<p>Pump status: Off!</p>\n";
  }
  else {
    ptr +="<p>Pump status: On!</p>\n";
  }

  ptr +="<p>Choose period</p>\n";
  ptr +="<a class=\"button button-on\" href=\"/changeTimeTenSec\">10 Sec</a>";
  ptr +="<a class=\"button button-on\" href=\"/changeTimeThirtySec\">30 Sec</a>\n";
  ptr +="<a class=\"button button-on\" href=\"/changeTimeThiryMinutes\">30 Min</a>";
  ptr +="<a class=\"button button-on\" href=\"/changeTimeOneDay\">1 Day</a>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
