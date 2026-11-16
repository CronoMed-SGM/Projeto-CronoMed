#pragma once
#include <Arduino.h>

namespace Settings {
void begin();
const String& wifiSsid();
const String& wifiPass();
const String& broker();
const String& deviceSecret();
bool hasStoredWifi();
bool provisioningNeeded();

/** Grava pareamento completo (primeiro uso via BLE). */
void saveProvisioned(const String& ssid, const String& pass, const String& broker, const String& secret);

/** Atualiza apenas a rede Wi-Fi (e opcionalmente o broker) sem perder o segredo nem os dados da caixa (Regra 5). */
void updateWifiOnly(const String& ssid, const String& pass, const String& broker = "");

/** Reset de fabrica (Regra 6 - Portabilidade). */
void factoryReset();
}  // namespace Settings