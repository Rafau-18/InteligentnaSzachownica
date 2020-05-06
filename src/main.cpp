#include <Arduino.h>

// Piny wejściowe i wyjsciowe macierzy 8x8 dla szachownicy 
int tab_out[8]={13,12,14,27,26,25,33,32};
int tab_read[8]={15,22,4,16,17,5,18,19};
void odczyt();
void initialize();
const char* TablicaPozycji[8][8]={{"A1","A2","A3","A4","A5","A6","A7","A8"},
                                  {"B1","B2","B3","B4","B5","B6","B7","B8"},
                                  {"C1","C2","C3","C4","C5","C6","C7","C8"},
                                  {"D1","D2","D3","D4","D5","D6","D7","D8"},
                                  {"E1","E2","E3","E4","E5","E6","E7","E8"},
                                  {"F1","F2","F3","F4","F5","F6","F7","F8"},
                                  {"G1","G2","G3","G4","G5","G6","G7","G8"},
                                  {"H1","H2","H3","H4","H5","H6","H7","H8"},
                                 };
//----------------

void setup() {
  
    initialize();
    
}

void loop() {

 odczyt();
 delay(500);

}


void initialize(){

    Serial.begin(115200);
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
    pinMode(tab_read[7],INPUT_PULLUP);    
    digitalWrite(tab_out[0],HIGH);
    digitalWrite(tab_out[1],HIGH);
    digitalWrite(tab_out[2],HIGH);
    digitalWrite(tab_out[3],HIGH);
    digitalWrite(tab_out[4],HIGH);
    digitalWrite(tab_out[5],HIGH);
    digitalWrite(tab_out[6],HIGH);
    digitalWrite(tab_out[7],HIGH);    
    delay(1500);
    Serial.println("Setup done");
    
}

void odczyt(){
    
  for (int j=0;j<8;j++){
    
     digitalWrite(tab_out[j],LOW); // ustawienie kolumny w stanie niskim do odczytu        
    
    for(int l=0;l<8;l++){
      //Serial.print(digitalRead(tab_read[l]));  
      delay(50);    
    }
        
    digitalWrite(tab_out[j],LOW); // ustawienie kolumny w stanie niskim do odczytu
  }
}

