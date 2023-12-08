#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
// memasukan library telegram bot
#include <UniversalTelegramBot.h>
// memasukan library arduino json (digunakan jika ingin menggunakan telegram bot)
#include <ArduinoJson.h>

// definisi nama dan pass wifi yang akan digunakan
const char* ssid = "ESP32_SmartCentralLock";
const char* password = "ESP32_SmartCentralLock24";

// definisi telegram bot token yang didapat di bot father
String BOTtoken = "6905701441:AAHd6r1si6Lgb2OQGh-JocoXzLrCKhnD_TQ";  // your Bot Token (Get from Botfather)

// definisi id dari telegram user
String CHAT_ID = "898149240";
bool sendPhoto = false;
bool motionDetected = false;

// menjadikan esp 32 cam sebagai client untuk dapat terhubung ke router
WiFiClientSecure clientTCP;
// inisialisasi telegram bot oleh esp 32 cam
UniversalTelegramBot bot(BOTtoken, clientTCP);
// variabel untuk menentukan nyala atau matinya led flash
bool flashState = LOW;

//variabel delay untuk mengecekan pesan yang masuk dari telegram setiap 1 detik
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//definisi jenis pin yang digunakan di esp 32 cam
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// function untk mengecek pesan yang diterima oleh telegram
void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;
    // jika pesan adalah "xxx" maka "xxx"
    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo request");
      bot.sendMessage(chat_id, "");
    }
  }
}

// funtion yang dapat di panggil untuk mengirimkan gambar ke telegram
String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";
  // function untuk pengambilan gambar menggunakan esp 32 cam
  digitalWrite(4, 1);
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  digitalWrite(4, 0);
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }

  Serial.println("Connect to " + String(myDomain));


  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");

    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    clientTCP.println("POST /bot" + BOTtoken + "/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if (n + 1024 < fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        clientTCP.write(fbBuf, remainder);
      }
    }

    clientTCP.print(tail);

    esp_camera_fb_return(fb);

    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;

    while ((startTimer + waitTime) > millis()) {
      Serial.print(".");
      delay(100);
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state == true) getBody += String(c);
        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length() > 0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  return getBody;
}

void setup() {
  // function bawaan esp 32 cam untuk menghindari program crash
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // memanggil serial monitor untuk keperluan debuging
  Serial.begin(115200);
  pinMode(13, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, 0);
  // menghubungkan ke internet
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
  // konfigurasi kamera di esp 32 cam
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //menentukan jenis kualitas gambar esp 32 cam
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 semakin kecil nilai makan akan semakin besar kualitas
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 15;  //0-63 semakin kecil nilai makan akan semakin besar kualitas
    config.fb_count = 1;
  }

  // pengecekan kamera untuk memastikan tidak ada kesalahan wiring
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  // Drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  // menentukan sensor PIR di pin no 13
  // err = gpio_isr_handler_add(GPIO_NUM_13, &detectsMovement, (void *)13);
  if (err != ESP_OK)
  {
    Serial.printf("handler add failed with error 0x%x \r\n", err);
  }
  //  err = gpio_set_intr_type(GPIO_NUM_13, GPIO_INTR_POSEDGE);
  if (err != ESP_OK)
  {
    Serial.printf("set intr type failed with error 0x%x \r\n", err);
  }
  Serial.println("OK");
}

void loop() {
  if (digitalRead(13) == 0) {
    while (digitalRead(13) == 0) {
      delay(100);
    }
    sendPhoto = true;
  }

  if (sendPhoto) {
    Serial.println("SEND PHOTO");
    bot.sendMessage(CHAT_ID, "Foto Akan Segera Dikirim !");
    sendPhotoTelegram();
    sendPhoto = false;
  }

  // menunggu perintah dari user yang dikirim kan melalui telegram bot
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  delay(100);
}
