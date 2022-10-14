// 导入库，不用管
#include <ArduinoJson.h> // JSON库
#include <Arduino.h>
#include "Esp32MQTTClient.h" // ESP32的MQTT库
#include <PubSubClient.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 23
#define BUILTIN_LED 2
#define MSG_BUFFER_SIZE (50)
#define STASSID "xs"
#define STAPSK "11111111"

#define BeePIN 4
#define LedPIN 5

#define DHTTYPE    DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
float temp1;
float humi1;
int LED = 0;

StaticJsonDocument<400> sendJson;
StaticJsonDocument<400> readJson;

unsigned long lastUpdateTime = 0;   //
const char* ssid = STASSID;
const char* password = STAPSK;

char msg[MSG_BUFFER_SIZE];
// 定义WIFI客户端对象
WiFiClient espClient;
// MQTT对象
PubSubClient client(espClient);

const unsigned long updateInterval = 5000;

char sub[] = "Sub/test";
char pub[] = "Pub/test";
const char* mqtt_server = "117.50.176.171";

void setup() {
    // 串口开始 9600波特率 可以被电脑接收
  Serial.begin(9600);
    // 初始化引脚
  pinMode(BeePIN, OUTPUT);
  pinMode(LedPIN, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  // 开启WIFI
  setup_wifi();
  // 连接MQTT
  client.setServer(mqtt_server, 1883);
  // 如果接到消息 就会调用这个callback函数
  client.setCallback(callback);
  // 引脚操作
  digitalWrite(BUILTIN_LED, HIGH);
  digitalWrite(BeePIN, LOW);
  digitalWrite(LedPIN, LOW);
  // DHT传感器初始化
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
    // 如果断开了连接 则重新连接
  if (!client.connected()) {
    reconnect();
  }
    // MQTT一直运行
  client.loop();
    // 如果温度 高于25度 就响蜂鸣器
  if (temp1 >= 25) {
    digitalWrite(BeePIN, HIGH);
  }
  else if (temp1 < 25) { //低于的话就不响
    digitalWrite(BeePIN, LOW);
  }

  if (millis() - lastUpdateTime > updateInterval) {// 数据定时上报
    sendJsonData();
    lastUpdateTime = millis();
  }

}

// 定义回调函数 topic 所接收到主题 payload length 所接收到的消息的长度
void callback(char *topic, byte *payload, unsigned int length) {
    // 创建一个JOSN对象
  DynamicJsonDocument jsonBuffer(200);
  // 定义一个字符串
  String inputString;
    // 从头开始讲所有的消息保存在字符串inputString内
  for (int i = 0; i < length; i++) {
    inputString += (char)payload[i];
  }
  // 解析Josn格式数据inputString 给 jsonlBuffer
  deserializeJson(jsonBuffer, inputString);

  // 将解析到的数据以对象的形式赋给root
  JsonObject root = jsonBuffer.as<JsonObject>();

  //所接收的的信息中 Led 的值赋给 LED
  LED = root["Led"];
  // 串口打印 LED
  Serial.println(LED);
  if (LED == 1) { // 如果是 1 就将LED引脚拉高
    digitalWrite(LedPIN, HIGH);
  } 
  else {           // 否则拉低
    digitalWrite(LedPIN, LOW);
  }

}

void setup_wifi() {
    //串口换行
  Serial.println();
  // 串口打印正在连接WIFI
  Serial.print("Connecting to "); // 
  Serial.println(ssid);
  // 连接wifi
  WiFi.begin(ssid, password);
    // 检测WIFI状态 没有连上就一直等并且串口打印.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    // 打印当前的设备IP
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
// 如果没有连接上MQTT的话就一直重复以下操作
  while (!client.connected()) {
    
    String clientId = "Esp32";
    clientId += "888888";

    // 连接 用 Eso32888888 为名字连接MQTT服务器 
    if (client.connect(clientId.c_str())) {
        // 如果连接上了则订阅主题
      client.subscribe(sub);
    }
    
    else {
    // 没有连接上则返回当前的状态码给串口打印出来
      Serial.print(client.state());
      delay(5000);
    }
  }

}

void sendJsonData() {
    // 发送 Json格式的数据
  String pubres;

  sensors_event_t event0;
  sensors_event_t event1;
    // 读取温度和湿度
  dht.temperature().getEvent(&event0);
  dht.humidity().getEvent(&event1);
  
  if (isnan(event0.temperature) || isnan(event1.relative_humidity)) {
      // 如果有一个数据没有读取出来就串口打印读取错误
    Serial.println(F("Error reading temperature!"));
    Serial.println(F("Error reading humidity!"));
  }
  else {
      // 否则就将温度与湿度保存为在数组 sendJson 中
    sendJson["Temp"] = event0.temperature;
    sendJson["Humi"] = event1.relative_humidity;
  }
  // 将sendJson数组改成Json格式 存储在pubres中
  serializeJson(sendJson, pubres);
  // 获取Json报文的长度
  int str_len = pubres.length() + 1;
  char char_array[str_len];
    // 将pubres的格式改为ASCLII格式的
  pubres.toCharArray(char_array, str_len);
  // MQTT 在pub主题发布这个Json报文
  client.publish(pub, char_array);

}
