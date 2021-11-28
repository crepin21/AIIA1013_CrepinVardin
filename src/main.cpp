/*
  Titre      : Detecteur de vibration
  Auteur     : Code de Andre Roussel moidifier par Crepin Vardin Fouelefack
  Date       : 28/11/2021
  Description: 
  Version    : 0.0.1
*/



#include <Arduino.h>
#include "WIFIConnector_MKR1000.h"
#include "MQTTConnector.h"


/*
  Variables de base pour la lecture de la 
  sortie du TMP36 a partir d'une broche digitale
*/

const int ANALOG_PIN             = A1;   // Utilisation de la broche A1 pour lecture analogue
const int ANALOG_PIN_ROUGE       = A3;   // Utilisation de la broche A3 pour la led rouge
const int ANALOG_PIN_VERTE       = A5;   // Utilisation de la broche A1 pour la led verte
bool LedRouge                    = 0;    // Variable pour le statut de la led rouge
bool LedVerte                    = 0;    // Variable pour le statut de la led verte
const int MS_DELAI               = 2000; 
int AnalogValue                  = 0;    // Variable pour contenir la valeur lue de la broche analogue
float PinVoltage                 = 0.0;


void setup() {
  
  Serial.begin(9600);             // Activation du port série pour affichage dans le moniteur série

  wifiConnect();                  //Branchement au réseau WIFI
  MQTTConnect();                  //Branchement au broker MQTT

  pinMode(ANALOG_PIN, INPUT);        // Pour une bonne lecture, la broche analogique doit être placé en mode entré explicitment
  pinMode(ANALOG_PIN_ROUGE, OUTPUT); // Pour controler la sortie a la broche A3
  pinMode(ANALOG_PIN_VERTE, OUTPUT);  // Pour controler la sortie a la broche A5
}


// Boucle infinie qui permet au uC de faire une ou plusieurs taches sans arrêt

void loop() {

  // put your main code here, to run repeatedly:
  AnalogValue = analogRead(ANALOG_PIN);                  // Lecture de la broche analogue et sauvegarde de valeur
  
  // Allumage du systeme (led verte)
  digitalWrite(ANALOG_PIN_VERTE, HIGH);
  // La led rouge est eteinte au debut
  digitalWrite(ANALOG_PIN_ROUGE, LOW);
  LedVerte  = 1; 
  LedRouge  = 0;

  // Impression dans le moniteur série
  Serial.print("La valeur obtene par la broche analogue est ");
  Serial.println(AnalogValue);

  // Le systeme s'arrete (led verte s'eteint si la valeur analogue est superieure a 200 puis se relance)
  if (AnalogValue >= 200)
  {
    digitalWrite(ANALOG_PIN_ROUGE, HIGH);
    LedRouge  = 1;
    digitalWrite(ANALOG_PIN_VERTE, LOW);   
    LedVerte   = 0;   
  }

  appendPayload("ValeurBroche", AnalogValue);  //Ajout de la valeur de la broche A1 au message MQTT
  appendPayload("LedRouge", LedRouge);  //Ajout de la valeur de la broche A3 au message MQTT
  appendPayload("LedVerte", LedVerte);  //Ajout de la valeur de la broche A5 au message MQTT

  sendPayload();                                   //Envoie du message via le protocole MQTT

  delay(MS_DELAI);    // Délai de sorte a ce qu'on puisse lire les valeurs et ralentir le uC
                      // Note: l'utilisation d'un délai est généralement une mauvaise pratique mais utilisable dans le cas de ce démo
}
