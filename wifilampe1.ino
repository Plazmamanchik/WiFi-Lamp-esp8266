#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <EEPROM.h>




String ssid = "ESPLamp";
String password = "12345678";

bool debug=false;
#define redLED2 D2
#define greenLED2 D1
#define blueLED2 D3

#define redLED D2
#define greenLED D1
#define blueLED D3

#define systemLED D4

char storedSSID[32] = "";
char storedPassword[32] = "";


short EEPROM_SSID_ADDRESS = 20;
short EEPROM_PASSWORD_ADDRESS = 52;

ESP8266WebServer webServer(80);

String currentColor = "#FFFFFF";
String SaveColor = "";

ushort currentBrightness = 255;

char state = 1;
ushort brightnessState = 255;
char mode = 0;
unsigned long previousMillis = 0;
// Оголошення змінних r, g, b
ushort r, g, b;
ushort flashStep = 0;
ushort strobeStep = 0;
ushort fadeStep = 0;
ushort smoothStep = 0;
ushort fadeAmount = 3;
ushort brightness = 0;

ushort flashTick = 1000;
ushort strobeTick = 50;
ushort fadeTick = 57;
ushort smoothTick = 1000;


ushort redValue = 255;
ushort greenValue = 0;
ushort blueValue = 0;

String data;

String saveSSID;
String savePassword;

void handleRoot() {


 String webpage = "<!DOCTYPE html>"
 "<html>"
 "<head>"
 "    <meta charset=\"utf-8\">"
 "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
 "    <title>Remotecontrol</title>"
 "    <style>"
 "        /* Змінено для контролю переповнення */"
 "        .remotecontrol {"
 "            background: white; "
 "            border: 2px solid black;"
 "            width: 290px; "
 "            height: 500px; "
 "            margin: 20px auto; /* Додано відступ знизу */"
 "            border-radius: 10px;"
 "            display: flex;"
 "            flex-direction: column; /* Змінено, щоб елементи були розташовані вертикально */"
 "            justify-content: space-between; /* Змінено, щоб елементи були розташовані вертикально */"
 "            padding: 10px; /* Змінено, щоб було більше простору між контролерами */"
 "            margin-top: 60px;"
 "        }"
 "        .headcon {"
 "            display: flex;"
 "            justify-content: space-between; /* Змінено, щоб контролери були розташовані горизонтально */"
 "        }"
 "        .bodycon {"
 "            display: flex;"
 "            justify-content: center; /* Змінено, щоб контролери були розташовані по центру */"
 "            height: 400px;"
 "            width: 260px;"
 "            flex-direction: row; /* Змінено, щоб елементи були розташовані горизонтально */"
 "        }"
 "        .brightness {"
 "            background: gray;"
 "            border: none; "
 "            width: 120px; "
 "            height: 40px; "
 "            border-radius: 7px;"
 "            margin-top: 40px;"
 "            display: flex;"
 "            justify-content: space-between;"
 "            align-items: center;"
 "            padding: 5px;"
 "        }"
 "        .switch {"
 "            background: gray;"
 "            border: none; "
 "            width: 120px; "
 "            height: 40px; "
 "            border-radius: 7px;"
 "            margin-top: 40px;"
 "            display: flex;"
 "            justify-content: space-between;"
 "            align-items: center;"
 "            padding: 5px;"
 "        }"
 "        .button {"
 "          background: white;"
 "          border: none;"
 "          width: 40px;"
 "          height: 40px;"
 "          border-radius: 50%;"
 "          display: flex;"
 "          justify-content: center;"
 "          align-items: center;"
 "          cursor: pointer;"
 "        }"
 "        .color {"
 "            background: gray;"
 "            border: none; "
 "            width: 200px; "
 "            height: 390px; "
 "            border-radius: 7px 0 7px 7px; /* Змінено, щоб кути були закруглені */"
 "            margin-bottom: 40px; /* Додано відступ знизу */"
 "            display: grid; /* Змінено, щоб можна було використовувати grid-систему для розміщення кнопок */"
 "            grid-template-columns: repeat(3, 1fr); /* Змінено, щоб було 4 стовпчика з однаковою шириною */"
 "            grid-template-rows: repeat(2, 1fr) repeat(3, 1fr); smooth"
 "            grid-gap: 5px; /* Змінено, щоб було простори між кнопками */"
 "            padding: 5px; /* Змінено, щоб кнопки не прилягали до країв блоку */"
 "        }"
 "        .white {"
 "            background: gray;"
 "            border: none;"
 "            width: 40px;"
 "            height: 40px;"
 "            border-radius: 0 7px 15px 0;"
 "            display: flex;"
 "            justify-content: space-between;"
 "            align-items: center;"
 "            padding: 5px;"
 "            margin-bottom: 10px;"
 "        }"
 "        .mode {"
 "            background-color: white;"
 "            border-radius: 5px 0 0 0;"
 "            height: 348px;"
 "            width: 60px;"
 "            position: absolute;"
 "            top: 268px;"
 "            margin-left: 216px;"
 "            padding: 5px;"
 "        }"
 "        .mode .button {"
 "            background-color: gray;"
 "            width: 40px;"
 "            height: 40px;"
 "            border-radius: 50%;"
 "            margin-bottom: 5px;"
 "            word-wrap: break-word;"
 "            overflow: hidden;"
 "            text-overflow: ellipsis;"
 "            font-size: 9px;"
 "        }"
 "        .led {"
 "            width: 40px;"
 "            height: 40px;"
 "            background-color: white;"
 "            margin: 0 auto;"
 "            border-radius: 7px;"
 "            border: 0px solid black"
 "        }"
 "    </style>"
 "    <script>"
 "          function color(colorVaule) {"
 "            currentColor = colorVaule;"
 "            var xhttp = new XMLHttpRequest();"
 "            xhttp.open('POST', '/', true);"
 "            xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
 "            xhttp.send('color=' + currentColor);"
 "          }"
 "          function switchLED(state) {"
 "            var xhttp = new XMLHttpRequest();"
 "            xhttp.open('POST', '/', true);"
 "            xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
 "            xhttp.send('command=' + state);"
 "          }"
 "          function tickCommand(command) {"
 "            var xhttp = new XMLHttpRequest();"
 "            xhttp.open('POST', '/', true);"
 "            xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
 "            xhttp.send('tickCommand=' + command);"
 "          }"
 "          function modeCommand(mode) {"
 "            var xhttp = new XMLHttpRequest();"
 "            xhttp.open('POST', '/', true);"
 "            xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
 "            xhttp.send('modeCommand=' + mode);"
 "          }"
 "           function updateSensorValue() {"
 "              var xhttp = new XMLHttpRequest();"
 "              xhttp.onreadystatechange = function() {"
 "                  if (this.readyState == 4 && this.status == 200) {"
 "                      var analogValues = this.responseText.split(',');"
 "                      var dr = parseInt(analogValues[0]);"
 "                      var dg = parseInt(analogValues[1]);"
 "                      var db = parseInt(analogValues[2]);"
 "                      var ledColor = 'rgb(' + dr + ',' + dg + ',' + db + ')';"
 "                      document.getElementById('led').style.backgroundColor = ledColor;"
 "                  }"
 "              };"
 "              xhttp.open('GET', '/sendData', true);"
 "              xhttp.send();"
 "          }"
 "          setInterval(updateSensorValue, 70);"
 "    </script>"
 "</head>"
 "<body style=\"background-color:grey\">"
 "     <div class= 'more'>"
 "     </div>"
 "    <div class=\"led\" id=\"led\"></div>"
 "    <div class=\"remotecontrol\">"
 "        <div class=\"headcon\">"
 "            <div class=\"brightness\">"
 "                <div class=\"button\" style=\"font-size: 24px; font-weight: bold;\" onclick=\"tickCommand('add')\">⇧</div>"
 "                <div class=\"button\" style=\"font-size: 24px; font-weight: bold;\" onclick=\"tickCommand('remove')\">⇩</div>"
 "            </div>"
 "            <div class=\"switch\">"
 "                <div class=\"button\" style=\"background-color: black; color: white;\" onclick=\"switchLED('off')\">Off</div>"
 "                <div class=\"button\" style=\"background-color: red; color: white;\" onclick=\"switchLED('on')\">On</div>"
 "            </div>"
 "        </div>"
 "        <div class=\"bodycon\">"
 "            <div class=\"color\">"
 "                <div class=\"button\" style=\"background-color: #FF0000;\" onclick=\"color('#FF0000')\">R</div>"
 "                <div class=\"button\" style=\"background-color: #008000;\" onclick=\"color('#008000')\">G</div>"
 "                <div class=\"button\" style=\"background-color: #00008B;\" onclick=\"color('#00008B')\">B</div>"
 "                <div class=\"button\" style=\"background-color: #FF8C00;\" onclick=\"color('#FF8C00')\"></div>"
 "                <div class=\"button\" style=\"background-color: #00FF00;\" onclick=\"color('#00FF00')\"></div>"
 "                <div class=\"button\" style=\"background-color: #0000FF;\" onclick=\"color('#0000FF')\"></div>"
 "                <div class=\"button\" style=\"background-color: #FFA500;\" onclick=\"color('#FFA500')\"></div>"
 "                <div class=\"button\" style=\"background-color: #20B2AA;\" onclick=\"color('#20B2AA')\"></div>"
 "                <div class=\"button\" style=\"background-color: #800080;\" onclick=\"color('#800080')\"></div>"
 "                <div class=\"button\" style=\"background-color: #FFD700;\" onclick=\"color('#FFD700')\"></div>"
 "                <div class=\"button\" style=\"background-color: #008B8B;\" onclick=\"color('#008B8B')\"></div>"
 "                <div class=\"button\" style=\"background-color: #FF00FF;\" onclick=\"color('#FF00FF')\"></div>"
 "                <div class=\"button\" style=\"background-color: #FFFF00;\" onclick=\"color('#FFFF00')\"></div>"
 "                <div class=\"button\" style=\"background-color: #008080;\" onclick=\"color('#008080')\"></div>"
 "                <div class=\"button\" style=\"background-color: #EE82EE;\" onclick=\"color('#EE82EE')\"></div>"
 "            </div>"
 "            <div class=\"white\">"
 "                <div class=\"button\" style=\"background-color: #FFFFFF;\" onclick=\"color('#FFFFFF')\">W</div>"
 "            </div>"
 "            <div class=\"mode\">"
 "                <div class=\"button\" style=\"background-color: gray; margin-top: 29px; \" onclick=\"modeCommand('flash')\">FLASH</div>"
 "                <div class=\"button\" style=\"background-color: gray; margin-top: 39px\" onclick=\"modeCommand('strobe')\">STROBE</div>"
 "                <div class=\"button\" style=\"background-color: gray; margin-top: 38px\" onclick=\"modeCommand('fade')\">FADE</div>"
 "                <div class=\"button\" style=\"background-color: gray; margin-top: 41px;\" onclick=\"modeCommand('smooth')\">SMOOTH</div>"
 "            </div>"
 "        </div>"
 "    </div>"
 "   <p style=\"text-align: right; color: #c9c9c9; cursor: pointer;\" onclick=\"window.location.href = '/more'\"><b>More</b></p>"
 
 "</body>"
 "</html>";



  String color = webServer.arg("color");
  String command = webServer.arg("command");
  String tickCommand = webServer.arg("tickCommand");
  String modeCommand = webServer.arg("modeCommand");
  

  int r, g, b;
  int r2, g2, b2;
  if (color != "" && state == 1) {
    currentColor = color;
    
    
    mode = 0;
    r = (int)strtol(color.substring(1, 3).c_str(), NULL, 16);
    g = (int)strtol(color.substring(3, 5).c_str(), NULL, 16);
    b = (int)strtol(color.substring(5, 7).c_str(), NULL, 16);

    setColor(r,g,b,currentBrightness);
  }

  if (command == "on" && state == 0) {
    //16236607
    state = 1;
    Serial.println(SaveColor);
    currentColor = SaveColor;
    Serial.println(currentColor);
    r = (int)strtol(currentColor.substring(1, 3).c_str(), NULL, 16);
    g = (int)strtol(currentColor.substring(3, 5).c_str(), NULL, 16);
    b = (int)strtol(currentColor.substring(5, 7).c_str(), NULL, 16);

    setColor(r,g,b,currentBrightness);

  } else if (command == "off" && state == 1) {
    state = 0;
    Serial.println("off");
    SaveColor = currentColor;
    currentColor = "#000000";
    //16203967
    
    r = (int)strtol(currentColor.substring(1, 3).c_str(), NULL, 16);
    g = (int)strtol(currentColor.substring(3, 5).c_str(), NULL, 16);
    b = (int)strtol(currentColor.substring(5, 7).c_str(), NULL, 16);

    setColor(r,g,b,currentBrightness);

  }


  if (tickCommand == "remove" && state == 1 && flashTick < 2000 && mode == 1) {
    flashTick = flashTick + 20;
    Serial.println(flashTick);
  } else if (tickCommand == "add" && state == 1 && flashTick > 20 && flashTick != 20 && mode == 1) {
    flashTick = flashTick - 20;
    Serial.println(flashTick);
  }


  if (tickCommand == "remove" && state == 1 && strobeTick < 100 && mode == 2) {
    strobeTick = strobeTick + 2;
    Serial.println(strobeTick);
  } else if (tickCommand == "add" && state == 1 && strobeTick > 1 && strobeTick != 7 && mode == 2) {
    strobeTick = strobeTick - 2;
    Serial.println(strobeTick);
  }


  if (tickCommand == "remove" && state == 1 && fadeTick < 107 && mode == 3) {
    fadeTick = fadeTick + 5;
    Serial.println(fadeTick);
  } else if (tickCommand == "add" && state == 1 && fadeTick > 7 && fadeTick != 7 && mode == 3) {
    fadeTick = fadeTick - 5;
    Serial.println(fadeTick);
  }

  if (tickCommand == "remove" && state == 1 && smoothTick < 2000 && mode == 4) {
    smoothTick = smoothTick + 20;
    Serial.println(smoothTick);
  } else if (tickCommand == "add" && state == 1 && smoothTick > 20 && smoothTick != 20 && mode == 4) {
    smoothTick = smoothTick - 20;
    Serial.println(smoothTick);
  }



  if (modeCommand == "flash") {
    mode = 1;
  } else if (modeCommand == "strobe") {
    mode = 2;
  } else if (modeCommand == "fade") {
    mode = 3;
  } else if (modeCommand == "smooth") {
    mode = 4;
  }

  webServer.send(200, "text/html", webpage);
}


void handleMore() {
  String html = "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "  <meta charset='utf-8'>"
  "  <meta name='viewport' content='width=device-width, initial-scale=1'>"
  "  <title>More</title>"
  "  <style>"
  "    body {"
  "      font-family: 'Arial', sans-serif;"
  "      background-color: #f2f2f2;"
  "      text-align: center;"
  "      background-color:grey;"
  "    }"
  "    .container {"
  "      max-width: 400px;"
  "      margin: 0 auto;"
  "      padding: 20px;"
  "      background-color: #9c9a9a;"
  "      border-radius: 5px;"
  "      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"
  "    }"
  "    h1 {"
  "      color: #333;"
  "    }"
  "    label {"
  "      display: block;"
  "      margin-bottom: 10px;"
  "      font-weight: bold;"
  "    }"
  "    .input-container {"
  "      display: flex;"
  "      flex-direction: column;"
  "      align-items: center;"
  "      margin-bottom: 20px;" // Відступ між контейнером інпутів і іншими елементами
  "    }"
  "    input[type='text'], input[type='password'] {"
  "      width: 100%;"
  "      padding: 10px;"
  "      margin-bottom: 10px;" // Відступ між інпутами
  "      border: 1px solid #ccc;"
  "      border-radius: 3px;"
  "    }"
  "    .connect-button {"
  "      background-color: #007BFF;"
  "      color: #fff;"
  "      border: none;"
  "      padding: 10px 20px;"
  "      border-radius: 3px;"
  "      cursor: pointer;"
  "    }"
  "    .connect-button:hover {"
  "      background-color: #0056b3;"
  "    }"
  "    .back-button {"
  "      background-color: #ccc;"
  "      color: #333;"
  "      border: none;"
  "      padding: 10px 20px;"
  "      border-radius: 3px;"
  "      cursor: pointer;"
  "      margin-top: 10px;" // Відступ між кнопкою і іншими елементами
  "    }"
  "    .back-button:hover {"
  "      background-color: #999;"
  "    }"
  "  </style>"
  "</head>"
  "<body>";

  if (WiFi.status() != WL_CONNECTED) {
    html += "<div class='container' id='setting'>";
    html += "  <h1>Wi-Fi налаштування</h1>";
    html += "  <div class='input-container'>"; // Один контейнер для обох інпутів
    html += "    <label for='ssid'>Назва Wi-Fi:</label>";
    html += "    <input type='text' id='ssid' name='ssid'>";
    html += "    <label for='password'>Пароль Wi-Fi:</label>";
    html += "    <input type='password' id='password' name='password'>";
    html += "  </div>";
    html += "  <div>";
    html += "    <button class='connect-button' onclick='connectWiFi();'>";
    html += "      <b>З'єднатись</b>";
    html += "    </button>";
    html += "  </div>";
    html += "</div>";
  } else {
    html += "<h1 class='white-text' style='text-align:center;'>" + WiFi.localIP().toString() + "</h1>";
  }



  html += "<script>"
  "  function connectWiFi() {"
  "    var ssid = document.getElementById('ssid').value;"
  "    var password = document.getElementById('password').value;"
  "    var xhttp = new XMLHttpRequest();"
  "    xhttp.open('POST', '/connect', true);"
  "    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
  "    xhttp.send('ssid=' + ssid + '&password=' + password);"
  "    alert('Перезавантажте сторінку');"
  "  }"
  "  function back() {"
  "    window.location.href = '/';"
  "  }"
  "  function sendBrightness() {"
  "    var brightnessValue = document.getElementById('brightness').value;"
  "    var xhttp = new XMLHttpRequest();"
  "    xhttp.open('POST', '/brightness', true);"
  "    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
  "    xhttp.send('brightness=' + brightnessValue);"
  "  }"
  "  function morecolor(){"
  "    var morecolor = document.getElementById('morecolor').value;"
  "    currentColor = morecolor;"
  "    var xhttp = new XMLHttpRequest();"
  "    xhttp.open('POST', '/', true);"
  "    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
  "    xhttp.send('color=' + encodeURIComponent(currentColor));"
  "    document.getElementById('morecolor').style.backgroundColor = morecolor;"
  "  }"
  "    function updateBringhtness() {"
  "        var xhttp = new XMLHttpRequest();"
  "        xhttp.onreadystatechange = function() {"
  "            if (this.readyState == 4 && this.status == 200) {"
 // "                console.log(this.responseText);"
  "                document.getElementById('brightness').value = this.responseText;"
  "            }"
  "        };"
  "        xhttp.open('GET', '/sendBringhtness', true);"
  "        xhttp.send();"
  "    }"
  "    setInterval(updateBringhtness, 50);"
  "</script>"
  "<div class='brightness'>"
  "  <h2 class='brightness-label'>Яскравість:</h2>"
  "  <input oninput=\"sendBrightness()\" style='width:300px;' class='brightness-input' type='range' id='brightness' name='brightness' min='10' max='250' value='"+ String(currentBrightness) +"' step='1'>"
  "  <p style='color:#b5b5b5'>Ви не зможете керувати такими режимамі як STROBE, FADE</p>"
  "</div>"
  "<div class='more-color'>"
  "  <h2 class='brightness-label'>Виберіть колір:</h2>"
  "  <input type='color' style=' border-radius:5px;' name='morecolor' id='morecolor' class='morecolor' oninput='morecolor()'>"
  "</div>"
  "<button class='back-button' onclick='back();'>Назад</button>"
  "</body>"
  "</html>";

  webServer.send(200, "text/html", html);
}


void setColor(int r, int g, int b, int setBringhtness){
  analogWrite(redLED, map(r, 0, 255, 255, 255 - setBringhtness));
  analogWrite(greenLED, map(g, 0, 255, 255, 255 - setBringhtness));
  analogWrite(blueLED, map(b, 0, 255, 255, 255 - setBringhtness));
  if(!debug){
    analogWrite(redLED2, 255 -map(r, 0, 255, 255, 255 - setBringhtness));
    analogWrite(greenLED2, 255 -map(g, 0, 255, 255, 255 - setBringhtness));
    analogWrite(blueLED2, 255 -map(b, 0, 255, 255, 255 - setBringhtness));
  }
  data = String(map(r, 0, 255, 0, 255)) + "," + String(map(g, 0, 255, 0, 255)) + "," + String(map(b, 0, 255, 0, 255));
}

void sendData() {
  webServer.send(200, "text/plain", data);
}
void sendBringhtness(){
  webServer.send(200, "text/plain", String(currentBrightness));
}




void handleBrightness() {
  String brightnessStr = webServer.arg("brightness"); // Отримуємо рядок для яскравості
  int brightness = 0; // Оголошуємо змінну для збереження яскравості
  
  if (brightnessStr != "" && state == 1 && mode != 2) {
    brightness = brightnessStr.toInt(); // Перетворюємо рядок у ціле число
    currentBrightness = brightness; // Оновлюємо поточну яскравість
    SaveColor = currentColor;
    currentColor = SaveColor;

    r = (int)strtol(currentColor.substring(1, 3).c_str(), NULL, 16);
    g = (int)strtol(currentColor.substring(3, 5).c_str(), NULL, 16);
    b = (int)strtol(currentColor.substring(5, 7).c_str(), NULL, 16);

    setColor(r,g,b,currentBrightness);
  }
}




void handleConnect() {
  String ssidInput = webServer.arg("ssid");
  String passwordInput = webServer.arg("password");

  Serial.println("Дані Wi-Fi:");
  Serial.println("SSID: " + ssidInput);
  Serial.println("Пароль: " + passwordInput);
  WiFi.begin(ssidInput.c_str(), passwordInput.c_str());

  // Зберігаємо ім'я та пароль в EEPROM при успішному підключенні
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    EEPROM.begin(512); // Ініціалізуємо EEPROM

    // Записуємо SSID в EEPROM
    for (int i = 0; i < ssidInput.length(); ++i) {
      EEPROM.write(EEPROM_SSID_ADDRESS + i, ssidInput[i]);
    }
    EEPROM.write(EEPROM_SSID_ADDRESS + ssidInput.length(), '\0'); // Додаємо нуль-термінатор для завершення рядка

    // Записуємо пароль в EEPROM
    for (int i = 0; i < passwordInput.length(); ++i) {
      EEPROM.write(EEPROM_PASSWORD_ADDRESS + i, passwordInput[i]);
    }
    EEPROM.write(EEPROM_PASSWORD_ADDRESS + passwordInput.length(), '\0'); // Додаємо нуль-термінатор для завершення рядка

    EEPROM.commit(); // Записуємо дані в EEPROM
    EEPROM.end(); // Закриваємо EEPROM
    Serial.println("Ім'я та пароль збережено в EEPROM");
    Serial.println("Підключено до Wi-Fi");
    Serial.print("IP адреса: ");
    Serial.println(WiFi.localIP());
    
  } else {
    Serial.println("Помилка при підключенні до Wi-Fi");
  }
}


void readStoredCredentials() {
  EEPROM.begin(512); // Ініціалізуємо EEPROM

  // Читаємо SSID та пароль з EEPROM
  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(EEPROM_SSID_ADDRESS + i);
    if (c == '\0') {
      break; // Зупиняємо читання при знаходженні нуль-термінатора
    }
    storedSSID[i] = c;
  }

  for (int i = 0; i < 32; ++i) {
    char c = EEPROM.read(EEPROM_PASSWORD_ADDRESS + i);
    if (c == '\0') {
      break; // Зупиняємо читання при знаходженні нуль-термінатора
    }
    storedPassword[i] = c;
  }

  EEPROM.end(); // Закриваємо EEPROM
  
  if (storedSSID[0] != '\0') {
    Serial.print("Зчитані дані з EEPROM:\nStored SSID: ");
    Serial.println(storedSSID);
    Serial.print("Stored Password: ");
    Serial.println(storedPassword);
    
    WiFi.begin(storedSSID, storedPassword);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.println("Підключено до Wi-Fi");
      Serial.print("IP адреса: ");
      Serial.println(WiFi.localIP());
      WiFi.softAPdisconnect(true);
      // Встановлення точки доступу
      String apSSID = ssid + " " + WiFi.localIP().toString();
      WiFi.softAP(apSSID, password);
      storedSSID = storedSSID;
      savePassword = storedPassword;
    } else {
      Serial.println("Помилка при підключенні до Wi-Fi");
    }
  } else {
    Serial.println("Немає збережених даних в EEPROM");
  }
}


void setup() {
  if (!debug){
    pinMode(redLED2, OUTPUT);
    pinMode(greenLED2, OUTPUT);
    pinMode(blueLED2, OUTPUT);
  }
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(systemLED,OUTPUT);
 
  digitalWrite(systemLED,HIGH);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(blueLED, LOW);

  Serial.begin(9600);

  webServer.on("/", handleRoot);
  webServer.on("/more", HTTP_GET, handleMore);
  webServer.on("/connect", handleConnect);
  webServer.on("/brightness",handleBrightness);
  webServer.on("/sendBringhtness", HTTP_GET, sendBringhtness);
  webServer.on("/sendData", HTTP_GET, sendData);
  webServer.begin();
 
  mode = 3;
  fadeTick = 7;
  
  readStoredCredentials();
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  Serial.println("...");
}


void loop() {
  SerialCommand();
  webServer.handleClient();
  modes_led();
  wificonnect();

  
}





void wificonnect(){
  if (WiFi.status() != WL_CONNECTED){
    digitalWrite(systemLED, LOW);
  }else{
    digitalWrite(systemLED, HIGH);
  }
}


void SerialCommand() {
  if (Serial.available()) {
    char data[30];
    int amount = Serial.readBytesUntil(';', data, 30);
    data[amount] = '\0';

    char* separator = strchr(data, ' ');
    if (separator != NULL) {
      // Витягання командної частини
      int commandLength = separator - data;
      char command[commandLength + 1];
      strncpy(command, data, commandLength);
      command[commandLength] = '\0';

      // Витягання аргументу
      char* argument = separator + 1;

      // Вивід результату
      Serial.print("command: ");
      Serial.println(command);
      Serial.print("argument: ");
      Serial.println(argument);

      // Обробка команд
      if (strcmp(command, "info")) {
        Serial.println("Status WiFi:");
        if(WiFi.status() == WL_CONNECTED){
          Serial.println("Підключено до: " + )
        }
      } else {
        Serial.println("Невідома команда");
      }
    } else {
    }
  }
}


void modes_led() {
  unsigned long currentMillis = millis();
  int Brightness = 0;
  int currentBrightness = 255;

  if (mode == 1 && state == 1) {
    if (currentMillis - previousMillis >= flashTick) {
      previousMillis = currentMillis;

      if (smoothStep == 0) {
        currentColor = "#FF0000";
      } else if (smoothStep == 1) {
        currentColor = "#FFFF00";
      } else if (smoothStep == 2) {
        currentColor = "#008000";
      } else if (smoothStep == 3) {
        currentColor = "#00FFFF";
      } else if (smoothStep == 4) {
        currentColor = "#00008B";
      } else if (smoothStep == 5) {
        currentColor = "#FF00FF";
      }

      r = (int)strtol(currentColor.substring(1, 3).c_str(), NULL, 16);
      g = (int)strtol(currentColor.substring(3, 5).c_str(), NULL, 16);
      b = (int)strtol(currentColor.substring(5, 7).c_str(), NULL, 16);

      setColor(r,g,b,currentBrightness);
      smoothStep = (smoothStep + 1) % 6;
    }
  }

  // Роздільна затримка для режиму "strobe"
  if (mode == 2 && state == 1) {
    if (currentMillis - previousMillis >= strobeTick) {
      previousMillis = currentMillis;

      r = (int)strtol(currentColor.substring(1, 3).c_str(), NULL, 16);
      g = (int)strtol(currentColor.substring(3, 5).c_str(), NULL, 16);
      b = (int)strtol(currentColor.substring(5, 7).c_str(), NULL, 16);

      setColor(r,g,b,brightness);

      brightness += fadeAmount;  // Збільшуємо яскравість

      if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount;  // Змінюємо напрямок зміни яскравості
      }
    }
  }

  if (mode == 3 && state == 1) {
    int brightness = 0;
    if (currentMillis - previousMillis >= fadeTick) {
      previousMillis = currentMillis;

      setColor(redValue ,greenValue,blueValue ,currentBrightness);


      if (greenValue < 255 && redValue == 255 && blueValue == 0) {
        greenValue++;
      } else if (greenValue == 255 && redValue > 0 && blueValue == 0) {
        redValue--;
      } else if (greenValue == 255 && redValue == 0 && blueValue < 255) {
        blueValue++;
      } else if (greenValue > 0 && redValue == 0 && blueValue == 255) {
        greenValue--;
      } else if (greenValue == 0 && redValue < 255 && blueValue == 255) {
        redValue++;
      } else if (greenValue == 0 && redValue == 255 && blueValue > 0) {
        blueValue--;
      }
    }
  }














// Роздільна затримка для режиму "smooth"
  if (mode == 4 && state == 1) {
    if (currentMillis - previousMillis >= smoothTick) {
      previousMillis = currentMillis;

      if (smoothStep == 0) {
        currentColor = "#FF0000";  // Red
      } else if (smoothStep == 1) {
        currentColor = "#008000";  // Green
      } else if (smoothStep == 2) {
        currentColor = "#00008B";  // Blue
      }

      r = (int)strtol(currentColor.substring(1, 3).c_str(), NULL, 16);
      g = (int)strtol(currentColor.substring(3, 5).c_str(), NULL, 16);
      b = (int)strtol(currentColor.substring(5, 7).c_str(), NULL, 16);

      setColor(r,g,b,currentBrightness);
      smoothStep = (smoothStep + 1) % 3;  // Збільшуємо крок (0, 1, 2) і циклічно переходимо через всі кольори
    }
  }
}



