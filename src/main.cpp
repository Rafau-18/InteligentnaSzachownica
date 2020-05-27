#include <Arduino.h>

#include <WiFi.h>
#include <FirebaseESP32.h>

//1. Change the following info
#define FIREBASE_HOST "chessboard-iot2020.firebaseio.com"
#define FIREBASE_AUTH "whgvLkMDEeQ7ej6ur0ZBSfB1Mgx8a1vc13HbRRxZ"
#define WIFI_SSID "Orange_Swiatlowod_09E0"
#define WIFI_PASSWORD "GQKH6GTEKWLC"

//const char* password =  "GQKH6GTEKWLC";



// Piny wejściowe i wyjsciowe macierzy 8x8 dla szachownicy 
//int tab_out[8]={13,12,14,27,26,25,33,32};
//int tab_read[8]={15,22,4,16,17,5,18,19};
//int tab_out[8]={19,18,5,17,16,4,2,15};
int tab_out[8]={15,2,4,16,17,5,18,19};
int tab_read[8]={13,12,14,27,26,25,33,34};
void odczyt();
void initialize();
void wypisz();

const String TablicaPozycji[8][8]={{"A1","B1","C1","D1","E1","F1","G1","H1"},
                                  {"A2","B2","C2","D2","E2","F2","G2","H2"},
                                  {"A3","B3","C3","D3","E3","F3","G3","H3"},
                                  {"A4","B4","C4","D4","E4","F4","G4","H4"},
                                  {"A5","B5","C5","D5","E5","F5","G5","H5"},
                                  {"A6","B6","C6","D6","E6","F6","G6","H6"},
                                  {"A7","B7","C7","D7","E7","F7","G7","H7"},
                                  {"A8","B8","C8","D8","E8","F8","G8","H8"}                                                         
                                 };

int PreviousState[8][8]={{1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1}                                                                                        
                        }; //Tablica z poprzednim odczytem
int state[8][8]; // aktualnie odcztywany stan do porównania z poprzednim z tablicy
int reading[8][8]=      { {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1}}; // 

unsigned long timer = 10; 
unsigned long debounceDelay = 75; 
unsigned long lastDebounceTime[8][8];// czas debouncingu na odczycie kontaktronu 
FirebaseData standane;

void setup() {
  
    initialize();
    
}

void loop() {

 odczyt();
 if(millis()-timer>9000)
 {
   //wypisz();
   //Serial.println();
  // Serial.println();
  // Serial.println();
  // Serial.println();
  // timer=millis();
 }
 

}


void initialize(){

    Serial.begin(9600);
    pinMode(tab_out[0],OUTPUT);
    pinMode(tab_out[1],OUTPUT);
    pinMode(tab_out[2],OUTPUT);
    pinMode(tab_out[3],OUTPUT);
    pinMode(tab_out[4],OUTPUT);
    pinMode(tab_out[5],OUTPUT);
    pinMode(tab_out[6],OUTPUT);
    pinMode(tab_out[7],OUTPUT);    
    pinMode(tab_read[0],INPUT_PULLUP);
    pinMode(tab_read[1],INPUT_PULLUP);
    pinMode(tab_read[2],INPUT_PULLUP);
    pinMode(tab_read[3],INPUT_PULLUP);
    pinMode(tab_read[4],INPUT_PULLUP);
    pinMode(tab_read[5],INPUT_PULLUP);
    pinMode(tab_read[6],INPUT_PULLUP);
    pinMode(tab_read[7],INPUT);    
    digitalWrite(tab_out[0],HIGH);
    digitalWrite(tab_out[1],HIGH);
    digitalWrite(tab_out[2],HIGH);
    digitalWrite(tab_out[3],HIGH);
    digitalWrite(tab_out[4],HIGH);
    digitalWrite(tab_out[5],HIGH);
    digitalWrite(tab_out[6],HIGH);
    digitalWrite(tab_out[7],HIGH);    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
      Serial.println();
      Serial.print("Connected with IP: ");
      Serial.println(WiFi.localIP());
      Serial.println();
      Serial.print(WIFI_SSID);

      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Enable auto reconnect the WiFi when connection lost
      Firebase.reconnectWiFi(true);

    delay(4500);


    Serial.println("Setup done");

    for (int i=0;i<8;i++){
    
      digitalWrite(tab_out[i],LOW); // ustawienie kolumny w stanie niskim do odczytu        
    
      for(int j=0;j<8;j++)
      {
         reading[i][j]= digitalRead(tab_read[j]);
         state[i][j]= reading[i][j];
        // state[i][j]= digitalRead(tab_read[j]);
        PreviousState[i][j]=reading[i][j];
        if(digitalRead(tab_read[j]))
        {
          Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j], 0);
        }
        else
        {
          Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j], 1);
        }
      }

      digitalWrite(tab_out[i],HIGH);
    }
     wypisz();

    Serial.println("Wczytano początkowe ustawienie");
    
}

void odczyt(){
    
  for (int i=0;i<8;i++){
    
     digitalWrite(tab_out[i],LOW); // ustawienie kolumny w stanie niskim do odczytu        
    
    for(int j=0;j<8;j++){
      reading[i][j]= digitalRead(tab_read[j]); // odczytaj stan pinu 
      if(PreviousState[i][j] != reading[i][j])
      {
        lastDebounceTime[i][j]=millis();
       // Serial.print("Reset"); Serial.println(TablicaPozycji[i][j]); Serial.println(reading[i][j]);  // resetuj timer debouncera     
      }
      if( ( millis()-lastDebounceTime[i][j] ) > debounceDelay)
      {
        if(reading[i][j]!=state[i][j])
        {
          state[i][j]=reading[i][j] ;// zapisz odczyt jako aktualny stan 

          if(state[i][j]==0){
            Serial.println("Postawiono figurę na pozycji: " ); Serial.println(TablicaPozycji[i][j]);
            Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j],1);
            if(i==0 && j==0)
            {
              //Firebase.setBool(standane, "Status/A2", 1);
            }
          } 
          else
          {
             Serial.println("Podniesiono figurę z pozycji: " ); Serial.println(TablicaPozycji[i][j]);
            Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j], 0);
          }
          wypisz();
        }          
          
      } 
      PreviousState[i][j]=reading[i][j]; // zapisz odczyt jako stan poprzedni        
    }
        
    digitalWrite(tab_out[i],HIGH);
    delay(5); // ustawienie kolumny w stanie niskim do odczytu
  }
}

void wypisz()
{
  for(int i =7; i>=0 ; i--)
  {
    if(i==7)
    {
        
        Serial.println();
        Serial.println("__________________");           
    }
    for(int j =0; j<8 ; j++)
    {
      if(state[i][j])
      {
        if(j==0)
        {
          Serial.print(i+1);Serial.print("|");
        }
        Serial.print(" ");Serial.print("|");
        
      }
      else{
          if(j==0)
        {
          Serial.print(i+1);Serial.print("|");
        }
        Serial.print("O");Serial.print("|");
      }
      
    }
    if(i==0)
    {
        Serial.println();
        //Serial.println("________________");
        Serial.println("  A B C D E F G H ");   
        Serial.println();
    }
    else
    {
      Serial.println();
    }
    
    

  }
}
