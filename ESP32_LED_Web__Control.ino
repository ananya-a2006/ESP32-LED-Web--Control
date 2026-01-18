#include <WiFi.h>

const char* ssid = "YOUR WIFI NAME";
const char* password = "YOUR WIFI PASSWORD";

// External LED on D2 (GPIO2)
const int ledPin = 2;

WiFiServer server(80);

bool ledState = false;
bool djMode   = false;

// Static AP IP
IPAddress local_IP(192, 168, 1, 4);
IPAddress gateway(192, 168, 1, 4);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  Serial.print("Portal IP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
}

void loop() {

  // DJ mode blinking
  if (djMode) {
    digitalWrite(ledPin, HIGH);
    delay(80);
    digitalWrite(ledPin, LOW);
    delay(80);
  }

  WiFiClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("/toggle") != -1) {
    djMode = false;
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
  }

  if (request.indexOf("/dj") != -1) {
    djMode = true;
  }

  if (request.indexOf("/stop") != -1) {
    djMode = false;
    ledState = false;
    digitalWrite(ledPin, LOW);
  }

  // Web page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<title>ESP32 LED Control</title>");
  client.println("<style>");
  client.println("body{margin:0;height:100vh;display:flex;align-items:center;justify-content:center;background:#111;color:white;font-family:Arial;}");
  client.println(".card{width:300px;background:#1c1c1c;padding:25px;border-radius:15px;text-align:center;}");
  client.println(".switch{position:relative;display:inline-block;width:60px;height:34px;}");
  client.println(".switch input{display:none;}");
  client.println(".slider{position:absolute;top:0;left:0;right:0;bottom:0;background:#555;border-radius:34px;}");
  client.println(".slider:before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background:white;border-radius:50%;}");
  client.println("input:checked + .slider{background:#00aa66;}");
  client.println("input:checked + .slider:before{transform:translateX(26px);}");
  client.println("button{width:100%;padding:12px;margin-top:15px;font-size:16px;border:none;border-radius:20px;}");
  client.println(".dj{background:#0088cc;color:white;}");
  client.println(".stop{background:#cc0000;color:white;}");
  client.println("</style>");
  client.println("</head>");

  client.println("<body>");
  client.println("<div class='card'>");
  client.println("<h2>ESP32 LED CONTROL</h2>");

  client.print("<label class='switch'>");
  client.print("<input type='checkbox' onclick=\"location.href='/toggle'\" ");
  if (ledState) client.print("checked");
  client.println(">");
  client.println("<span class='slider'></span>");
  client.println("</label>");
  client.println("<p>LED Power</p>");

  client.println("<button class='dj' onclick=\"location.href='/dj'\">DJ MODE</button>");
  client.println("<button class='stop' onclick=\"location.href='/stop'\">STOP</button>");

  client.println("</div>");
  client.println("</body>");
  client.println("</html>");

  client.stop();
}
