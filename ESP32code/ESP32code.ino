#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "TrippleC";
const char *password = "19671970990003";

const char *webhookURL = "https://discord.com/api/webhooks/1374875339796381777/PN5DmLXZNwDXZ5PHlpchZeYB1kCR5_3nbJuy60eNCDLUKmOM9_XRPhNK5W2YnXdfhe3U";

NetworkServer server(80);

#define Red_LED 2
#define Blue_LED 4
#define Green_LED 5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(Red_LED, OUTPUT);
  pinMode(Blue_LED, OUTPUT);
  pinMode(Green_LED, OUTPUT);
  
  delay(10);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFI");
    delay(500);
  }


  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  NetworkClient client = server.accept();

  if(client) {
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<a href=\"/RED\">RED</a><br>");
            client.print("<a href=\"/BLUE\">BLUE</a><br>");
            client.print("<a href=\"/GREEN\">GREEN</a><br>");
            //client.print("<a href=\"/END\">END</a><br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if(currentLine.endsWith("GET /RED")) {
          digitalWrite(Red_LED, !digitalRead(Red_LED));
          if(digitalRead(Red_LED) == HIGH) {
            sendDiscordNotification("Red LED turned on");
          }
          else {
            sendDiscordNotification("Red LED turned off");
          }
        }
        if(currentLine.endsWith("GET /BLUE")) {
          digitalWrite(Blue_LED, !digitalRead(Blue_LED));
          if(digitalRead(Blue_LED) == HIGH) {
            sendDiscordNotification("Blue LED turned on");
          }
          else {
            sendDiscordNotification("Blue LED turned off");
          }
        }
        if(currentLine.endsWith("GET /GREEN")) {
          digitalWrite(Green_LED, !digitalRead(Green_LED));
          if(digitalRead(Green_LED) == HIGH) {
            sendDiscordNotification("Green LED turned on");
          }
          else {
            sendDiscordNotification("Green LED turned off");
          }
        }
        // if(currentLine.endsWith("GET /END")) {
        //   client.stop();
        // }
        // client.stop();
        // Serial.println("Client Disconnected.");
      }
    }
  }
}

void sendDiscordNotification(String message) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhookURL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"content\":\"" + message + "\"}";
    int httpResponseCode = http.POST(payload);

    if(httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    }
    else {
      Serial.println("Error snding message");
    }

    http.end();
  }

  else {
    Serial.println("WiFi not connected");
  }
}
