/*

 Project Name Reaction Lights Training Module Project.
 Developed by : projectswithalex.
 Email :projectswithalex@gmail.com
 Github : https://github.com/projectswithalex/Reaction-Lights-Training-Module
 Social Media and stuff : https://linktr.ee/projectswithalex
 

Disclaimer : Code is opensource and can be modified by everyone. If you can improve the code and add new functionality to it i would be happy to get
              in touch with you and update the repository. 
              Also for everything HW related you have a full responsability for your build. I am not a hardware developer and every decision that i have made
              when i build this project was my own understanding over the modules that i have used. Always double check and if you have any questions let me know.
              
              
 Thanks for being here and wish you a good training session with this modules.
*/


#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <VL6180X_WE.h>
/* #include <VL6180X.h> */


/******************************** TRAINING MODE SELECTION ******************************/
#define MasterECU
#define SELBUTTON 14
enum tipeOfTraining_en {
  TRAINING_START = 0,
  TRAINING_SIMPLE = 1,       //GREEN
  TRAINING_ALLONALLOFF,      //BLUE
  TRAINING_RETURNTOMASTER,   //YELLOW
  TRAINING_PARTNERMODE,      //AQUA
  TRAINING_TIMERMODE = 5,    //MAGENTA
  TRAINING_COUNTERMODE = 6,  //WHITE
  TRAINING_END = 6
};
bool training_startTraining_Flag = false;
bool training_allSettingsSent = false;
uint8_t training_allSettingsSentCounter = 0;

enum TRAINING_SIMPLE_en {
  TRAINING_SIMPLE_SelectNrOfECUs,
  TRAINING_SIMPLE_NrOfColorSelection,
  TRAINING_SIMPLE_counterSetSelection
};
TRAINING_SIMPLE_en TRAINING_SIMPLE_selection;
void trainingSimpleMain(void);


enum TRAINING_ALLONALLOFF_en {
  TRAINING_ALLONALLOFF_SelectNrOfECUs,
  TRAINING_ALLONALLOFF_NrOfColorSelection,
  TRAINING_ALLONALLOFF_counterSetSelection,
  TRAINING_ALLONALLOFF_stopTimeDuration,
};
TRAINING_ALLONALLOFF_en TRAINING_ALLONALLOFF_selection;
void trainingAllOnAllOffMain(void);

uint8_t TRAINING_ALLONALLOFF_sendData = 0;
uint8_t trainingAllOnAllOfActiveColorIndex = 0;  //red

enum TRAINING_RETURNTOMASTER_en {
  TRAINING_RETURNTOMASTER_SelectNrOfECUs,
  TRAINING_RETURNTOMASTER_NrOfColorSelection,
  TRAINING_RETURNTOMASTER_counterSetSelection,
  TRAINING_RETURNTOMASTER_stopTimeDuration,
};
TRAINING_RETURNTOMASTER_en TRAINING_RETURNTOMASTER_selection;
void trainingReturnToMasterMain(void);

enum TRAINING_PARTNERMODE_en {
  TRAINING_PARTNERMODE_SelectNrOfECUs,
  TRAINING_PARTNERMODE_SelectColor_Player1,
  TRAINING_PARTNERMODE_SelectColor_Player2,
  TRAINING_PARTNERMODE_counterSetSelection,
};
TRAINING_PARTNERMODE_en TRAINING_PARTNERMODE_selection;
void trainingPartnerModeMain(void);
void TRAINING_PARTNERMODE_player1Loop(void);
void TRAINING_PARTNERMODE_player2Loop(void);
void TRAINING_PARTNERMODE_SendToActiveP1(void);
void TRAINING_PARTNERMODE_SendToActiveP2(void);


enum TRAINING_TIMERMODE_en {
  TRAINING_TIMERMODE_SelectNrOfECUs,
  TRAINING_TIMERMODE_NrOfColorSelection,
  TRAINING_TIMERMODE_counterSetSelection,
  TRAINING_TIMERMODE_maxTimeInterval,
  TRAINING_TIMERMODE_minTimeInterval
};
TRAINING_TIMERMODE_en TRAINING_TIMERMODE_selection;
void trainingTimerModeMain(void);


enum TRAINING_COUNTERMODE_en {
  TRAINING_COUNTERMODE_counterSetSelection,
};
TRAINING_COUNTERMODE_en TRAINING_COUNTERMODE_selection;
void trainingCounterModeMain(void);
void setupTrainingCounterMode(void);
/* VL6180X sensor; */
uint8_t TRAINING_COUNTERMODE_pushUpCounter = 0;
bool TRAINING_COUNTERMODE_flagUp = false;
bool TRAINING_COUNTERMODE_flagDown = false;
uint8_t TRAINING_COUNTERMODE_restartCounterTraining = 0;


bool training_tipeOfTrainingFlag = false;
tipeOfTraining_en training_trainingType;

bool training_nrOfColorsFlag = false;
uint8_t training_nrOfColors = 0;

bool training_counterValStopFlag = false;
uint8_t training_counterValStop = 0;

bool training_stopTimeDurationFlag = false;
uint16_t training_stopTimeDuration = 0;

bool training_partnerMode_P1ColorFlag = false;
uint8_t training_partnerMode_P1Color = 0;

bool training_partnerMode_P2ColorFlag = false;
uint8_t training_partnerMode_P2Color = 0;

bool training_timerMode_minIntervalTimeFlag = false;
uint8_t training_timerMode_minIntervalTime = 0;

bool training_timerMode_maxIntervalTimeFlag = false;
uint8_t training_timerMode_maxIntervalTime = 0;

bool training_changeDistanceFlag = false;
uint8_t training_changeDistance = 0;

bool traininig_partnerMode_stopTimeIntervalFlag = false;
uint8_t traininig_partnerMode_stopTimeInterval = 0;

bool training_counterMode_setRepsFlag = false;
uint8_t training_counterMode_setReps = 0;

bool training_SelectNrOfECUsFlag = false;
uint8_t training_SelectNrOfECUs = 0;

unsigned long debounceDelay = 200;
unsigned long longDebounce = 3000;
volatile unsigned long lastDebounce = 0;
volatile uint8_t interruptModeSelection = 0;
volatile bool isbuttonpressed = 0;

uint8_t startNewTraining = 0;

volatile bool timerIntreruptFlag;

ICACHE_RAM_ATTR void onTimerInt() {
  timerIntreruptFlag = true;
}

ICACHE_RAM_ATTR void handleInterruptSEL() {
  lastDebounce = millis();
  isbuttonpressed = true;
}

bool buttonPushValid(void) {
  //unsigned long start = millis();
  if (isbuttonpressed) {
    if (millis() - lastDebounce > debounceDelay) {
      interruptModeSelection++;
      isbuttonpressed = false;
      lastDebounce = millis();
      return true;
    }
  }
  return false;
}


bool buttonLongPush(void) {
  if (isbuttonpressed) {
    Serial.print("pressed:");
    Serial.println(lastDebounce);

    if (millis() - lastDebounce > longDebounce) {
      isbuttonpressed = false;
      lastDebounce = millis();
      return true;
    }
  }
  return false;
}


bool TRAINING_SelectNrOfECUsFunction(void);
bool TRAINING_counterSetSelectionFunction(void);
bool TRAINING_nrOfColorsFunction(void);
bool TRAINING_stopTimeDurationFunction(void);
bool TRAINING_selectColor_P1(void);
bool TRAINING_selectColor_P2(void);
bool TRAINING_CounterModeSetReps(void);
bool TRAINING_setMaxIntervalTime(void);
bool TRAINING_setMinIntervalTime(void);
bool TRAINING_changeDistanceFunction(void);

uint16_t TRAINING_stopTimeDurationFunctionHelper(void) {
  return interruptModeSelection * 200;
}

uint8_t TRAINING_counterSetSelectionFunctionHelper(void) {
  return interruptModeSelection * 5;
}

uint8_t TRAINING_changeDistanceFunctionHelper(void) {
  return interruptModeSelection * 20;
}


uint8_t counterExercise = 0;
/******************************** TRAINING MODE SELECTION ******************************/

/********************************ESP NOW COMMUNICATION CODE ******************************/
#define MY_ROLE ESP_NOW_ROLE_COMBO        // set the role of this device: CONTROLLER, SLAVE, COMBO
#define RECEIVER_ROLE ESP_NOW_ROLE_COMBO  // set the role of the receiver
/*replaceValueHere*/ #define MY_ECU 1     //ECU number
#define WIFI_CHANNEL 1
#define MACADDRESSSIZE 6                       //Mac address size
#define NO_ECU 0                               //No ecu with the define MY_ECU 0
#define RGBCLEARDELAY 100                      //delay to be used with RGB clear ?TBD
  /*replaceValueHere*/ #define AVAILABLEECU 3  //Nr of ECUs to be used
#define MAXAVAILABLEECU 10                     // I think ESPNOW supports up to 10 devices

  //Receivers ECUS addreses.Add all of them here.

  // /*replaceValueHere*/ uint8_t receiverAddress1[] = { 0x40, 0xF5, 0x20, 0x33, 0xDA, 0xF9 };    //  this ECU MAC address ,only for example purposes
  /*replaceValueHere*/ uint8_t receiverAddress2[] = { 0xEC, 0xFA, 0xBC, 0x01, 0xA0, 0xA8 };  //  ECU 2
/*replaceValueHere*/ uint8_t receiverAddress3[] = { 0x3C, 0x61, 0x05, 0xED, 0x2A, 0xB6 };    //  ECU 3
/*replaceValueHere*/ uint8_t receiverAddress4[] = { 0xF4, 0xCF, 0xA2, 0x79, 0x23, 0x84 };    //  ECU 4
// /*replaceValueHere*/ uint8_t receiverAddress4[] = { 0x4C, 0xEB, 0xD6, 0x62, 0x09, 0x54 };    //  ECU 5

uint8_t receiverECU_Address[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  //Placeholder for the receiver address

uint8_t receiverArray[MAXAVAILABLEECU][MACADDRESSSIZE];

struct __attribute__((packed)) dataPacketAlone {
  uint8_t LED_Token;  // Token for activating ECUs
  uint8_t counterExerciseData;
};

struct __attribute__((packed)) dataPacketPartner {
  uint8_t LED_Token_Partner;
  uint8_t activeECU;
  uint8_t counterExercisePartner;
};

struct __attribute__((packed)) dataPacketSettings {
  uint8_t training_NrOfEcus;
  uint8_t training_trainingType;
  uint8_t training_nrOfColors;
  uint8_t training_counterValStop;
  uint16_t training_stopTimeDuration;
  uint8_t training_partnerMode_P1Color;
  uint8_t training_partnerMode_P2Color;
  uint32_t training_maxIntervalTime;
  uint32_t training_minIntervalTime;
  uint8_t winnerPartner;
};


//state in which the ECU can be found
enum transmissionState_en {
  DATARECEIVED_en,
  SENDDATA_en,
  SENDINGDATA_en,
  TRANSMISIONSUCCESFULL_en,
  ONLYRECEIVE_en
};

/*replaceValueHere*/ dataPacketAlone packetAlone = { 1, 0 };  //Package of data to be sent !if not ECU1 set to 0!
transmissionState_en TransmisionStatus = DATARECEIVED_en;     //Transmision Status


dataPacketSettings packetSettings;

dataPacketPartner partnerP1 = { 1, 3, 0 };
dataPacketPartner partnerP2 = { 2, 2, 0 };
uint8_t P1TOFtrigger = 0;
uint8_t P2TOFtrigger = 0;

void initReceiverAddress(void) {


  // memcpy(&receiverArray[0], NOECU, 6); //no ECU is allowed to be on 0 position
  // memcpy(&receiverArray[1], receiverAddress1, 6);  //This is my ECU position doesn't need to be filed.
  switch (training_SelectNrOfECUs) {
    case 1:
      memcpy(&receiverArray[2], receiverAddress2, 6);
      esp_now_add_peer(receiverAddress2, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      break;

    case 2:
      memcpy(&receiverArray[2], receiverAddress2, 6);
      memcpy(&receiverArray[3], receiverAddress3, 6);
      esp_now_add_peer(receiverAddress2, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      esp_now_add_peer(receiverAddress3, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      break;

    case 3:
      memcpy(&receiverArray[2], receiverAddress2, 6);
      memcpy(&receiverArray[3], receiverAddress3, 6);
      memcpy(&receiverArray[4], receiverAddress4, 6);
      esp_now_add_peer(receiverAddress2, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      esp_now_add_peer(receiverAddress3, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      esp_now_add_peer(receiverAddress4, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      break;

    case 4:
      memcpy(&receiverArray[2], receiverAddress2, 6);
      memcpy(&receiverArray[3], receiverAddress3, 6);
      memcpy(&receiverArray[4], receiverAddress4, 6);
      //to add
      esp_now_add_peer(receiverAddress2, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      esp_now_add_peer(receiverAddress3, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      esp_now_add_peer(receiverAddress4, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);
      //to add
      break;
  }
  //.......
  //and so on until MAXAVAILABLEECU
}


void initESPNOWcomm(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // we do not want to connect to a WiFi network

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  esp_now_set_self_role(MY_ROLE);
  esp_now_register_send_cb(transmissionComplete);  // this function will get called once all data is sent
  esp_now_register_recv_cb(dataReceived);          // this function will get called whenever we receive data

  // initReceiverAddress();
}
/********************************ESP NOW COMMUNICATION CODE ******************************/

/******************************** RGB LED CODE  ******************************/
//RGB Defines
#define RGBDATAPIN 2
#define RGBLEDNUM 16
#define NUMBEROFCOLORS 7
Adafruit_NeoPixel pixels(RGBLEDNUM, RGBDATAPIN, NEO_GRB + NEO_KHZ800);

//                                            RED   Green  Blue
const uint8_t colors[NUMBEROFCOLORS][3] = { { 150, 0, 0 },        //RED
                                            { 0, 150, 0 },        //GREEN
                                            { 0, 0, 150 },        //BLUE
                                            { 150, 150, 0 },      //YELLOW
                                            { 0, 150, 150 },      //AQUA
                                            { 150, 0, 150 },      //MAGENTA
                                            { 150, 150, 150 } };  //WHITE
uint8_t selectColor = 0;

//Select color next ON cycle
void selectColorNextCycle(void) {
  selectColor++;
  if (selectColor > NUMBEROFCOLORS - 1) {
    selectColor = 0;
  }
}

uint8_t generateRandomColor(void) {
  randomSeed(millis());
  uint8_t flagLocal = 0;
  uint8_t returnValue = 0;
  uint8_t randomColor = 0;
  while (flagLocal == 0) {
    randomColor = random(training_nrOfColors);  //function is exclusive

    if (training_trainingType == TRAINING_ALLONALLOFF) {
      if (randomColor != trainingAllOnAllOfActiveColorIndex) {  //BLUE is the active LIGHT for ALLONALLOFF
        returnValue = randomColor;
        flagLocal = 1;
      }
    } else {
      returnValue = randomColor;
      flagLocal = 1;
    }
  }
  return returnValue;
}

void setRGBcolors(uint8_t colorIndex) {

  for (int i = 0; i < RGBLEDNUM; i++) {  // For each pixel...
    pixels.setPixelColor(i, pixels.Color(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]));
    pixels.show();  // Send the updated pixel colors to the hardware.
  }
}

void setRGBColorsNumber(uint8_t number, uint8_t color) {
  for (int i = 0; i < number; i++) {  // For each pixel...
    pixels.setPixelColor(i, pixels.Color(colors[color][0], colors[color][1], colors[color][2]));
    pixels.show();  // Send the updated pixel colors to the hardware.
  }
}

void clearRGBcolors() {
  //Serial.println("Clear RGB");
  for (int i = 0; i < RGBLEDNUM; i++) {              // For each pixel...
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));  //clear the RGBs
    pixels.show();                                   // Send the updated pixel colors to the hardware.
  }
}

/******************************** RGB LED CODE  ******************************/


/******************************** BATTERY CHECK CODE  ******************************/
//Battery Check Defines
#define BATMEAS A0       // Analog input pin
int analogVal = 0;       // Analog Output of Sensor
int bat_percentage = 0;  // Battery in percentage

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
};

void readBatValue(void) {
  analogVal = analogRead(BATMEAS);
  /*replaceValueHere*/ float voltage = (((analogVal * 3.3) / 1024) * 1.54);  //1.54 is the constant for me , check out with the multimeter and set the right value for you (trial&error) until correct
  bat_percentage = mapfloat(voltage, 3, 4.2, 0, 100);                        //Real Value

  if (bat_percentage >= 100) {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0) {
    bat_percentage = 1;
  }

  Serial.print("battery perc");
  Serial.println(bat_percentage);
};

void showBatteryPercentage(void) {
  if ((bat_percentage <= 100) && (bat_percentage >= 90))  //Battery Levels between 100%-90% = 16LEDS green
  {
    for (int i = 0; i < RGBLEDNUM; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 0));
      pixels.show();
      delay(50);
    }
  }

  if ((bat_percentage < 90) && (bat_percentage >= 60))  //Battery Levels between 89%-60% = 12LEDS green
  {
    for (int i = 0; i < 12; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 0));
      pixels.show();
      delay(50);
    }
  }

  if ((bat_percentage < 60) && (bat_percentage >= 40))  //Battery Levels between 59%-40% = 8LEDS green
  {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 0));
      pixels.show();
      delay(50);
    }
  }

  if ((bat_percentage < 40) && (bat_percentage >= 20))  //Battery Levels between 39%-20% = 4LEDS green
  {
    for (int i = 0; i < 4; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 0));
      pixels.show();
      delay(50);
    }
  }

  if ((bat_percentage < 20) && (bat_percentage >= 5))  //Battery Levels between 19%-10% = 4LEDS flashing RED leds
  {

    for (int i = 0; i < RGBLEDNUM; i++) {
      pixels.setPixelColor(i, pixels.Color(20, 0, 0));
      pixels.show();
      i = i + 3;
    }
    delay(200);
    pixels.clear();
    pixels.show();
    delay(200);
    for (int i = 0; i < RGBLEDNUM; i++) {
      pixels.setPixelColor(i, pixels.Color(20, 0, 0));
      pixels.show();
      i = i + 3;
    }
  }
  // Lower then 10 percent nothing is displayed. As a safety measure since i don't know how much trust i have in the "BMS" that is going to
  // protect the battery of over-discharge.
};

void initBatteryCheck(void) {
  readBatValue();
  showBatteryPercentage();
}
/******************************** BATTERY CHECK CODE  ******************************/

/******************************** TOF SENSOR CODE  ******************************/
#define VL6180X_ADDRESS 0x29
#define TOF_INT 12
#define SCL_PIN 5
#define SDA_PIN 4

VL6180xIdentification identification;
VL6180x TOFsensor(VL6180X_ADDRESS);

volatile bool intrerruptTOF = false;
volatile int resTOFflag = 0;

ICACHE_RAM_ATTR void handleInterruptTOF() {

  intrerruptTOF = true;
  if ((counterExercise >= training_counterValStop && training_startTraining_Flag == true) || (packetSettings.winnerPartner != 0)) {
    resTOFflag++;
  }
}


void initTOFSensor(void) {
  pinMode(TOF_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOF_INT), handleInterruptTOF, FALLING);

  delay(100);  //do i really need this here
  while (TOFsensor.VL6180xInit() == VL6180x_FAILURE_RESET) {
    Serial.println("FAILED TO INITALIZE");  //Initialize device and check for errors
    ESP.restart();
  }
  delay(500);
  TOFsensor.VL6180xDefautSettings();                         //Load default settings to get started.
  delay(500);                                                //do i really need this here
  /*replaceValueHere*/ TOFsensor.VL6180xSetDistInt(50, 255);  //it detects a movement when it lower than 2cm. With the current initialization should work for values up until 20cm .
  TOFsensor.getDistanceContinously();
  TOFsensor.VL6180xClearInterrupt();
  intrerruptTOF = false;
  //delay(500);  //do i really need this here
}
/******************************** TOF SENSOR CODE  ******************************/

uint8_t dataP1received = 0;
uint8_t dataP2received = 0;

uint8_t randomECUSelectionPartnerP1 = 0;
uint8_t randomECUSelectionPartnerP2 = 0;

uint8_t randomECUselectPartner(void) {
  randomSeed(millis());
  uint8_t returnValue = 0;
  uint8_t randomNumber = 0;
  while (returnValue == 0) {
    randomNumber = random(0, training_SelectNrOfECUs + 2);  //we have +2 because 1 is master and the function is exclusive
    if ((randomNumber != partnerP1.activeECU) && (randomNumber != partnerP2.activeECU) && (randomNumber != NO_ECU)) {
      returnValue = randomNumber;
    }
  }
  Serial.print("P1Active:");
  Serial.println(partnerP1.activeECU);
  Serial.print("P2Active:");
  Serial.println(partnerP2.activeECU);
  return returnValue;
}


//Callback function after a transmission has been sent
void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if (transmissionStatus == 0) {

    TRAINING_ALLONALLOFF_sendData++;
    TransmisionStatus = TRANSMISIONSUCCESFULL_en;
    if (training_allSettingsSent == false) {
      training_allSettingsSentCounter++;
    }
  } else {
    TransmisionStatus = SENDDATA_en;
    //  Serial.print("Error code: ");
    //  Serial.println(transmissionStatus);
  }
}
//Callback function after a transmission has been received
void dataReceived(uint8_t *senderMac, uint8_t *data, uint8_t dataLength) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);

  Serial.println();
  Serial.print("Received data from: ");
  Serial.println(macStr);

  switch (dataLength) {
    case 2:
      memcpy(&packetAlone, data, sizeof(packetAlone));
      if (packetSettings.training_trainingType == TRAINING_TIMERMODE && packetAlone.LED_Token == MY_ECU) {
        timer1_write(randomTimerInterval());
      }
      break;

    case 3:

      if (data[0] == 1) {
        memcpy(&partnerP1, data, sizeof(partnerP1));
        randomECUSelectionPartnerP1 = randomECUselectPartner();
        partnerP1.activeECU = randomECUSelectionPartnerP1;
        Serial.print("randomP1 ");
        Serial.println(randomECUSelectionPartnerP1);
        dataP1received = 1;
      } else {
        if (data[0] == 2) {
          memcpy(&partnerP2, data, sizeof(partnerP2));
          randomECUSelectionPartnerP2 = randomECUselectPartner();
          partnerP2.activeECU = randomECUSelectionPartnerP2;
          Serial.print("randomP2 ");
          Serial.println(randomECUSelectionPartnerP2);
          dataP2received = 1;
        }
      }
      Serial.print("trigerP1 :");
      Serial.println(P1TOFtrigger);
      Serial.print("trigerP2 :");
      Serial.println(P2TOFtrigger);
      break;
    case 9:
      memcpy(&packetSettings, data, sizeof(packetSettings));
      break;
  }
  TransmisionStatus = DATARECEIVED_en;
}
/******************************** Logic CODE  ******************************/

uint8_t randomECUSelection = 0;

uint32_t randomTimerIntervalHelper(uint8_t value) {
  uint32_t retVal = (uint32_t)value;
  if (retVal > 2) {
    retVal = retVal * 156250;  // 0.5s and 1sec
    return retVal;
  } else {
    retVal = ((retVal - 1) * 312500);  // 2s, 3s, 4s, 5s
    return retVal;
  }
}

uint32_t randomTimerInterval(void) {
  randomSeed(millis());
  uint8_t randomNumber = 0;
  uint32_t retVal = 0;
  if (training_timerMode_minIntervalTime != training_timerMode_maxIntervalTime) {
    randomNumber = random(training_timerMode_minIntervalTime, training_timerMode_maxIntervalTime);
  } else {
    randomNumber = training_timerMode_minIntervalTime;
  }
  retVal = randomTimerIntervalHelper(randomNumber);
  Serial.print("Timer 1 randomNumber :");
  Serial.println(retVal);
  return retVal;
}

uint8_t randomECUselect(void) {
  randomSeed(millis());
  uint8_t returnValue = 0;
  uint8_t randomNumber = 0;
  while (returnValue == 0) {
    randomNumber = random(0, training_SelectNrOfECUs + 2);  //we have +2 because 1 is master and the function is exclusive
    if ((randomNumber != MY_ECU) && (randomNumber != NO_ECU)) {
      returnValue = randomNumber;
    }
  }
  return returnValue;
}


void selectECU_number(uint8_t ECU) {
  memcpy(&receiverECU_Address, receiverArray[ECU], MACADDRESSSIZE);
  packetAlone.LED_Token = ECU;
  TransmisionStatus = SENDDATA_en;
}

void endOfTrainingLightPartner(void) {

  if (packetSettings.winnerPartner == 1) {
    setRGBcolors(packetSettings.training_partnerMode_P1Color);
    delay(500);
    pixels.clear();
    pixels.show();
    delay(500);
  } else {
    setRGBcolors(packetSettings.training_partnerMode_P2Color);
    delay(500);
    pixels.clear();
    pixels.show();
    delay(500);
  }
  if (buttonLongPush()) {
    if (intrerruptTOF) {
      Serial.print("StartNewTraining");
      startOfNewTraining();
    } else {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
  }
  if (intrerruptTOF) {
    delay(300);
    intrerruptTOF = false;
    TOFsensor.VL6180xClearInterrupt();
    Serial.print("Flag endOfTrainingLightPartner:");
    Serial.println(resTOFflag);
    if (resTOFflag > 5) {
      restartTrainingMode();
    }
  }
}
void endOfTrainingLight(void) {
  setRGBcolors(0);
  delay(500);
  pixels.clear();
  pixels.show();
  delay(500);

  if (intrerruptTOF) {
    delay(300);
    intrerruptTOF = false;
    TOFsensor.VL6180xClearInterrupt();
    Serial.print("Flag endOfTrainingLight:");
    Serial.println(resTOFflag);
    if (resTOFflag > 5) {
      restartTrainingModeSingle();
    }
  }
}

void startOfNewTraining(void) {
  ESP.restart();
}

void restartTrainingModeSingle(void) {
  unsigned long start = millis();
  while (millis() - start < 3000) {

    Serial.print("Broadcast");
    packetAlone.LED_Token = MY_ECU;
    packetAlone.counterExerciseData = 0;
    esp_now_send(NULL, (uint8_t *)&packetAlone, sizeof(packetAlone));

    pixels.clear();
    pixels.show();

    for (int i = 0; i < RGBLEDNUM; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 50, 0));
      pixels.show();
      delay(50);
    }
    intrerruptTOF = false;
    TOFsensor.VL6180xClearInterrupt();

    TransmisionStatus = DATARECEIVED_en;
    packetAlone.LED_Token = MY_ECU;
    counterExercise = 0;
    resTOFflag = 0;
  }
  if (training_trainingType == TRAINING_TIMERMODE) {
    timer1_write(randomTimerInterval());
  }
}

void restartTrainingMode(void) {
  training_allSettingsSent = false;
  resTOFflag = 0;
  dataP2received = 0;
  dataP1received = 0;
  partnerP1.counterExercisePartner = 0;
  partnerP2.counterExercisePartner = 0;
  partnerP1 = { 1, 3, 0 };
  partnerP2 = { 2, 2, 0 };
  packetSettings.winnerPartner = 0;
  counterExercise = 0;
  clearRGBcolors();
  training_trainingType = TRAINING_PARTNERMODE;
  TransmisionStatus = DATARECEIVED_en;
  sendSettingsData();
}

void endOfTrainingCounter(void) {
  uint16_t distanceVal = 0;
  setRGBcolors(0);
  delay(500);
  pixels.clear();
  pixels.show();
  delay(500);
  distanceVal = TOFsensor.getDistance();
  if (distanceVal < 100) {
    TRAINING_COUNTERMODE_restartCounterTraining++;
  }
  if (TRAINING_COUNTERMODE_restartCounterTraining > 5) {
    TRAINING_COUNTERMODE_restartCounterTraining = 0;
    TRAINING_COUNTERMODE_pushUpCounter = 0;
    pixels.clear();
    pixels.show();
    delay(100);
    for (int i = 0; i < RGBLEDNUM; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 100, 0));
      pixels.show();
      delay(50);
    }
    delay(1000);
    pixels.clear();
    pixels.show();
  }
}

void startOptionSelection(void) {
  tipeOfTraining_en lModeSelect;
  uint8_t setCounter = 0;
  uint8_t setColor = 0;
  if (training_startTraining_Flag == false) {
    if (training_tipeOfTrainingFlag == false) {
      buttonPushValid();
      lModeSelect = (tipeOfTraining_en)interruptModeSelection;
      if (lModeSelect > TRAINING_END) {
        lModeSelect = TRAINING_SIMPLE;
        interruptModeSelection = 1;
      }
      setRGBcolors(lModeSelect);
      if (intrerruptTOF == true && lModeSelect != 0) {
        training_trainingType = lModeSelect;
        training_tipeOfTrainingFlag = true;
        interruptModeSelection = 0;
        intrerruptTOF = false;
        TOFsensor.VL6180xClearInterrupt();
        Serial.print("modeSelect:");
        Serial.println(lModeSelect);
        clearRGBcolors();
      }
    }

    switch (training_trainingType) {
      case TRAINING_SIMPLE:

        switch (TRAINING_SIMPLE_selection) {
          case TRAINING_SIMPLE_SelectNrOfECUs:
            if (TRAINING_SelectNrOfECUsFunction()) {
              TRAINING_SIMPLE_selection = TRAINING_SIMPLE_NrOfColorSelection;
            }
            break;

          case TRAINING_SIMPLE_NrOfColorSelection:
            if (TRAINING_nrOfColorsFunction()) {
              TRAINING_SIMPLE_selection = TRAINING_SIMPLE_counterSetSelection;
            }
            break;

          case TRAINING_SIMPLE_counterSetSelection:
            if (TRAINING_counterSetSelectionFunction()) {
              training_startTraining_Flag = true;
            }
            break;
        }
        break;  //TRAINING_SIMPLE

      case TRAINING_ALLONALLOFF:
        switch (TRAINING_ALLONALLOFF_selection) {
          case TRAINING_ALLONALLOFF_SelectNrOfECUs:
            if (TRAINING_SelectNrOfECUsFunction()) {
              TRAINING_ALLONALLOFF_selection = TRAINING_ALLONALLOFF_NrOfColorSelection;
            }
            break;

          case TRAINING_ALLONALLOFF_NrOfColorSelection:
            if (TRAINING_nrOfColorsFunction()) {
              TRAINING_ALLONALLOFF_selection = TRAINING_ALLONALLOFF_counterSetSelection;
            }
            break;

          case TRAINING_ALLONALLOFF_counterSetSelection:
            if (TRAINING_counterSetSelectionFunction()) {
              TRAINING_ALLONALLOFF_selection = TRAINING_ALLONALLOFF_stopTimeDuration;
            }
            break;

          case TRAINING_ALLONALLOFF_stopTimeDuration:
            if (TRAINING_stopTimeDurationFunction()) {
              training_startTraining_Flag = true;
            }
            break;
        }

        break;  //TRAINING_ALLONALLOFF

      case TRAINING_RETURNTOMASTER:
        switch (TRAINING_RETURNTOMASTER_selection) {
          case TRAINING_RETURNTOMASTER_SelectNrOfECUs:
            if (TRAINING_SelectNrOfECUsFunction()) {
              TRAINING_RETURNTOMASTER_selection = TRAINING_RETURNTOMASTER_NrOfColorSelection;
            }
            break;

          case TRAINING_RETURNTOMASTER_NrOfColorSelection:
            if (TRAINING_nrOfColorsFunction()) {
              TRAINING_RETURNTOMASTER_selection = TRAINING_RETURNTOMASTER_counterSetSelection;
            }
            break;

          case TRAINING_RETURNTOMASTER_counterSetSelection:
            if (TRAINING_counterSetSelectionFunction()) {
              TRAINING_RETURNTOMASTER_selection = TRAINING_RETURNTOMASTER_stopTimeDuration;
            }
            break;

          case TRAINING_RETURNTOMASTER_stopTimeDuration:
            if (TRAINING_stopTimeDurationFunction()) {
              training_startTraining_Flag = true;
            }
            break;
        }
        break;  //TRAINING_RETURNTOMASTER

      case TRAINING_PARTNERMODE:
        switch (TRAINING_PARTNERMODE_selection) {
          case TRAINING_PARTNERMODE_SelectNrOfECUs:
            if (TRAINING_SelectNrOfECUsFunction()) {
              TRAINING_PARTNERMODE_selection = TRAINING_PARTNERMODE_SelectColor_Player1;
            }
            break;

          case TRAINING_PARTNERMODE_SelectColor_Player1:
            if (TRAINING_selectColor_P1()) {
              TRAINING_PARTNERMODE_selection = TRAINING_PARTNERMODE_SelectColor_Player2;
            }
            break;

          case TRAINING_PARTNERMODE_SelectColor_Player2:
            if (TRAINING_selectColor_P2()) {
              TRAINING_PARTNERMODE_selection = TRAINING_PARTNERMODE_counterSetSelection;
            }
            break;

          case TRAINING_PARTNERMODE_counterSetSelection:
            if (TRAINING_counterSetSelectionFunction()) {
              training_startTraining_Flag = true;
            }
            break;
        }
        break;  //TRAINING_PARTNERMODE

      case TRAINING_TIMERMODE:
        switch (TRAINING_TIMERMODE_selection) {
          case TRAINING_TIMERMODE_SelectNrOfECUs:
            if (TRAINING_SelectNrOfECUsFunction()) {
              TRAINING_TIMERMODE_selection = TRAINING_TIMERMODE_NrOfColorSelection;
            }
            break;

          case TRAINING_TIMERMODE_NrOfColorSelection:
            if (TRAINING_nrOfColorsFunction()) {
              TRAINING_TIMERMODE_selection = TRAINING_TIMERMODE_counterSetSelection;
            }
            break;

          case TRAINING_TIMERMODE_counterSetSelection:
            if (TRAINING_counterSetSelectionFunction()) {
              TRAINING_TIMERMODE_selection = TRAINING_TIMERMODE_maxTimeInterval;
            }
            break;

          case TRAINING_TIMERMODE_maxTimeInterval:
            if (TRAINING_setMaxIntervalTime()) {
              TRAINING_TIMERMODE_selection = TRAINING_TIMERMODE_minTimeInterval;
            }
            break;

          case TRAINING_TIMERMODE_minTimeInterval:
            if (TRAINING_setMinIntervalTime()) {
              training_startTraining_Flag = true;
            }
            break;
        }
        break;  //TRAINING_TIMERMODE

      case TRAINING_COUNTERMODE:
        if (TRAINING_CounterModeSetReps()) {
          training_startTraining_Flag = true;
          setupTrainingCounterMode();
        }
        break;  //TRAINING_COUNTERMODE
    }
  }
}

void sendSettingsData(void) {
  if (TransmisionStatus != SENDINGDATA_en) {
    packetSettings.training_NrOfEcus = training_SelectNrOfECUs;
    packetSettings.training_counterValStop = training_counterValStop;
    packetSettings.training_stopTimeDuration = training_stopTimeDuration;
    packetSettings.training_nrOfColors = training_nrOfColors;
    packetSettings.training_partnerMode_P1Color = training_partnerMode_P1Color;
    packetSettings.training_partnerMode_P2Color = training_partnerMode_P2Color;
    packetSettings.training_trainingType = training_trainingType;
    packetSettings.training_maxIntervalTime = training_timerMode_maxIntervalTime;
    packetSettings.training_minIntervalTime = training_timerMode_minIntervalTime;
    initReceiverAddress();
    Serial.println("datawriten");
  }

  if ((TransmisionStatus == TRANSMISIONSUCCESFULL_en) || (TransmisionStatus == DATARECEIVED_en)) {
    Serial.println("datasending");
    esp_now_send(NULL, (uint8_t *)&packetSettings, sizeof(packetSettings));
    TransmisionStatus = SENDINGDATA_en;
  }
  if (training_allSettingsSentCounter > (training_SelectNrOfECUs)) {
    Serial.println("datasent");
    training_allSettingsSent = true;
    intrerruptTOF = false;
    TOFsensor.VL6180xClearInterrupt();
    showBatteryPercentage();
    delay(1000);
    intrerruptTOF = false;
    TOFsensor.VL6180xClearInterrupt();

    timer1_write(randomTimerInterval());
    TransmisionStatus = ONLYRECEIVE_en;
  }
}

/******************************** Logic CODE  ******************************/

void setup() {
  Serial.begin(115200);     // initialize serial port
                            // Wire.begin(SDA_PIN, SCL_PIN);  //Initialize I2C for VL6180x (TOF Sensor)
  pinMode(BATMEAS, INPUT);  //measure Battery Pin

  timer1_attachInterrupt(onTimerInt);  // Add ISR Function
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);

  pinMode(SELBUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(SELBUTTON), handleInterruptSEL, FALLING);

  Serial.println("pixels");
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors to 'off'
  pixels.show();
  Serial.println("initESPNOWcomm");
  initESPNOWcomm();

  Serial.println("initTOFSensor");
  initTOFSensor();
  Serial.println("initBatteryCheck");
  initBatteryCheck();
  Serial.println("almost Ready");
  delay(1000);
  //clearRGBcolors();
  Serial.println("Ready");
}

unsigned long timeFlag = 0;
unsigned long countTime = 0;
void loop() {
  if (training_startTraining_Flag == false) {
    startOptionSelection();
  } else {
    if ((training_allSettingsSent == false) && (training_trainingType != TRAINING_COUNTERMODE)) {
      sendSettingsData();
    } else {
      switch (training_trainingType) {
        case TRAINING_SIMPLE:
          trainingSimpleMain();
          break;

        case TRAINING_ALLONALLOFF:
          trainingAllOnAllOffMain();
          break;

        case TRAINING_RETURNTOMASTER:
          trainingReturnToMasterMain();
          break;

        case TRAINING_PARTNERMODE:
          if (packetSettings.winnerPartner == 0) {
            trainingPartnerModeMain();
          } else {
            training_trainingType = TRAINING_START;
            training_allSettingsSent = false;
          }
          break;

        case TRAINING_TIMERMODE:
          trainingTimerModeMain();
          break;

        case TRAINING_COUNTERMODE:
          trainingCounterModeMain();
          break;

        case TRAINING_START:
          if (training_allSettingsSent == false) {
            sendSettingsData();
          } else {
            endOfTrainingLightPartner();
          }
          break;
      }
    }
  }
}

void trainingSimpleMain(void) {
  if (counterExercise < training_counterValStop) {
    if (millis() - timeFlag > 500) {
      randomECUSelection = randomECUselect();
      timeFlag = millis();
    }

    if (packetAlone.LED_Token == MY_ECU) {
      setRGBcolors(selectColor);
      //Is the sensor active and the ECU is valid ?
      if (intrerruptTOF) {
        selectColor = generateRandomColor();
        Serial.println("Intrerupt received");
        intrerruptTOF = false;
        selectECU_number(randomECUSelection);
        //delay(RGBCLEARDELAY);  //why did i used this ???
        clearRGBcolors();
        TOFsensor.VL6180xClearInterrupt();
        counterExercise = packetAlone.counterExerciseData;
        counterExercise++;
        packetAlone.counterExerciseData = counterExercise;
        Serial.print("counter:");
        Serial.println(counterExercise);

      } else {
        //do nothing but wait
      }
    } else {
      if (TransmisionStatus == SENDDATA_en) {
        Serial.print("randomECUSelection ");
        Serial.println(randomECUSelection);

        esp_now_send(receiverECU_Address, (uint8_t *)&packetAlone, sizeof(packetAlone));
        TransmisionStatus = SENDINGDATA_en;
      } else {
        if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

          Serial.println("Transmision succesful");
          TransmisionStatus = ONLYRECEIVE_en;
          intrerruptTOF = false;
          TOFsensor.VL6180xClearInterrupt();
        } else {
          if (TransmisionStatus == ONLYRECEIVE_en) {
            intrerruptTOF = false;
            clearRGBcolors();
            TOFsensor.VL6180xClearInterrupt();
          }
        }
      }
    }

  } else {
    endOfTrainingLight();
  }
}

void trainingAllOnAllOffMain(void) {

  if (TransmisionStatus == DATARECEIVED_en || TRAINING_ALLONALLOFF_sendData >= (training_SelectNrOfECUs - 1)) {

    countTime = millis();
    while (millis() - countTime < training_stopTimeDuration) {
      clearRGBcolors();
    }
    TRAINING_ALLONALLOFF_sendData = 0;
    TransmisionStatus = ONLYRECEIVE_en;
    selectColor = generateRandomColor();
    setRGBcolors(selectColor);
    Serial.print("TRAINING_ALLONALLOFF_sendData:");
    Serial.println(TRAINING_ALLONALLOFF_sendData);
    Serial.print("TransmisionStatus:");
    Serial.println(TransmisionStatus);
  }

  if (counterExercise < training_counterValStop) {
    if (millis() - timeFlag > 500) {
      randomECUSelection = randomECUselect();

      timeFlag = millis();
    }

    if (packetAlone.LED_Token == MY_ECU) {
      setRGBcolors(trainingAllOnAllOfActiveColorIndex);
      //Is the sensor active and the ECU is valid ?
      if (intrerruptTOF) {
        Serial.println("Intrerupt received");
        intrerruptTOF = false;
        selectECU_number(randomECUSelection);
        //delay(RGBCLEARDELAY);  //why did i used this ???
        clearRGBcolors();
        TOFsensor.VL6180xClearInterrupt();
        counterExercise = packetAlone.counterExerciseData;
        counterExercise++;
        packetAlone.counterExerciseData = counterExercise;
      } else {
        //do nothing but wait
      }
    } else {

      if (TransmisionStatus == SENDDATA_en) {

        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
        Serial.print("sending to:");
        Serial.println(macStr);
        esp_now_send(NULL, (uint8_t *)&packetAlone, sizeof(packetAlone));
        TransmisionStatus = SENDINGDATA_en;

      } else {
        if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

          Serial.println("Transmision succesful");
          TransmisionStatus = ONLYRECEIVE_en;
          intrerruptTOF = false;
          TOFsensor.VL6180xClearInterrupt();
        } else {
          if (TransmisionStatus == ONLYRECEIVE_en) {
            // setRGBcolors(selectColor);
            intrerruptTOF = false;
            TOFsensor.VL6180xClearInterrupt();
          }
        }
      }
    }

  } else {

    endOfTrainingLight();
  }
}

void trainingReturnToMasterMain(void) {

  if (counterExercise < training_counterValStop) {
    if (millis() - timeFlag > 500) {
      randomECUSelection = randomECUselect();
      timeFlag = millis();
    }

    if (packetAlone.LED_Token == MY_ECU) {
      setRGBcolors(selectColor);
      //Is the sensor active and the ECU is valid ?
      if (intrerruptTOF) {
        selectColor = generateRandomColor();
        Serial.println("Intrerupt received");
        intrerruptTOF = false;
        selectECU_number(randomECUSelection);
        //delay(RGBCLEARDELAY);  //why did i used this ???
        clearRGBcolors();
        TOFsensor.VL6180xClearInterrupt();
        counterExercise = packetAlone.counterExerciseData;
        //counterExercise++;
        packetAlone.counterExerciseData = counterExercise;
      } else {
        //do nothing but wait
      }
    } else {
      if (TransmisionStatus == SENDDATA_en) {
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
        Serial.print("sending to:");
        Serial.println(macStr);
        esp_now_send(receiverECU_Address, (uint8_t *)&packetAlone, sizeof(packetAlone));
        TransmisionStatus = SENDINGDATA_en;
      } else {
        if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

          Serial.println("Transmision succesful");
          TransmisionStatus = ONLYRECEIVE_en;
          intrerruptTOF = false;
          TOFsensor.VL6180xClearInterrupt();
        } else {
          if (TransmisionStatus == ONLYRECEIVE_en) {
            intrerruptTOF = false;
            clearRGBcolors();
            TOFsensor.VL6180xClearInterrupt();
          }
        }
      }
    }

  } else {

    endOfTrainingLight();
  }
}

void trainingPartnerModeMain(void) {

  if (dataP1received == 1 || dataP2received == 1) {
    if (dataP1received == 1) {
      if (partnerP1.activeECU == 1) {
        TRAINING_PARTNERMODE_player1Loop();
      } else {
        if (partnerP1.activeECU == 0) {
          //do nothing
        } else {
          if (partnerP1.counterExercisePartner < packetSettings.training_counterValStop) {
            TRAINING_PARTNERMODE_SendToActiveP1();
          } else {
            packetSettings.winnerPartner = 1;
          }
        }
      }
    }

    if (dataP2received == 1) {
      if (partnerP2.activeECU == 1) {
        TRAINING_PARTNERMODE_player2Loop();
      } else {
        if (partnerP2.activeECU == 0) {
          //do nothing
        } else {
          if (partnerP2.counterExercisePartner < packetSettings.training_counterValStop) {
            TRAINING_PARTNERMODE_SendToActiveP2();
          } else {
            packetSettings.winnerPartner = 2;
          }
        }
      }
    }

  } else {
    TransmisionStatus = ONLYRECEIVE_en;
    intrerruptTOF = false;
    TOFsensor.VL6180xClearInterrupt();
    clearRGBcolors();
  }
}

void trainingTimerModeMain(void) {
  if (counterExercise < packetSettings.training_counterValStop) {
    if (millis() - timeFlag > 500) {
      randomECUSelection = randomECUselect();
      timeFlag = millis();
    }

    if (packetAlone.LED_Token == MY_ECU) {
      setRGBcolors(selectColor);
      //Is the sensor active and the ECU is valid ?
      if (timerIntreruptFlag) {
        selectColor = generateRandomColor();
        Serial.println("Intrerupt received");
        timerIntreruptFlag = false;
        intrerruptTOF = false;
        selectECU_number(randomECUSelection);
        //delay(RGBCLEARDELAY);  //why did i used this ???
        clearRGBcolors();
        TOFsensor.VL6180xClearInterrupt();
        counterExercise = packetAlone.counterExerciseData;
        counterExercise++;
        packetAlone.counterExerciseData = counterExercise;
        Serial.print("counter:");
        Serial.println(counterExercise);

      } else {
        //do nothing but wait
      }
    } else {
      if (TransmisionStatus == SENDDATA_en) {

        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
        // Serial.print("sending to:");
        // Serial.println(macStr);
        esp_now_send(receiverECU_Address, (uint8_t *)&packetAlone, sizeof(packetAlone));
        TransmisionStatus = SENDINGDATA_en;
      } else {
        if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

          Serial.println("Transmision succesful");
          TransmisionStatus = ONLYRECEIVE_en;
          intrerruptTOF = false;
          TOFsensor.VL6180xClearInterrupt();
        } else {
          if (TransmisionStatus == ONLYRECEIVE_en) {
            intrerruptTOF = false;
            clearRGBcolors();
            TOFsensor.VL6180xClearInterrupt();
          }
        }
      }
    }

  } else {

    endOfTrainingLight();
  }
}

void trainingCounterModeMain(void) {

  uint16_t distanceVal;
  if (TRAINING_COUNTERMODE_pushUpCounter < training_counterMode_setReps) {
    distanceVal = TOFsensor.getDistance();

    /* if (TOFsensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); } */

    delay(100);

    if ((distanceVal < 500) && (TRAINING_COUNTERMODE_flagDown == false)) {
      Serial.println(distanceVal);
      Serial.println("DOWN");

      TRAINING_COUNTERMODE_flagDown = true;
    }

    if ((TRAINING_COUNTERMODE_flagDown == true) && (TRAINING_COUNTERMODE_flagUp == false)) {
      Serial.println("Light one up");
      TRAINING_COUNTERMODE_pushUpCounter++;
      Serial.print("pushUpCounter:");
      Serial.println(TRAINING_COUNTERMODE_pushUpCounter);
      Serial.print("training_counterMode_setReps:");
      Serial.println(training_counterMode_setReps);

      TRAINING_COUNTERMODE_flagUp = true;
      for (int i = 0; i < TRAINING_COUNTERMODE_pushUpCounter; i++) {
        pixels.setPixelColor(i, pixels.Color(150, 150, 150));
        pixels.show();
      }
    }

    if (distanceVal > 500 && (TRAINING_COUNTERMODE_flagUp == true)) {
      Serial.println(distanceVal);
      Serial.println("UP");
      TRAINING_COUNTERMODE_flagUp = 0;
      TRAINING_COUNTERMODE_flagDown = 0;
    }
  } else {
    endOfTrainingCounter();
  }
}

void TRAINING_PARTNERMODE_player1Loop(void) {
  if (partnerP1.counterExercisePartner < packetSettings.training_counterValStop) {
    setRGBcolors(packetSettings.training_partnerMode_P1Color);
    //Is the sensor active and the ECU is valid ?
    if (intrerruptTOF) {
      selectColor = generateRandomColor();
      Serial.print("Intrerupt received p1");
      intrerruptTOF = false;
      randomECUSelectionPartnerP1 = randomECUselectPartner();
      selectECU_number(randomECUSelectionPartnerP1);
      Serial.println(randomECUSelectionPartnerP1);
      partnerP1.activeECU = randomECUSelectionPartnerP1;
      clearRGBcolors();
      TOFsensor.VL6180xClearInterrupt();
      partnerP1.counterExercisePartner++;
      P1TOFtrigger = 1;
    }
    if (TransmisionStatus == SENDDATA_en && P1TOFtrigger == 1) {
      char macStr[18];
      snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
      Serial.print("p1 sending to:");
      Serial.println(macStr);
      esp_now_send(receiverECU_Address, (uint8_t *)&partnerP1, sizeof(partnerP1));
      P1TOFtrigger = 0;
      TransmisionStatus = SENDINGDATA_en;
    } else {
      if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

        Serial.println("p1 Transmision succesful ");
        TransmisionStatus = ONLYRECEIVE_en;
        intrerruptTOF = false;
        TOFsensor.VL6180xClearInterrupt();
        clearRGBcolors();
        dataP1received = 0;
      }
    }
  } else {
    packetSettings.winnerPartner = 1;
  }
}

void TRAINING_PARTNERMODE_player2Loop(void) {
  if (partnerP2.counterExercisePartner < packetSettings.training_counterValStop) {
    setRGBcolors(packetSettings.training_partnerMode_P2Color);
    //Is the sensor active and the ECU is valid ?
    if (intrerruptTOF) {
      selectColor = generateRandomColor();
      Serial.print("Intrerupt received p2 :");
      intrerruptTOF = false;
      randomECUSelectionPartnerP2 = randomECUselectPartner();
      selectECU_number(randomECUSelectionPartnerP2);
      Serial.println(randomECUSelectionPartnerP2);
      partnerP2.activeECU = randomECUSelectionPartnerP2;
      clearRGBcolors();
      TOFsensor.VL6180xClearInterrupt();
      partnerP2.counterExercisePartner++;
      P2TOFtrigger = 1;
    }
    if (TransmisionStatus == SENDDATA_en && P2TOFtrigger == 1) {
      char macStr[18];
      snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
      Serial.print("p2 sending to :");
      Serial.println(macStr);
      esp_now_send(receiverECU_Address, (uint8_t *)&partnerP2, sizeof(partnerP2));
      TransmisionStatus = SENDINGDATA_en;
      P2TOFtrigger = 0;
    } else {
      if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

        Serial.println("p2 Transmision succesful ");
        TransmisionStatus = ONLYRECEIVE_en;
        intrerruptTOF = false;
        TOFsensor.VL6180xClearInterrupt();
        clearRGBcolors();
        dataP2received = 0;
      }
    }
  } else {
    packetSettings.winnerPartner = 2;
  }
}

void TRAINING_PARTNERMODE_SendToActiveP1(void) {

  Serial.println("p1 reset receice flag");

  if (TransmisionStatus == SENDDATA_en) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
    Serial.print("p1 active sending to:");
    Serial.println(macStr);
    esp_now_send(receiverECU_Address, (uint8_t *)&partnerP1, sizeof(partnerP1));
    TransmisionStatus = SENDINGDATA_en;
  } else {
    if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

      Serial.println("Transmision succesful p1");
      TransmisionStatus = ONLYRECEIVE_en;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      dataP1received = 0;
    }
  }
  if (TransmisionStatus == SENDINGDATA_en || TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

  } else {
    selectECU_number(randomECUSelectionPartnerP1);
  }
}

void TRAINING_PARTNERMODE_SendToActiveP2(void) {

  Serial.println("p2 reset receice flag");

  if (TransmisionStatus == SENDDATA_en) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", receiverECU_Address[0], receiverECU_Address[1], receiverECU_Address[2], receiverECU_Address[3], receiverECU_Address[4], receiverECU_Address[5]);
    Serial.print("p2 active sending to:");
    Serial.println(macStr);
    esp_now_send(receiverECU_Address, (uint8_t *)&partnerP2, sizeof(partnerP2));
    TransmisionStatus = SENDINGDATA_en;
  } else {
    if (TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

      Serial.println("Transmision succesful p2");
      TransmisionStatus = ONLYRECEIVE_en;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      dataP2received = 0;
    }
  }
  if (TransmisionStatus == SENDINGDATA_en || TransmisionStatus == TRANSMISIONSUCCESFULL_en) {

  } else {
    selectECU_number(randomECUSelectionPartnerP2);
  }
}

bool TRAINING_counterSetSelectionFunction(void) {
  bool lreturn = false;
  uint8_t setCounter;
  if (training_counterValStopFlag == false) {
    buttonPushValid();
    setCounter = TRAINING_counterSetSelectionFunctionHelper();
    if (setCounter != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (setCounter > 30) {
      setCounter = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }
    setRGBColorsNumber(interruptModeSelection, 0);  //red
    if (intrerruptTOF == true && setCounter != 0) {
      training_counterValStop = setCounter;
      training_counterValStopFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("training_counterValStop:");
      Serial.println(training_counterValStop);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_SelectNrOfECUsFunction(void) {
  bool lreturn = false;
  uint8_t setEcuCounter;
  if (training_SelectNrOfECUsFlag == false) {
    buttonPushValid();
    setEcuCounter = interruptModeSelection;
    if (setEcuCounter != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (setEcuCounter > 10) {
      setEcuCounter = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }
    setRGBColorsNumber(interruptModeSelection, 6);  //white
    if (intrerruptTOF == true && setEcuCounter != 0) {
      training_SelectNrOfECUs = setEcuCounter;
      training_SelectNrOfECUsFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("training_SelectNrOfECUs:");
      Serial.println(training_SelectNrOfECUs);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_nrOfColorsFunction(void) {
  bool lreturn = false;
  uint8_t setColor;
  if (training_nrOfColorsFlag == false) {
    buttonPushValid();
    setColor = interruptModeSelection;
    if (setColor != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (setColor > 6) {
      setColor = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }
    setRGBColorsNumber(setColor, 1);  //green
    if (intrerruptTOF == true && setColor != 0) {
      training_nrOfColors = setColor;
      training_nrOfColorsFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("training_nrOfColors:");
      Serial.println(training_nrOfColors);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_stopTimeDurationFunction(void) {
  bool lreturn = false;
  uint16_t stopDuration = 0;
  if (training_stopTimeDurationFlag == false) {
    buttonPushValid();
    stopDuration = TRAINING_stopTimeDurationFunctionHelper();
    if (stopDuration != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (stopDuration > 1200) {
      stopDuration = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }
    setRGBColorsNumber(interruptModeSelection, 2);  //blue
    if (intrerruptTOF == true && stopDuration != 0) {
      training_stopTimeDuration = stopDuration;
      training_counterValStopFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("stopDuration:");
      Serial.println(stopDuration);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_setMaxIntervalTime(void) {
  bool lreturn = false;
  uint8_t maxTime = 0;
  if (training_timerMode_maxIntervalTimeFlag == false) {
    buttonPushValid();
    maxTime = interruptModeSelection;
    if (maxTime != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (interruptModeSelection > 6) {
      maxTime = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }
    setRGBColorsNumber(interruptModeSelection, 5);  //magenta
    if (intrerruptTOF == true && maxTime != 0) {
      training_timerMode_maxIntervalTime = maxTime;
      training_timerMode_maxIntervalTimeFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("training_timerMode_maxIntervalTime:");
      Serial.println(training_timerMode_maxIntervalTime);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_setMinIntervalTime(void) {
  bool lreturn = false;
  uint8_t minTime = 0;
  if (training_timerMode_minIntervalTimeFlag == false) {
    buttonPushValid();
    minTime = interruptModeSelection;
    if (minTime != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if ((interruptModeSelection > 6) || (minTime > training_timerMode_maxIntervalTime)) {
      minTime = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }
    setRGBColorsNumber(interruptModeSelection, 4);  //aqua
    if (intrerruptTOF == true && minTime != 0) {
      training_timerMode_minIntervalTime = minTime;
      training_timerMode_minIntervalTimeFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("training_timerMode_minIntervalTime:");
      Serial.println(training_timerMode_minIntervalTime);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_selectColor_P1(void) {
  bool lreturn = false;
  uint8_t setColor;
  if (training_partnerMode_P1ColorFlag == false) {

    buttonPushValid();
    setColor = interruptModeSelection;
    if (setColor != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (setColor > NUMBEROFCOLORS - 1) {
      interruptModeSelection = 1;
    }

    setRGBcolors(interruptModeSelection);
    if (intrerruptTOF == true && setColor != 0) {
      training_partnerMode_P1Color = setColor;
      training_partnerMode_P1ColorFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("P1:");
      Serial.println(training_partnerMode_P1Color);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_selectColor_P2(void) {
  bool lreturn = false;
  uint8_t setColor;
  if (training_partnerMode_P2ColorFlag == false) {
    buttonPushValid();
    setColor = interruptModeSelection;
    if (setColor != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (setColor == training_partnerMode_P1Color) {
      setColor++;
      interruptModeSelection++;
    }
    if (setColor > NUMBEROFCOLORS - 1) {
      interruptModeSelection = 1;
    }


    setRGBcolors(interruptModeSelection);
    if (intrerruptTOF == true && setColor != 0) {
      training_partnerMode_P2Color = setColor;
      training_partnerMode_P2ColorFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("P2:");
      Serial.println(training_partnerMode_P2Color);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

bool TRAINING_changeDistanceFunction(void) {
  bool lreturn = false;
  uint8_t distance;
  if (training_changeDistanceFlag == false) {
    buttonPushValid();
    distance = TRAINING_changeDistanceFunctionHelper();
    if (distance != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (interruptModeSelection > 6) {
      distance = 0;
      interruptModeSelection = 0;
      clearRGBcolors();
    }

    setRGBColorsNumber(interruptModeSelection, 3);  //yellow
    if (intrerruptTOF == true && distance != 0) {
      training_changeDistance = distance;
      training_changeDistanceFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      delay(50); 
      TOFsensor.VL6180xSetDistInt(training_changeDistance, training_changeDistance);
      delay(50); 
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}

void setupTrainingCounterMode(void) {
  TOFsensor.VL6180xInit();
  TOFsensor.VL6180xDefaultSettings();

}

bool TRAINING_CounterModeSetReps(void) {
  bool lreturn = false;
  uint8_t setReps;
  if (training_counterMode_setRepsFlag == false) {
    buttonPushValid();
    setReps = interruptModeSelection;
    if (setReps != 0) {
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
    }
    if (setReps > RGBLEDNUM) {
      interruptModeSelection = 1;
    }
    setRGBColorsNumber(interruptModeSelection, 3);  // YELLOW
    if (intrerruptTOF == true && setReps != 0) {
      training_counterMode_setReps = setReps;
      training_counterMode_setRepsFlag = true;
      interruptModeSelection = 0;
      intrerruptTOF = false;
      TOFsensor.VL6180xClearInterrupt();
      Serial.print("training_counterMode_setReps:");
      Serial.println(training_counterMode_setReps);
      clearRGBcolors();
      lreturn = true;
    }
  }
  return lreturn;
}
