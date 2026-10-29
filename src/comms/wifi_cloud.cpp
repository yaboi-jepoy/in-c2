#include "wifi_cloud.h"
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <time.h>
#include "config.h"

namespace {
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;
bool firebaseReady = false;
bool littleFsReady = false;
bool backupPending = false;
unsigned long lastBackupFlush = 0;
unsigned long lastFirebaseAuthAttempt = 0;

bool backupFileExists() {
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
    return false;
  }

  File entry = root.openNextFile();
  while (entry) {
    bool found = String(entry.name()) == BACKUP_LOG_PATH;
    entry.close();
    if (found) {
      root.close();
      return true;
    }
    entry = root.openNextFile();
  }
  root.close();
  return false;
}

bool ensureFirebaseAuth() {
  if (firebaseReady) {
    return true;
  }
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  if (millis() - lastFirebaseAuthAttempt < FIREBASE_AUTH_RETRY_MS) {
    return false;
  }
  lastFirebaseAuthAttempt = millis();

  if (!Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    const String error(firebaseConfig.signer.signupError.message.c_str());
    Serial.printf("cloud: Firebase sign-up failed: %s\n", error.c_str());
    if (error.indexOf("CONFIGURATION_NOT_FOUND") >= 0) {
      Serial.println("cloud: enable Anonymous Authentication for this Firebase project");
    }
    return false;
  }
  firebaseReady = true;
  Serial.println("cloud: Firebase authenticated");
  return true;
}

bool uploadJson(const char* jsonText) {
  if (!ensureFirebaseAuth()) {
    return false;
  }

  FirebaseJson source;
  if (!source.setJsonData(jsonText)) {
    Serial.println("cloud: invalid JSON record");
    return false;
  }

  FirebaseJson document;
  FirebaseJsonData value;

  if (!source.get(value, "deviceId") || !value.success) {
    Serial.println("cloud: record is missing deviceId");
    return false;
  }
  document.set("fields/deviceId/stringValue", value.stringValue);

  if (!source.get(value, "source") || !value.success) {
    Serial.println("cloud: record is missing source");
    return false;
  }
  document.set("fields/source/stringValue", value.stringValue);

  const char* numericFields[] = {
      "waterTemperature", "ph", "dissolvedOxygen", "airTemperature",
      "humidity", "pressure"};
  for (const char* field : numericFields) {
    if (!source.get(value, field) || !value.success) {
      Serial.printf("cloud: record is missing %s\n", field);
      return false;
    }
    document.set(String("fields/") + field + "/doubleValue", value.doubleValue);
  }

  const char* locationFields[] = {"latitude", "longitude"};
  for (const char* field : locationFields) {
    if (!source.get(value, field) || !value.success) {
      Serial.printf("cloud: record is missing %s\n", field);
      return false;
    }
    if (value.type == "null") {
      document.set(String("fields/") + field + "/nullValue", "NULL_VALUE");
    } else {
      document.set(String("fields/") + field + "/doubleValue", value.doubleValue);
    }
  }

  if (!source.get(value, "measuredAt") || !value.success) {
    Serial.println("cloud: record is missing measuredAt");
    return false;
  }
  document.set("fields/measuredAt/timestampValue", value.stringValue);

  if (!Firebase.Firestore.createDocument(&firebaseData, FIREBASE_PROJECT_ID, "",
                                         "sensor_readings", document.raw())) {
    Serial.printf("cloud: upload failed: %s\n", firebaseData.errorReason().c_str());
    return false;
  }
  Serial.printf("cloud: uploaded %s\n", jsonText);
  return true;
}
}

void cloudInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  littleFsReady = LittleFS.begin(false);
  if (!littleFsReady) {
    Serial.println("cloud: LittleFS is corrupted; formatting backup partition");
    if (LittleFS.format()) {
      littleFsReady = LittleFS.begin(false);
    }
    if (!littleFsReady) {
      Serial.println("cloud: LittleFS mount failed after format");
    }
  } else {
    backupPending = backupFileExists();
  }

  firebaseConfig.api_key = FIREBASE_API_KEY;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  Serial.println("cloud: Wi-Fi connection started");
}

bool cloudUpload(const FormattedRecord& record) {
  return uploadJson(record.json);
}

bool backupRecord(const FormattedRecord& record) {
  if (!littleFsReady) {
    Serial.println("cloud: backup unavailable because LittleFS is not mounted");
    return false;
  }

  // Keep only the newest unsent record. This prevents a long offline period
  // from producing a burst of stale records when connectivity returns.
  if (backupPending) {
    LittleFS.remove(BACKUP_LOG_PATH);
  }
  File file = LittleFS.open(BACKUP_LOG_PATH, FILE_WRITE);
  if (!file) {
    Serial.println("cloud: unable to open backup log");
    return false;
  }

  size_t written = file.println(record.json);
  file.close();
  if (written == 0) {
    Serial.println("cloud: unable to write backup log");
    return false;
  }
  backupPending = true;
  return true;
}

void flushBackup() {
  if (!littleFsReady || !firebaseReady || !backupPending ||
      WiFi.status() != WL_CONNECTED) {
    return;
  }

  File input = LittleFS.open(BACKUP_LOG_PATH, FILE_READ);
  if (!input) {
    Serial.println("cloud: unable to open backup log for retry");
    return;
  }

  String latestRecord;
  while (input.available()) {
    String line = input.readStringUntil('\n');
    line.trim();
    if (line.isEmpty()) {
      continue;
    }
    latestRecord = line;
  }
  input.close();

  if (latestRecord.isEmpty()) {
    LittleFS.remove(BACKUP_LOG_PATH);
    backupPending = false;
    return;
  }
  if (uploadJson(latestRecord.c_str())) {
    LittleFS.remove(BACKUP_LOG_PATH);
    backupPending = false;
    Serial.println("cloud: latest backup flushed");
  }
}

void cloudService() {
  ensureFirebaseAuth();
  if (millis() - lastBackupFlush >= BACKUP_FLUSH_INTERVAL_MS) {
    flushBackup();
    lastBackupFlush = millis();
  }
}