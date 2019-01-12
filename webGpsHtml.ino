/**
   第12课 web进阶--GPS解析，定位到百度地图
   上传代码，打开串口监视器，将ip地址加入浏览器或者app inventor 浏览框
   将gps 放到室外；
*/
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
TinyGPSPlus gps;
static const int RXPin = 14, TXPin = 12;//d5,d6
SoftwareSerial swSer(RXPin, TXPin, false, 256);//rx,tx,
static const uint32_t GPSBaud = 9600;
//const char* ssid     = "sjt";
//const char* password = "15176866904QWER";
const char* ssid     = "LLL";
const char* password = "15176866904SJT";
// Set web server port number to 80
WiFiServer server(80);
String header;// Variable to store the HTTP request
double LAT = 0.00;
double LNG = 0.00;
const int output4 = D4;

void setup() {
  swSer.begin(GPSBaud);
  Serial.begin(115200);
  while (!Serial);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("wait...");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  delay(500);
//获取经纬度
            while (swSer.available() > 0) {
              gps.encode(swSer.read());
              if (gps.location.isUpdated()) {
                Serial.print("Latitude= ");
                Serial.print(gps.location.lat(), 6);
                LAT = gps.location.lat();
                Serial.print(" Longitude= ");
                Serial.println(gps.location.lng(), 6);
                LNG = gps.location.lng();
              }
            }
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();




            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv=\"Content-Type\" name=\"viewport\" content=\"charset=utf-8, width=device-width, initial-scale=1\">");

            client.println("<meta http-equiv=\"refresh\" content=30>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html,body { width: 100%;height: 100%;margin:0;}");
            client.println("#allmap {height:100%;width:100%;}</style>");
            client.println("<script type=\"text/javascript\" src=\"http://api.map.baidu.com/api?v=2.0&ak=U1rimaZQl2HKMB89QU0sh9C6C2E2W33N\">");
            client.println("</script>");
            client.println("</head>");

            // Web Page Heading
            client.println("<body><div id=\"allmap\"></div>");
            client.println("</body></html>");

            //js 代码
            client.println("<script type=\"text/javascript\">");
            client.println("var map = new BMap.Map(\"allmap\");");
            client.println("map.centerAndZoom(new BMap.Point(116.4136103013,39.9110666857),11);");
            client.println("map.enableScrollWheelZoom(true);");
            client.println("map.clearOverlays();");
            if(LAT==0){
               client.println("alert(\"gps no signal\");");
            }
            client.println("var new_point = new BMap.Point(" + (String)LNG + "," + (String)LAT + ");");
            client.println("var marker = new BMap.Marker(new_point);");
            client.println("map.addOverlay(marker);");
            client.println("map.panTo(new_point);");
            client.println("</script>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
