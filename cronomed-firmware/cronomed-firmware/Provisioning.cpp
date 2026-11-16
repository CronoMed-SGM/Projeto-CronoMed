#include "Provisioning.h"
#include "Alarm.h"
#include "CronoNet.h"
#include "config.h"
#include "Settings.h"

#if PROVISIONING_BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <WiFi.h>
#endif

namespace {
#if PROVISIONING_BLE
const char* SERVICE_UUID     = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* CHAR_MAC_UUID    = "19b10000-e8f2-537e-4f6c-d104768a1215";
const char* CHAR_SSID_UUID   = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* CHAR_PASS_UUID   = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* CHAR_SECRET_UUID = "19b10003-e8f2-537e-4f6c-d104768a1214";
const char* CHAR_BROKER_UUID = "19b10004-e8f2-537e-4f6c-d104768a1214";

String g_ssid, g_pass, g_broker, g_secret;
bool g_haveSsid = false;
volatile bool g_done = false;
uint32_t g_doneAt = 0;
bool g_started = false;

class WriteCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    String uuid = String(c->getUUID().toString().c_str());
    String value = String(c->getValue().c_str());

    if (uuid.equalsIgnoreCase(CHAR_SSID_UUID)) {
      g_ssid = value;
      g_haveSsid = value.length() > 0;
      LOGF("[ble] Nova rede Wi-Fi recebida: %s\n", g_ssid.c_str());
    } else if (uuid.equalsIgnoreCase(CHAR_PASS_UUID)) {
      g_pass = value;
      LOGF("[ble] Senha do Wi-Fi recebida\n");
    } else if (uuid.equalsIgnoreCase(CHAR_BROKER_UUID)) {
      g_broker = value;
      LOGF("[ble] Broker recebido: %s\n", g_broker.c_str());
    } else if (uuid.equalsIgnoreCase(CHAR_SECRET_UUID)) {
      g_secret = value;
      if (g_haveSsid) {
        if (g_secret == "KEEP_EXISTING_SECRET" || g_secret.length() == 0) {
          LOGF("[ble] Reconfigurando apenas Wi-Fi (mantendo vinculo e dados)...\n");
          Settings::updateWifiOnly(g_ssid, g_pass, g_broker);
        } else {
          LOGF("[ble] Gravando pareamento inicial completo...\n");
          Settings::saveProvisioned(g_ssid, g_pass, g_broker, g_secret);
        }
        g_done = true;
        g_doneAt = millis();
      }
    }
  }
};

WriteCallbacks g_callbacks;
#endif
uint32_t g_pressStart = 0;
}  // namespace

namespace Provisioning {

void begin() {
#if PROVISIONING_BLE
  if (g_started) return;
  String mac = CronoNet::macAddress();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF); // Libera RAM e antena 2.4GHz para o BLE funcionar sem crash
  delay(100);

  String bleName = "CronoMed-" + mac.substring(12, 14) + mac.substring(15, 17);
  LOGF("============================================\n");
  LOGF("MODO DE PAREAMENTO BLUETOOTH ATIVO\n");
  LOGF("MAC desta caixa: %s | Nome BLE: %s\n", mac.c_str(), bleName.c_str());
  LOGF("Abra 'Cadastrar caixa' ou 'Reconfigurar Wi-Fi' no navegador.\n");
  LOGF("============================================\n");

  BLEDevice::init(bleName.c_str());
  BLEServer* server = BLEDevice::createServer();
  // Importante: 30 handles para suportar 5 characteristics sem retornar nullptr!
  BLEService* service = server->createService(BLEUUID(SERVICE_UUID), 30, 0);

  BLECharacteristic* macChar = service->createCharacteristic(CHAR_MAC_UUID, BLECharacteristic::PROPERTY_READ);
  if (macChar) macChar->setValue(mac.c_str());

  const char* writeUuids[] = {CHAR_SSID_UUID, CHAR_PASS_UUID, CHAR_BROKER_UUID, CHAR_SECRET_UUID};
  for (const char* u : writeUuids) {
    BLECharacteristic* ch = service->createCharacteristic(u, BLECharacteristic::PROPERTY_WRITE);
    if (ch) ch->setCallbacks(&g_callbacks);
  }
  service->start();

  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();
  g_started = true;
#endif
}

void loop() {
#if PROVISIONING_BLE
  if (g_started && g_done && millis() - g_doneAt > 1200) {
    LOGF("[ble] Configuracao concluida. Reiniciando para conectar ao Wi-Fi...\n");
    BLEDevice::deinit(true);
    ESP.restart();
  }
#endif
}

void beginButton() {
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
#if RESET_BUTTON_ALT_PIN >= 0
  pinMode(RESET_BUTTON_ALT_PIN, INPUT_PULLUP);
#endif
}

void checkResetButton() {
  bool pressed = digitalRead(RESET_BUTTON_PIN) == LOW;
#if RESET_BUTTON_ALT_PIN >= 0
  pressed = pressed || digitalRead(RESET_BUTTON_ALT_PIN) == LOW;
#endif
  if (pressed) {
    if (g_pressStart == 0) g_pressStart = millis() ? millis() : 1;
    if (millis() - g_pressStart >= RESET_BUTTON_HOLD_MS) {
      LOGF("[reset] Botao segurado por 3s: ativando modo Bluetooth para configurar Wi-Fi\n");
      Alarm::blip(300);
      Settings::factoryReset();
      delay(300);
      ESP.restart();
    }
  } else {
    g_pressStart = 0;
  }
}

}  // namespace Provisioning