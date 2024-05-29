#include "Zanshin_BME680.h"

// Création d'une instance de la classe BME680
BME680_Class BME680;

const uint32_t SERIAL_SPEED{115200};

// Déclaration de la fonction altitude
float altitude(const int32_t press);

void setup() {
  // Initialisation de la communication série
  Serial.begin(SERIAL_SPEED);

#ifdef __AVR_ATmega32U4__
  // Attente de 3 secondes pour les processeurs 32U4
  delay(3000);
#endif

  // Messages d'initialisation
  Serial.println(F("Starting I2CDemo example program for BME680"));
  Serial.println(F("- Initializing BME680 sensor"));

  // Initialisation du capteur BME680 en utilisant le mode I2C et le premier périphérique trouvé
  while (!BME680.begin(I2C_STANDARD_MODE)) {
    Serial.println(F("- Unable to find BME680. Trying again in 5 seconds."));
    delay(5000);
  }

  // Configuration des paramètres du capteur
  Serial.println(F("- Setting 16x oversampling for all sensors"));
  BME680.setOversampling(TemperatureSensor, Oversample16);
  BME680.setOversampling(HumiditySensor, Oversample16);
  BME680.setOversampling(PressureSensor, Oversample16);

  Serial.println(F("- Setting IIR filter to a value of 4 samples"));
  BME680.setIIRFilter(IIR4);

  Serial.println(F("- Setting gas measurement to 320°C for 150ms"));
  BME680.setGas(320, 150);
  Serial.println(F("CLEARDATA")); // on efface les données déja présente si jamais
  Serial.println(F("LABEL, Temps, Température, HR(%)")); // titre des colonnes dans Excel

}

void loop() {
  // Déclaration de variables pour les lectures du capteur
  static int32_t temp, humidity, pressure, gas;
  static float alt;
  static uint16_t loopCounter = 0;

  // Affichage de l'en-tête toutes les 25 boucles
  if (loopCounter % 25 == 0) {
    Serial.println("\nLoop Temp(C) Humid% Press(hPa) Alt(m) Air(mOhms)");
    Serial.println("==== ====== ======= ========= ======= ==========");
  }

  // Lecture des données du capteur BME680
  BME680.getSensorData(temp, humidity, pressure, gas);

  // Ignorer la première lecture (peut être incorrecte)
  if (loopCounter++ != 0) {
    // Affichage des données formatées
    Serial.print(loopCounter - 1);
    Serial.print(" ");

    // Formatage de la température
    float tempFloat = static_cast<float>(temp) / 100.0;
    Serial.print(tempFloat, 2);
    Serial.print(" ");

    // Formatage de l'humidité
    float humidityFloat = static_cast<float>(humidity) / 1000.0;
    Serial.print(humidityFloat, 3);
    Serial.print(" ");

    // Formatage de la pression
    float pressureFloat = static_cast<float>(pressure) / 100.0;
    Serial.print(pressureFloat, 2);
    Serial.print(" ");

    // Calcul et formatage de l'altitude
    alt = altitude(pressure);
    Serial.print(alt, 2);
    Serial.print(" ");

    // Formatage de la résistance du gaz
    float gasFloat = static_cast<float>(gas) / 100.0;
    Serial.print(gasFloat, 2);

    Serial.println();
    delay(10000);  // Attendre 10 secondes
  
  }

  Serial.print(("DATA, TIME, "));
  float tempFloat = static_cast<float>(temp) / 100.0;
  Serial.print(tempFloat, 2);
  Serial.print(F(","));
  float humidityFloat = static_cast<float>(humidity) / 1000.0;
  Serial.println(humidityFloat, 2);
  delay(1000);
}

// Définition de la fonction altitude
float altitude(const int32_t press) {
  static float Altitude;
  // Calcul de l'altitude en fonction de la pression
  Altitude = 44330.0 * (1.0 - pow(static_cast<float>(press) / 100.0 / 1013.25, 0.1903));
  return Altitude;
}

