#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char *ssid = "Smart Home";
const char *password = "12345678";

ESP8266WebServer server(80);
#define ARDUINO_ADDRESS 0x08

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    @font-face {
      font-family: Dirooz;
      src: url('../fonts/Dirooz.eot');
      src: url('../fonts/Dirooz.eot?#iefix') format('embedded-opentype'),
        url('../fonts/Dirooz.woff') format('woff'),
        url('../fonts/Dirooz.ttf') format('truetype');
      font-weight: normal;
    }

    body {
      font-family: Dirooz;
      margin: 0;
      padding: 0;
    }

    .header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      /* Space between items */
      padding: 10px 20px;
      background-color: #fff;
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
      position: relative;
    }

    .title {
      flex-grow: 1;
      /* Allows title to take up available space */
      text-align: center;
      /* Center the title text */
      font-weight: bold;
      /* Optional: make the title bold */
    }

    .avatar {
      width: 50px;
      height: 50px;
      border-radius: 50%;
      background-color: #ccc;
      position: relative;
    }

    .status {
      position: absolute;
      bottom: 0;
      right: 0;
      width: 15px;
      height: 15px;
      border-radius: 50%;
      background-color: green;
      border: 2px solid #fff;
    }

    .button-container {
      display: flex;
      gap: 10px;
      /* Space between buttons */
    }

    .btn {
      background-color: #ffffff;
      /* Light background */
      border: none;
      /* Remove default border */
      border-radius: 5px;
      /* Rounded corners */
      padding: 8px;
      /* Padding for better click area */
      cursor: pointer;
      /* Pointer on hover */
      transition: background-color 0.3s, transform 0.2s;
      /* Animation effects */
    }

    .btn:hover {
      background-color: #e0e0e0;
      /* Darker on hover */
      transform: scale(1.05);
      /* Slightly enlarge on hover */
    }

    .btn:focus {
      outline: none;
      /* Remove outline on focus */
    }

    .btn svg {
      fill: #333;
      /* Color for SVG icons */
      width: 24px;
      /* Adjust icon size */
      height: 24px;
      /* Adjust icon size */
    }

    .status_box {
      color: #fff;
      padding: 5px;
      background-color: green;
      border: 2px solid #fff;
      border-radius: 7px;
      position: absolute;
      right: 60px;
      top: 50%;
      transform: translateY(-50%);
    }

    .container {
      display: flex;
      flex-wrap: wrap;
      gap: 20px;
      padding: 20px;
      background-color: #f9f9f9;
    }

    .box {
      flex: 1 1 200px;
      height: 150px;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-direction: column;
      border-radius: 8px;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
      position: relative;
      overflow: hidden;
    }

    .box2 {
      flex: 1 1 200px;
      height: auto;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-direction: column;
      border-radius: 8px;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
      position: relative;
      overflow: hidden;
    }

    .box22 {
      flex: 2;
    }

    .box23 {
      flex: 1;
    }

    .box1 {
      height: auto;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-direction: column;
      border-radius: 8px;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
      position: relative;
      overflow: hidden;
      padding: 20px;
      /* Added padding for better spacing */
    }


    .highlight {
      width: 100%;
      height: 30px;
      background-color: #2e7d32;
      position: absolute;
      bottom: 0;
      left: 0;
      border-bottom-left-radius: 8px;
      border-bottom-right-radius: 8px;
    }

    .weather {
      background-color: #1076d0;
      background-image: url('../src/weather.svg');
      background-size: cover;
      background-position: center;
      background-size: 150px 150px;
      background-position: -45px -25px;
      background-repeat: no-repeat;
      position: relative;
      border: 1px solid #ccc;
      border-radius: 8px;
      overflow: hidden;
    }

    .hWeather {
      background-color: #0e599c;
    }

    .titleInfo {
      color: white;
      font-size: 25px;
      position: absolute;
      top: -20px;
      right: 20px;
    }

    .data {
      color: white;
      font-size: 35px;

    }

    .temperature {
      background-color: #ebbb0d;
      background-image: url('../src/temperature.svg');
      background-size: cover;
      background-position: center;
      background-size: 150px 150px;
      background-position: -45px -25px;
      background-repeat: no-repeat;
      position: relative;
      border: 1px solid #ccc;
      border-radius: 8px;
      overflow: hidden;
    }

    .hTemperature {
      background-color: #a48208;
    }

    .gas {
      background-color: #eb780d;
      background-image: url('../src/gas.svg');
      background-size: cover;
      background-position: center;
      background-size: 150px 150px;
      background-position: -45px -25px;
      background-repeat: no-repeat;
      position: relative;
      border: 1px solid #ccc;
      border-radius: 8px;
      overflow: hidden;
    }

    .hGas {
      background-color: #a35913;
    }

    .light {
      background-color: #ebe00d;
      background-image: url('../src/light.svg');
      background-size: cover;
      background-position: center;
      background-size: 150px 150px;
      background-position: -45px -25px;
      background-repeat: no-repeat;
      position: relative;
      border: 1px solid #ccc;
      border-radius: 8px;
      overflow: hidden;
    }

    .hLight {
      background-color: #b1a913;
    }

    .containerTitle {
      width: 400px;
      height: 20px;
      position: relative;
      display: flex;
      justify-content: center;
      align-items: center;
      margin-bottom: 20px;
      font-size: 20px;
      color: #434343;
    }

    .bcolor {
      background: linear-gradient(45deg, #5c7ae6, #6439FF);
      min-height: 300px;
    }

    .security {
      color: #ffffff;
    }

    .dis {
      color: #ffffff;
      margin-right: 10px;
    }

    .graphTitlr {
      margin-top: 20px;
    }

    .hsmart {
      padding-right: 50px;
      background-color: #23068e;
      direction: rtl;
      text-align: right;
      font-size: 10px;
    }

    .button {
      position: relative;
      display: inline-block;
      border: none;
      background: none;
      cursor: pointer;
      padding: 20px;
      outline: none;
    }

    .button svg {
      width: 100px;
      height: 100px;
    }

    .circle {
      fill: none;
      stroke: transparent;
      /* دایره در حالت عادی نامرئی است */
      stroke-width: 4;
      /* ضخامت دایره */
      transition: stroke 0.3s ease;
      /* انیمیشن */
    }

    .button:hover .circle {
      stroke: #007BFF;
      /* رنگ دایره پررنگ‌تر */
    }

    .notification-list {
      width: 100%;
      /* Full width */
      height: 300px;
      /* Limit height for scrolling */
      overflow-y: auto;
      /* Enable vertical scrolling */
      margin-bottom: 20px;
      /* Space below the list */
      border-radius: 5px;
      /* Rounded corners */
      background-color: #f9f9f9;
      /* Light background for notifications */
    }

    .notification-item {
      padding: 10px;
      /* Space inside each notification */
      border-bottom: 1px solid #eee;
      /* Divider between notifications */
      color: #333;
      margin: 10px;
      direction: rtl;
      /* Text color */

    }

    .notification-item:last-child {
      border-bottom: none;
      /* Remove border for the last item */
    }

    .notification-item:hover {
      background-color: #f0f0f0;
      /* Highlight on hover */
    }

    canvas {
      margin-top: 20px;
      /* Space above the canvas */
    }

    .delete-btn {
      background-color: red;
      border: none;
      cursor: pointer;
      display: flex;
      align-items: center;
      gap: 5px;
      /* فاصله بین متن و SVG */
    }

    .delete-btn img {
      width: 16px;
      height: 16px;
    }

    .toggle {
      position: relative;
      width: 60px;
      height: 30px;
      background-color: #ccc;
      border-radius: 15px;
      cursor: pointer;
      transition: background-color 0.3s;
      margin: 10px;
    }

    .circle {
      position: absolute;
      width: 26px;
      height: 26px;
      background-color: white;
      border-radius: 50%;
      top: 2px;
      left: 2px;
      transition: left 0.3s;
    }

    .toggle.on {
      background-color: #4caf50;
    }

    .toggle.on .circle {
      left: calc(100% - 28px);
    }

    .toggle-smart {
      position: relative;
      width: 60px;
      height: 30px;
      background-color: #ccc;
      border-radius: 15px;
      cursor: pointer;
      transition: background-color 0.3s;
    }

    .toggle-smart.on {
      background-color: #2d05bc;
    }

    .toggle-smart.on .circle-smart {
      left: calc(100% - 28px);
    }

    .btn-del {
      padding: 10px 15px;
      background-color: #ed3438;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      margin-bottom: 10px;
      gap: 5px;
      /* فاصله بین متن و SVG */

    }

    .btn-del:hover {
      background-color: #a32727;
    }

    .schedule-box {
      padding: 20px;
      border: 1px solid #ddd;
      border-radius: 8px;
      background-color: #f9f9f9;
    }

    label {
      display: block;
      margin: 10px 0 5px;
    }

    input,
    select {
      width: 100%;
      padding: 8px;
      margin-bottom: 10px;
      border: 1px solid #ccc;
      border-radius: 4px;
    }

    .btn-sch {
      padding: 10px 15px;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      margin-bottom: 10px;
    }

    .btn-sch:hover {
      background-color: #45a049;
    }

    .scheduled-list {
      margin-top: 20px;
    }

    .scheduled-list ul {
      list-style-type: none;
      padding: 0;
    }


    .overlay {
      display: none;
      /* پنهان در ابتدا */
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0, 0, 0, 0.5);
      /* پس‌زمینه تیره */
      backdrop-filter: blur(10px);
      /* مات کردن پس‌زمینه */
      justify-content: center;
      align-items: center;
    }

    .notification {
      background: rgba(255, 255, 255, 0.9);
      /* پس‌زمینه شفاف برای نوتیفیکیشن */
      color: #333;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 2px 15px rgba(0, 0, 0, 0.3);
      text-align: center;
    }

    .close-btn {
      margin-top: 10px;
      background-color: #b72e2e;
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s;
    }

    .close-btn:hover {
      background-color: #8f1a1a;
    }

    .box25 {
      min-height: 100px;
    }

    .form-container {
      background-color: white;
      padding: 30px;
      border-radius: 15px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
      text-align: center;
    }

    h2 {
      margin-bottom: 20px;
      color: #333;
    }

    .label-container {
      display: flex;
      align-items: center;
      justify-content: center;
      margin: 15px 0;
      cursor: pointer;
      padding: 10px;
      border-radius: 25px;
      transition: background-color 0.3s;
    }

    input[type="radio"] {
      display: none;
      /* پنهان کردن دکمه‌های رادیویی */
    }

    .custom-radio {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      border: 3px solid #4CAF50;
      display: inline-block;
      margin-right: 10px;
      position: relative;
      transition: background-color 0.3s, transform 0.3s;
    }

    input[type="radio"]:checked+.custom-radio {
      background-color: #4CAF50;
      transform: scale(1.2);
    }

    .label-text {
      font-size: 18px;
      color: #333;
    }

    .label-container:hover {
      background-color: rgba(76, 175, 80, 0.1);
    }

    .toggle-button {
      padding: 15px 30px;
      font-size: 16px;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      background-color: red;
      /* حالت اولیه OFF */
      transition: background-color 0.3s;
    }

    .on {
      background-color: green;
      /* حالت ON */
    }
  </style>
  <title>panel</title>
</head>

<body>
  <header>
    <div class="header">
      <div class="button-container">
        <button class="btn">
          <object data="src/logout.svg" type="image/svg+xml" width="30" height="30"></object>
        </button>
        <button class="btn">
          <object data="src/settings.svg" type="image/svg+xml" width="30" height="30"></object>
        </button>
        <button class="btn">
          <object data="src/missing_controller.svg" type="image/svg+xml" width="30" height="30"></object>
        </button>
      </div>
      <div class="title">پنل مدیریت</div>
      <div class="avatar">
        <div class="status"></div>
      </div>
    </div>
  </header>
  <div class="infoBox container">
    <div class="temperature box">
      <p class="titleInfo">دما و رطوبت</p>
      <p class="data" id="temp-hum">0 0</p>
      <div class="highlight hTemperature"></div>
    </div>
    <div class="gas box">
      <p class="titleInfo">میزان گاز</p>
      <p class="data" id="gas">0%</p>
      <div class="highlight hGas"></div>
    </div>
    <div class="light box">
      <p class="titleInfo">نور</p>
      <p class="data" id="light">0</p>
      <div class="highlight hLight"></div>
    </div>
  </div>
  <div class="container">
    <div class="box">
      <div class="containerTitle">
        <p>دزدگیر</p>
        <object data="src/verified_user.svg" type="image/svg+xml" width="30" height="30"></object>
      </div>
      <div id="toggleSwitchAlarms" class="toggle" onclick="toggleSwitch(this) ; toggleAlarm()">
        <div class="circle"></div>
      </div>
    </div>
    <div class="box">
      <div class="containerTitle">
        <p>کنترل لامپ</p>
        <object data="src/light_icon.svg" type="image/svg+xml" width="30" height="30"></object>
      </div>
      <div class="toggle" onclick="toggleSwitch(this) ; toggleDevice('lamp')">
        <div class="circle"></div>
      </div>
    </div>
    <div class="box">
      <div class="containerTitle">
        <p> سرمایشی</p>
        <object data="src/cold.svg" type="image/svg+xml" width="30" height="30"></object>
      </div>
      <div class="toggle" onclick="toggleSwitch(this) ;toggleDevice('cooling')">
        <div class="circle"></div>
      </div>
    </div>
  </div>
  <div class="container">
    <div class="bcolor box">
      <div class="security dis">
        <p>smart home</p>
      </div>
      <button class="toggle-button off" id="toggleButton" onclick="check()">OFF</button>
      <div id="smartHomeForm">
        <form>
          <label class="label-container">
            <input type="radio" name="type-smart" value="2" required>
            <span class="custom-radio"></span>
            <span class="label-text">P I R</span>
          </label>
          <label class="label-container">
            <input type="radio" name="type-smart" value="1">
            <span class="custom-radio"></span>
            <span class="label-text">LDR</span>
          </label>
        </form>
      </div>
      <div class="highlight hsmart">
      </div>
    </div>
  </div>
  <div class="overlay" id="overlay">
    <div class="notification">
      مقدار گاز از حد مجاز عبور کرده است !!!
      <br>
      <button class="close-btn" onclick="closeNotification()">بستن</button>
    </div>
  </div>

  <div class="overlay" id="overlay-alarm">
    <div class="notification">
      دزدگیر یه چیزایی حس میکنه!!!!
      <br>
      <button class="close-btn" onclick="closeNotificationAlarm()">بستن</button>
    </div>
  </div>
  <script>

    function toggleSwitch(element) {
      element.classList.toggle('on');
    }

    function showNotification() {
      const overlay = document.getElementById('overlay');
      overlay.style.display = 'flex'; // نمایش overlay
    }

    function showNotificationَAlarm() {
      const overlay = document.getElementById('overlay-alarm');
      overlay.style.display = 'flex'; // نمایش overlay
    }

    function closeNotification() {
      const overlay = document.getElementById('overlay');
      overlay.style.display = 'none'; // پنهان کردن overlay
    }
    function closeNotificationAlarm() {
      const overlay = document.getElementById('overlay-alarm');
      overlay.style.display = 'none'; // پنهان کردن overlay
    }
    let al = 1;
    function fetchData() {
      fetch('/get-sensor-data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp-hum').textContent = ` ${data.temperature}°C | ${data.humidity}%`;
          document.getElementById('light').textContent = `${Math.round(data.light * 100 / 1023)}%`;
          document.getElementById('gas').textContent = `${Math.round(data.gas * 100 / 1023)}%`;
          console.log(data.alarm);
          if (data.alarm == 1 && deviceStates["alarm"] == true) {
            if (al == 1) {
              showNotificationَAlarm();
              al = 0;
            }
          }
          if (Math.round(data.gas * 100 / 1023) > 20) {
            showNotification();
          }
        });
    }

    function updateButton(id, state, labels) {
      const button = document.getElementById(id);
      button.textContent = `${labels.prefix}: ${state ? labels.on : labels.off}`;
      button.className = state ? 'btn btn-on' : 'btn btn-off';
    }

    const deviceStates = {
      lamp: false, // وضعیت لامپ
      cooling: false,   // وضعیت سیستم سرمایشی
      alarm: false // وضعیت دزدگیر
    };

    function toggleDevice(device) {
      deviceStates[device] = !deviceStates[device];
      fetch('/toggle-device', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ device: device, status: deviceStates[device] })
      })
        .then(() => {
          updateButton(`${device}-btn`, deviceStates[device], {
            prefix: device === 'lamp' ? 'لامپ' : 'سیستم سرمایشی',
            on: 'روشن', off: 'خاموش'
          });
        })
        .catch(err => console.error(`Error toggling ${device}:`, err));
    }

    function toggleAlarm() {
      if (al == 0) {
        al = 1;
      }
      deviceStates.alarm = !deviceStates.alarm;
      fetch('/toggle-alarm', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ status: deviceStates.alarm })
      })
        .then(() => {
          updateButton('alarm-btn', deviceStates.alarm, {
            prefix: 'دزدگیر',
            on: 'فعال', off: 'غیرفعال'
          });
        })
        .catch(err => console.error("Error toggling alarm:", err));
    }
    let checkBox = 0;

    function check() {
      const selectedOption = document.querySelector('input[name="type-smart"]:checked');
      const button = document.getElementById('toggleButton');

      if (selectedOption) {
        checkBox = selectedOption.value;
        if (button.innerText === 'OFF') {
          button.innerText = 'ON';
          button.classList.add('on');
          button.classList.remove('off');
        } else {
          button.innerText = 'OFF';
          button.classList.remove('on');
          button.classList.add('off');
          checkBox = 0;
        }
        toggleSmartHome();
      } else {
        alert("لطفا یک گزینه رو انتخاب کن");
      }
    }


    function toggleSmartHome() {
      const formContainer = document.getElementById('smartHomeForm');
      const currentDisplay = window.getComputedStyle(formContainer).display;

      // نمایش یا پنهان کردن فرم
      formContainer.style.display = currentDisplay === 'none' ? 'block' : 'none';

      // ارسال درخواست fetch
      fetch('/toggle-smart', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ status: checkBox }),
      })
        .then((response) => response.json())
        .then((data) => {
          console.log("Server Response:", data);
        })
        .catch((error) => {
          console.error("Error:", error);
        });
    }

    function deactivateToggleById() {
      const toggle = document.getElementById("toggleSwitchAlarms");
      if (toggle && toggle.classList.contains("active")) {
        toggle.classList.remove("active");
        toggleAlarm();
      }
    }
    setInterval(fetchData, 2000);
    fetchData();
  </script>
</body>

</html>
)rawliteral";

void setup()
{
  Serial.begin(115200);
  Wire.begin(4, 5); // SDA, SCL pins for ESP8266
  WiFi.softAP(ssid, password);
  server.on("/", []()
            { server.send(200, "text/html", index_html); });
  server.on("/get-sensor-data", handleSensorData);
  server.on("/toggle-device", HTTP_POST, handleDeviceToggle);
  server.on("/toggle-alarm", HTTP_POST, handleAlarmToggle);
  server.on("/toggle-smart", HTTP_POST, handleSmartToggle);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}

void handleSensorData()
{
  Wire.requestFrom(ARDUINO_ADDRESS, 32);
  String data = "";
  while (Wire.available())
  {
    char c = Wire.read();
    data += c;
  }
  Serial.println("Received: " + data);

  StaticJsonDocument<200> json;
  int tempIndex = data.indexOf("T:");
  int humIndex = data.indexOf(",H:");
  int lightIndex = data.indexOf(",L:");
  int gasIndex = data.indexOf(",G:");
  int alarmIndex = data.indexOf(",A:");

  if (tempIndex != -1 && humIndex != -1 && lightIndex != -1 && gasIndex != -1)
  {
    json["temperature"] = data.substring(tempIndex + 2, humIndex).toFloat();
    json["humidity"] = data.substring(humIndex + 3, lightIndex).toFloat();
    json["light"] = data.substring(lightIndex + 3, gasIndex).toInt();
    json["gas"] = data.substring(gasIndex + 3, alarmIndex).toInt();
    json["alarm"] = data.substring(alarmIndex + 3).toInt();
  }

  String response;
  serializeJson(json, response);
  server.send(200, "application/json", response);
}

void handleDeviceToggle()
{
  StaticJsonDocument<200> json;
  String body = server.arg("plain");
  deserializeJson(json, body);

  String device = json["device"];
  bool status = json["status"];

  Wire.beginTransmission(ARDUINO_ADDRESS);
  if (device == "lamp")
  {
    Wire.write(status ? 'L' : 'l');
  }
  else if (device == "cooling")
  {
    Wire.write(status ? 'C' : 'c');
  }
  Wire.endTransmission();

  server.send(200, "text/plain", "OK");
}

void handleAlarmToggle()
{
  StaticJsonDocument<200> json;
  String body = server.arg("plain");
  deserializeJson(json, body);

  bool status = json["status"];
  Wire.beginTransmission(ARDUINO_ADDRESS);
  Wire.write(status ? 'A' : 'a');
  Wire.endTransmission();

  server.send(200, "text/plain", "OK");
}

void handleSmartToggle()
{
  StaticJsonDocument<200> json;
  String body = server.arg("plain");
  deserializeJson(json, body);

  int status = json["status"]; // دریافت وضعیت

  Wire.beginTransmission(ARDUINO_ADDRESS);

  if (status == 0)
  {
    Wire.write('s'); // غیرفعال کردن
  }
  else if (status == 1)
  {
    Wire.write('1'); // شدت روشنایی (LDR)
  }
  else if (status == 2)
  {
    Wire.write('2'); // حالت حرکتی (PIR)
  }

  Wire.endTransmission();

  server.send(200, "text/plain", "OK");
}