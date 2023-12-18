#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
const char* ssid = "FREE";
const char* password = "55555555";
const int LED = 14;
const int BUTTON_LED = 12;
const int LED_BAOCHAY = 27;
const int CHUONG_BAOCHAY = 26;
const int BUTTON_BAOCHAY = 25;
const int BUTTON_AUTO = 33;
const int AO = 32;
const int DO = 35;
const int LED_AUTO = 13;
int tt_LED = LOW;
int tt_BAOCHAY = LOW;
int tt_AUTO = LOW;
int tt_DO = LOW; 
int tt_AO = 0;
int tt_LED_AUTO = LOW;
int buttonled;
int buttonbaochay;
int buttonauto;
int last_buttonled;
int last_buttonbaochay;
int last_buttonauto;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_LED, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    pinMode(DO,INPUT);
    pinMode(AO,INPUT);
    pinMode(LED_BAOCHAY,OUTPUT);
    pinMode(CHUONG_BAOCHAY,OUTPUT);
    pinMode(BUTTON_BAOCHAY,INPUT_PULLUP);
    pinMode(BUTTON_AUTO,INPUT_PULLUP);
    pinMode(LED_AUTO,OUTPUT);
    buttonled = digitalRead(BUTTON_LED);
    buttonbaochay = digitalRead(BUTTON_BAOCHAY);
    buttonauto = digitalRead(BUTTON_AUTO);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Đang kết nối WiFi...");
    }
    Serial.println("Kết nối thành công!!!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    server.on("/", HTTP_GET, xuLyRoot);
    server.begin();
    webSocket.begin();
    webSocket.onEvent(xuLySuKienWebSocket);
}
String web = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Điều Khiển Nhà thông minh</title>
    <meta name='viewport' content='width=device-width, initial-scale=1.0' />
    <meta charset='UTF-8'>
    <style>
        body {
          background-color: #F7F9FD;
          text-align: center;
        }
        #led{
          color: red;
        }
        #autobaochay{
          color: red;
        }
        #baochay{
          color: red;
        }
    </style>
</head>
<body>
    <h1>Trạng Thái Đèn: <span id='led'>-</span></h1>
    <button type='button' id='btnLedOn'>
      <h1>Bật</h1>
    </button>
    <button type='button' id='btnLedOff'>
      <h1>Tắt</h1>
    </button>
    <h1>Trạng Thái Báo cháy tự động: <span id='autobaochay'>-</span></h1>
    <button type='button' id='btnAutoOn'>
      <h1>Bật</h1>
    </button>
    <button type='button' id='btnAutoOff'>
      <h1>Tắt</h1>
    </button>
    <h1>Hệ thống báo cháy: <span id='baochay'>-</span></h1>
    <button type='button' id='btnBaochayOn'>
      <h1>Bật</h1>
    </button>
    <button type='button' id='btnBaochayOff'>
      <h1>Tắt</h1>
    </button>
</body>
<script>
    var Socket;
    document.getElementById('btnLedOn').addEventListener('click', batDen);
    document.getElementById('btnLedOff').addEventListener('click', tatDen);
    document.getElementById('btnAutoOn').addEventListener('click', batAuto);
    document.getElementById('btnAutoOff').addEventListener('click', tatAuto);
    document.getElementById('btnBaochayOn').addEventListener('click', batBaoChay);
    document.getElementById('btnBaochayOff').addEventListener('click', tatBaoChay);
    
    function init() {
        Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
        Socket.onmessage = function(event) {
            xuLyLenhNhanDuoc(event);
        };
    }
    function xuLyLenhNhanDuoc(event) {
        var obj = JSON.parse(event.data);
        
        document.getElementById('led').innerHTML = obj.led;
        console.log(obj.led); 
        document.getElementById('autobaochay').innerHTML = obj.autobaochay;
        console.log(obj.autobaochay);
        document.getElementById('baochay').innerHTML = obj.baochay;
        console.log(obj.baochay);
    }
    function batDen() {
      Socket.send('0');
    }
    function tatDen() {
      Socket.send('1');
    }
    function batAuto() {
      Socket.send('2');  
    }
    function tatAuto() {
      Socket.send('3');  
    }
    function batBaoChay() {
      Socket.send('4');  
    }
    function tatBaoChay() {
      Socket.send('5');  
    }
    window.onload = function(event) {
      init();
    }
</script>
</html>
)";


void xuLyWebSocket() {
    webSocket.loop();
}

void xuLyRoot() {
    server.send(200, "text/html", web);
}

void xuLySuKienWebSocket(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    if (type == WStype_TEXT) {
        if (payload[0] == '0') {
          digitalWrite(LED, HIGH);
          tt_LED = HIGH;
          capNhatTrangThaiDen(true);                     
        } else if (payload[0] == '1') {
          digitalWrite(LED, LOW);
          tt_LED = LOW;
          capNhatTrangThaiDen(false);

        } else if (payload[0] == '2') {
            tt_AUTO = HIGH;
            digitalWrite(LED_AUTO,HIGH);
            capNhatTrangThaiAuto(true);
        } else if (payload[0] == '3') {
            tt_AUTO = LOW;
            digitalWrite(LED_AUTO,LOW);
            capNhatTrangThaiAuto(false);

        } else if (payload[0] == '4') {
            tt_BAOCHAY = HIGH;
            digitalWrite(LED_BAOCHAY,HIGH);
            digitalWrite(CHUONG_BAOCHAY,HIGH);
            capNhatTrangThaiBaochay(true);
        } else if (payload[0] == '5') {
            tt_BAOCHAY = LOW;
            digitalWrite(LED_BAOCHAY,LOW);
            digitalWrite(CHUONG_BAOCHAY,LOW);
            capNhatTrangThaiBaochay(false);
        }
    }
}

void dieukhienden() {
  last_buttonled = buttonled;
  buttonled = digitalRead(BUTTON_LED);
  
  if (last_buttonled == HIGH && buttonled == LOW) {
    tt_LED = !tt_LED;
    if (tt_LED == HIGH) {
      digitalWrite(LED, HIGH);
      capNhatTrangThaiDen(true);
      Serial.println("Bật đèn");
    } else {
      digitalWrite(LED, LOW);
      capNhatTrangThaiDen(false);
      Serial.println("Tắt đèn");
    }
  }
}

void capNhatTrangThaiDen(bool trangThai) {
    String trangThaiDen = trangThai ? "ĐANG BẬT" : "ĐANG TẮT";
    String json = "{\"led\":\"" + trangThaiDen + "\"}";
    webSocket.broadcastTXT(json.c_str());
}

void autocontrol(){
  last_buttonauto = buttonauto;
  buttonauto = digitalRead(BUTTON_AUTO);

  if(last_buttonauto == HIGH && buttonauto == LOW){
    tt_AUTO = !tt_AUTO;
    if(tt_AUTO == HIGH){
      digitalWrite(LED_AUTO,HIGH);
      capNhatTrangThaiAuto(true);
      Serial.println("Bật Auto");
    }else{
      digitalWrite(LED_AUTO,LOW);
      capNhatTrangThaiAuto(false);
      Serial.println("Tắt Auto");
    }
  }
}

void capNhatTrangThaiAuto(bool trangThai) {
    String trangThaiAuto = trangThai ? "ĐANG BẬT" : "ĐANG TẮT";
    String json = "{\"autobaochay\":\"" + trangThaiAuto + "\"}";
    webSocket.broadcastTXT(json.c_str());
}

void baochaytudong(){
  tt_DO = digitalRead(DO);
  tt_AO = digitalRead(AO);

  if(tt_AUTO == HIGH){
    if(tt_AO<10 && tt_DO == HIGH && tt_BAOCHAY == LOW){
      tt_BAOCHAY = HIGH;
      capNhatTrangThaiBaochay(true);
      digitalWrite(LED_BAOCHAY,HIGH);
      digitalWrite(CHUONG_BAOCHAY,HIGH);
      Serial.println("Phát hiện cháy");
    }
  }
}

void baochaythucong(){
  last_buttonbaochay = buttonbaochay;
  buttonbaochay = digitalRead(BUTTON_BAOCHAY);

  if(last_buttonbaochay == HIGH && buttonbaochay == LOW){
    tt_BAOCHAY = !tt_BAOCHAY;
    if(tt_BAOCHAY == HIGH){
      digitalWrite(LED_BAOCHAY,HIGH);
      digitalWrite(CHUONG_BAOCHAY,HIGH);
      capNhatTrangThaiBaochay(true);
      Serial.println("Bật báo cháy");
    }
    else{
      digitalWrite(LED_BAOCHAY,LOW);
      digitalWrite(CHUONG_BAOCHAY,LOW);
      capNhatTrangThaiBaochay(false);
      Serial.println("Tắt báo cháy");
    }
  }
}

void capNhatTrangThaiBaochay(bool trangThai) {
    String trangThaiBaochay = trangThai ? "Phát hiện cháy" : "Không phát hiện cháy";
    String json = "{\"baochay\":\"" + trangThaiBaochay + "\"}";
    webSocket.broadcastTXT(json.c_str());
}

void loop() {
    server.handleClient();
    xuLyWebSocket();
    dieukhienden();  
    autocontrol();
    baochaytudong();
    baochaythucong();
}
