// ============================================================================
// CronoMed - Firmware da caixa de medicamentos (ESP32)
//
// - Recebe do servidor, via MQTT, o plano de horarios e grava na flash (LittleFS).
// - Dispara sozinha os alarmes (LED + buzzer) pelo relogio, mesmo sem internet.
// - Detecta a abertura de cada compartimento (reed switch) e envia o evento ao servidor,
//   guardando-o na flash ate o servidor confirmar.
// - Envia heartbeat com o nivel da bateria.
// - Sem Wi-Fi salvo, sobe o pareamento por Bluetooth (pagina "Cadastrar caixa" da web).
//   Botao de reset (GPIO 4) segurado por 3 s apaga o Wi-Fi salvo e volta ao pareamento.
//
// Arduino IDE: placa "ESP32 Dev Module" + Tools > Partition Scheme > "Huge APP (3MB No OTA/1MB SPIFFS)".
// Configuracao: config.h (pinos e tempos) e secrets.h (Wi-Fi e MQTT; copie de secrets.example.h).
// Contratos de mensagem: docs/API-E-MQTT.md do repositorio do backend.
// ============================================================================
#include <WiFi.h>
#include "Alarm.h"
#include "Battery.h"
#include "EventQueue.h"
#include "CronoNet.h"
#include "Plan.h"
#include "Provisioning.h"
#include "Sensors.h"
#include "Settings.h"
#include "config.h"
#include "util.h"

static uint32_t lastPrune = 0;
static bool g_pairing = false;
static uint32_t g_bootTime = 0;

static void handleOpening(uint8_t comp, uint32_t now) {
  PlanDose* dose = Plan::findActive(comp, now);
  const char* doseId = dose ? dose->id : nullptr;
  EventQueue::pushOpening(comp, doseId);
  if (dose) {
    Plan::markDone(dose->id);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  LOGF("\nCronoMed - iniciando\n");
  g_bootTime = millis();

  Settings::begin();
  Provisioning::beginButton();
  Plan::begin();
  EventQueue::begin();
  Sensors::begin();
  Battery::begin();
  Alarm::begin();
  Alarm::selfTest();

  if (Settings::provisioningNeeded()) {
    g_pairing = true;
    Provisioning::begin();
  } else {
    LOGF("Wi-Fi salvo: %s | broker: %s\n", Settings::wifiSsid().c_str(), Settings::broker().c_str());
    CronoNet::begin();
  }
}

void loop() {
  Provisioning::checkResetButton();

  if (g_pairing) {
    Provisioning::loop();
  } else {
    CronoNet::loop();
    // Se a rede Wi-Fi salva nao conectar em 30 segundos (ex: mudou de roteador/local),
    // desliga o Wi-Fi e liga o Bluetooth automaticamente para permitir Reconfigurar Wi-Fi!
    if (WiFi.status() != WL_CONNECTED && (millis() - g_bootTime > 30000UL)) {
      LOGF("[wifi] Rede '%s' nao encontrada apos 30s. Ativando Bluetooth para reconfigurar Wi-Fi...\n", Settings::wifiSsid().c_str());
      g_pairing = true;
      Provisioning::begin();
    }
  }

  uint32_t now = Util::nowEpoch();
  Alarm::update(now);

  uint8_t missedComp;
  char missedDose[37];
  while (Alarm::takeMissed(&missedComp, missedDose)) {
    EventQueue::pushNotOpened(missedComp, missedDose);
    Plan::markDone(missedDose);
  }

  uint8_t comp;
  bool opened;
  while (Sensors::poll(&comp, &opened)) {
    if (opened) {
      handleOpening(comp, now);
    } else {
      Alarm::confirmClosed(comp);
    }
  }

  if (millis() - lastPrune > 600000UL) {
    lastPrune = millis();
    EventQueue::prune(now);
  }

  delay(2);
}