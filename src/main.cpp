#include <Arduino.h>
#include <GyverOLED.h>
#include <FastBot2.h>
#include <StringUtils.h>

#include "models/fencer.h"
#include "models/sparring.h"

// Defines:
#include "env_data.h"
// TG Bot
FastBot2 bot;

// OLED:
GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;
static int8_t pointer = 0;
static bool in_sparring_mode = false;

// IR sensor: TODO: IMPLEMENT
define IR_PIN

// GyverSettings:
#include <SettingsESP.h>
#include <GyverDBFile.h>
#include <LittleFS.h>

// Machine:
#include "equipment/machine.h"
#define LHS_SIG_PIN 25
#define LHS_GND_PIN 26
#define RHS_SIG_PIN 32
#define RHS_GND_PIN 33
Equipment::Machine machine(LHS_SIG_PIN, LHS_GND_PIN, RHS_SIG_PIN, RHS_GND_PIN);

// ESP-NOW
#include <esp_now.h>
//24:0A:C4:00:01:11
uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0x00, 0x01, 0x11};
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

#define LHS_PIN 12
#define RHS_PIN 13

GyverDBFile db(&LittleFS, "/data.db");
SettingsESP sett("Wi-Fi config", &db);
sets::Logger logger(1024);

DB_KEYS(kk,
    wifi_ssid,
    wifi_pass,
    pass,
    mls,
    lhs_fencer,
    rhs_fencer,
    bout_status,
    logging,
    apply);

void build(sets::Builder& b) {
    // if (b.beginGroup("Group 1")) {
        
    //     b.endGroup();
    // }
    // if (b.beginGuest()) {
    //     b.Pass(kk::pass, "Password");
    //     if (b.Button("Save & Restart")) {
    //         db.update();
    //         b.reload();
    //     }
    //     b.endGuest();
    // }
    // Following is unavailable for non-authorized users:
    // ---------- Wi-Fi -------
    if (b.beginGroup("Wi-Fi settings")) {   
        b.Input(kk::wifi_ssid, "SSID");
        b.Pass(kk::wifi_pass, "Password");
        if (b.Button(kk::apply, "Save & Restart")) {
            db.update();
            ESP.restart();
        }
        b.endGroup();
    }

    // ---------- Bout settings  -------
    if (b.beginGroup("Bouts")) {
        b.LED(kk::bout_status, "Bout status: ");
        if (b.beginRow()) {
            b.Number(kk::lhs_fencer, "LHS fencer: ");
            b.Number(kk::rhs_fencer, "RHS fencer: ");
        } 
        b.endRow();
        switch (db[kk::bout_status].toBool())
        {
        case 0: // inactive bout
            if (b.Button("Start bout")) {
                logger.print("["); logger.print(millis()); logger.print("]"); 
                logger.print(" ->> STARTING BOUT: ");
                logger.print(db[kk::lhs_fencer]); 
                logger.print(" VS "); 
                logger.println(db[kk::rhs_fencer]);
                db.set(kk::bout_status, 1);
                db.update(); // update LED status
            }
            break;
        case 1: // active bout
            if (b.Button("End bout")) {
                logger.print("["); logger.print(millis()); logger.print("]"); 
                logger.print(" ->> ENDING BOUT: ");
                logger.print(db[kk::lhs_fencer]); 
                logger.print(" VS "); 
                logger.println(db[kk::rhs_fencer]);
                db.set(kk::bout_status, 0);
                db.update(); // update LED status
            }
            break;
        }
        // TODO: status page of bout started/ended (LED?)
        b.endGroup();
    }

    // ---------- Equipment test  -------
    if (b.beginMenu("Testing")) {
        if (b.beginRow()) {
            if (b.Button("Test LHS")) {
                machine.point(Equipment::Side::LHS);
            }
            if (b.Button("Test RHS")) {
                machine.point(Equipment::Side::RHS);
            }
        } 
        b.endRow();
        if (b.Button("Test BOTH")) {
            machine.test_pins();
        }
        b.endMenu();
    }

    // ---------- Logging  -------
    if (b.beginGroup("Logging")) {
        b.Log(kk::logging, logger);
        b.endGroup();
    }
}

void update(sets::Updater& upd) {
    upd.update(kk::mls, millis());
    upd.update(kk::logging, logger);
}

void btn1_click() {
    oled.clear();
    oled.setScale(2);
    oled.home();
    oled.print("BTN CLICK");
}


void initiate_sparring(fb::Update& u) {
    if (in_sparring_mode) { return; }
    uint8_t fighters[2];
    u.message().text().substring(3).split(fighters, 2, Text(F(" ")));
    uint8_t fighter_1 = fighters[0];
    uint8_t fighter_2 = fighters[1];

    oled.clear();
    oled.setScale(4);
    oled.setCursor(0, 3);
    oled.print(fighter_1);
    oled.setCursor(64, 3);
    oled.print(fighter_2);
    oled.update();
}

void cancel_sparring(fb::Update& u) {
    if (!in_sparring_mode) { return; }
    in_sparring_mode = !in_sparring_mode;
    bot.sendMessage(fb::Message(F("Cancelled between 1 and 2"), u.message().from().id()));
}

void standard_text_update(fb::Update& u) {
    pointer++;
    if (pointer > 7) {
        pointer = 0;
    }
    oled.setCursor(0, pointer);
    oled.print("Echo: "); oled.print(u.message().from().id());
    bot.sendMessage(fb::Message(u.message().text(), u.message().from().id()));
}

void main_update(fb::Update& u) {
    if (u.isMessage() && u.message().text().startsWith(Text(F("/s")))) initiate_sparring(u);
    else if (u.isMessage() && u.message().text() == Text(F("/cancel"))) cancel_sparring(u);
    else if (u.isMessage()) standard_text_update(u);
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    pinMode(LHS_PIN, INPUT);
    pinMode(RHS_PIN, INPUT);
    machine.init();

    db.begin();
    db.init(kk::wifi_ssid, WIFI_SSID);
    db.init(kk::wifi_pass, WIFI_PASS);
    db.init(kk::bout_status, 0);
    db.init(kk::lhs_fencer, 0);
    db.init(kk::rhs_fencer, 0);
    db.init(kk::pass, "");

    oled.init();   
    oled.clear(); oled.home();
    oled.print("Connecting");
    oled.update();

    WiFi.begin(db[kk::wifi_ssid], db[kk::wifi_pass]);
    uint8_t counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        oled.print(".");
        if(++counter >= 10) {
            oled.setCursor(0, 2);
            oled.autoPrintln(true);
            oled.print(F("Connection failed. Please, hard-reset"));
            return;
        }
        delay(500);
    }
    oled.clear();
    bot.attachUpdate(main_update);
    bot.setToken(F(BOT_TOKEN));
    bot.setPollMode(fb::Poll::Long, 20000);
    oled.home();
    oled.print("Bot ON | "); oled.print(WiFi.localIP());
    oled.setCursor(0, 2); oled.print(WiFi.macAddress());
    Serial.println(WiFi.macAddress());
    sett.begin();
    sett.setPass(F(HUB_PASS));
    sett.onBuild(build);
    sett.onUpdate(update);
    
    LittleFS.begin(true);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);
}

static uint32_t timer1 = 0;
void loop() {
    // sender: 24:0A:C4:00:01:11
    // receiver 1: 24:0A:C4:00:01:10
    bot.tick();
    sett.tick();
    if (millis() - timer1 >= 2000) {
        // if (digitalRead(LHS_PIN)) {
        //     machine.point(Equipment::Side::LHS);
        // }
        // if (digitalRead(RHS_PIN)) {
        //     machine.point(Equipment::Side::RHS);
        // }
        strcpy(myData.a, "THIS IS A CHAR");
        myData.b = random(1,20);
        myData.c = 1.2;
        myData.d = false;
    
        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        }
        else {
            Serial.println("Error sending the data");
        }
        timer1 = millis();
    }
}