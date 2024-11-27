9.	#include <ESP8266WiFi.h>
10.	#include <DHT.h>
11.	
12.	// Wi-Fi credentials
13.	const char* ssid = "Jacker V";
14.	const char* password = "25271203";
15.	
16.	// Login credentials
17.	const String username = "admin";
18.	const String userPassword = "123456";
19.	
20.	// Server setup
21.	WiFiServer server(80);
22.	String header;
23.	
24.	// Output states
25.	String output5State = "off";
26.	String output4State = "off";
27.	String output3State = "off";
28.	
29.	// GPIO pins
30.	const int output5 = 5;    // D1
31.	const int output4 = 4;    // D2
32.	const int output3 = 14;   // D5 - LED thứ 3
33.	const int lightSensor = 12; // Cảm biến ánh sáng
34.	const int dhtPin = 13; // Chân D7 của ESP8266
35.	
36.	// DHT sensor setup
37.	DHT dht(dhtPin, DHT11); // Khởi tạo cảm biến DHT11 với chân DATA là D6
38.	
39.	bool isLoggedIn = false; // Trạng thái đăng nhập
40.	
41.	unsigned long currentTime = millis();
42.	unsigned long previousTime = 0;
43.	const long timeoutTime = 2000;
44.	
45.	void setup() {
46.	  Serial.begin(115200);
47.	  pinMode(output5, OUTPUT);
48.	  pinMode(output4, OUTPUT);
49.	  pinMode(output3, OUTPUT);
50.	  digitalWrite(output5, LOW);
51.	  digitalWrite(output4, LOW);
52.	  digitalWrite(output3, LOW);
53.	
54.	  Serial.print("Connecting to ");
55.	  Serial.println(ssid);
56.	  WiFi.begin(ssid, password);
57.	  while (WiFi.status() != WL_CONNECTED) {
58.	    delay(500);
59.	    Serial.print(".");
60.	  }
61.	  Serial.println("\nWiFi connected.");
62.	  Serial.println("IP address: ");
63.	  Serial.println(WiFi.localIP());
64.	  server.begin();
65.	
66.	  // Khởi tạo cảm biến DHT11
67.	  dht.begin();
68.	}
69.	
70.	void loop() {
71.	  WiFiClient client = server.available();
72.	
73.	  if (client) {
74.	    Serial.println("New Client.");
75.	    String currentLine = "";
76.	    currentTime = millis();
77.	    previousTime = currentTime;
78.	
79.	    while (client.connected() && currentTime - previousTime <= timeoutTime) {
80.	      currentTime = millis();
81.	      if (client.available()) {
82.	        char c = client.read();
83.	        Serial.write(c);
84.	        header += c;
85.	
86.	        if (c == '\n') {
87.	          if (currentLine.length() == 0) {
88.	            // HTTP header
89.	            client.println("HTTP/1.1 200 OK");
90.	            client.println("Content-type:text/html");
91.	            client.println("Connection: close");
92.	            client.println();
93.	
94.	            // Login functionality
95.	            if (!isLoggedIn) {
96.	              if (header.indexOf("GET /login?user=") >= 0) {
97.	                String user = header.substring(header.indexOf("user=") + 5, header.indexOf("&pass="));
98.	                String pass = header.substring(header.indexOf("pass=") + 5, header.indexOf(" HTTP"));
99.	                if (user == username && pass == userPassword) {
100.	                  isLoggedIn = true;
101.	                }
102.	              }
103.	              if (!isLoggedIn) {
104.	                client.println("<!DOCTYPE html><html><head>");
105.	                client.println("<style>");
106.	                client.println("body { font-family: Arial; text-align: center; background-color: #E3F2FD; color: #1565C0; }");
107.	                client.println("button { padding: 10px 20px; font-size: 16px; background-color: #42A5F5; color: white; border: none; border-radius: 5px; cursor: pointer; }");
108.	                client.println("button:hover { background-color: #1E88E5; }");
109.	                client.println("</style>");
110.	                client.println("</head><body>");
111.	                client.println("<h1>Welcome to IOT home</h1>");
112.	                client.println("<form action=\"/login\">");
113.	                client.println("Username: <input type=\"text\" name=\"user\"><br><br>");
114.	                client.println("Password: <input type=\"password\" name=\"pass\"><br><br>");
115.	                client.println("<input type=\"submit\" value=\"Login\" style=\"background-color: #1565C0; color: white;\">");
116.	                client.println("</form></body></html>");
117.	                break;
118.	              }
119.	            }
120.	
121.	            // Process controls if logged in
122.	            if (isLoggedIn) {
123.	              if (header.indexOf("GET /logout") >= 0) {
124.	                isLoggedIn = false;
125.	              }
126.	
127.	              if (header.indexOf("GET /5/on") >= 0) {
128.	                output5State = "on";
129.	                digitalWrite(output5, HIGH);
130.	              } else if (header.indexOf("GET /5/off") >= 0) {
131.	                output5State = "off";
132.	                digitalWrite(output5, LOW);
133.	              }
134.	
135.	              if (header.indexOf("GET /4/on") >= 0) {
136.	                output4State = "on";
137.	                digitalWrite(output4, HIGH);
138.	              } else if (header.indexOf("GET /4/off") >= 0) {
139.	                output4State = "off";
140.	                digitalWrite(output4, LOW);
141.	              }
142.	
143.	              // Đọc tín hiệu từ cảm biến ánh sáng
144.	              int lightSensorState = digitalRead(lightSensor);
145.	              if (lightSensorState == LOW) { // Nếu tín hiệu từ cảm biến là LOW
146.	                output3State = "on";
147.	                digitalWrite(output3, HIGH);
148.	              } else {
149.	                output3State = "off";
150.	                digitalWrite(output3, LOW);
151.	              }
152.	
153.	              // Đọc nhiệt độ và độ ẩm từ DHT11
154.	              float temperature = dht.readTemperature(); // Đọc nhiệt độ (đơn vị C)
155.	              float humidity = dht.readHumidity(); // Đọc độ ẩm
156.	
157.	              // Kiểm tra nếu có lỗi khi đọc từ cảm biến
158.	              
159.	              client.println("<p>Temperature: " + String(temperature) + " &deg;C</p>");
160.	              client.println("<p>Humidity: " + String(humidity) + " %</p>");
161.	              
162.	              
163.	
164.	              // Cung cấp giao diện điều khiển
165.	              client.println("<!DOCTYPE html><html><head>");
166.	client.println("<style>");
167.	client.println("body { font-family: Arial; text-align: center; background-color: #E3F2FD; color: #1565C0; }");
168.	client.println("h1 { color: #0D47A1; }");
169.	client.println("button { padding: 10px 20px; font-size: 16px; background-color: #42A5F5; color: white; border: none; border-radius: 5px; cursor: pointer; }");
170.	client.println("button:hover { background-color: #1E88E5; }");
171.	client.println("a { text-decoration: none; }");
172.	client.println("</style>");
173.	client.println("<script>");
174.	// JavaScript to display real-time clock
175.	client.println("function updateClock() {");
176.	client.println("  const now = new Date();");
177.	client.println("  const timeString = now.toLocaleTimeString();");
178.	client.println("  const dateString = now.toLocaleDateString();");
179.	client.println("  document.getElementById('clock').innerHTML = `${dateString} ${timeString}`;");
180.	client.println("}");
181.	client.println("setInterval(updateClock, 1000);");
182.	client.println("</script></head><body>");
183.	client.println("<h1>Control Led</h1>");
184.	client.println("<p id='clock' style='font-size: 18px; font-weight: bold; color: #1E88E5;'>Loading...</p>"); // Real-time clock display
185.	client.println("<p>Led 1 - State " + output5State + "</p>");
186.	client.println("<a href=\"/5/on\"><button>ON</button></a>");
187.	client.println("<a href=\"/5/off\"><button>OFF</button></a>");
188.	client.println("<p>Led 2 - State " + output4State + "</p>");
189.	client.println("<a href=\"/4/on\"><button>ON</button></a>");
190.	client.println("<a href=\"/4/off\"><button>OFF</button></a>");
191.	client.println("<p>Led 3 (Auto) - State " + output3State + "</p>");
192.	client.println("<a href=\"/logout\"><button>Logout</button></a>");
193.	client.println("</body></html>");
194.	            }
195.	            break;
196.	          } else {
197.	            currentLine = "";
198.	          }
199.	        } else if (c != '\r') {
200.	          currentLine += c;
201.	        }
202.	      }
203.	    }
204.	    header = "";
205.	    client.stop();
206.	    Serial.println("Client disconnected.");
207.	  }
208.	
209.	  // Cập nhật trạng thái đèn 3 từ cảm biến ánh sáng
210.	  int lightSensorState = digitalRead(lightSensor);
211.	  if (lightSensorState == HIGH) {
212.	    output3State = "on";
213.	    digitalWrite(output3, HIGH);
214.	  } else {
215.	    output3State = "off";
216.	    digitalWrite(output3, LOW);
217.	  }
218.	}
219.	

