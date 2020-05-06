#include <Arduino.h>

// Piny wejściowe i wyjsciowe macierzy 8x8 dla szachownicy 
int tab_out[8]={13,12,14,27,26,25,33,32};
int tab_read[8]={15,22,4,16,17,5,18,19};
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
    
  for (int j=2;j<3;j++){

    for(int i=0;i<8;i++){
      digitalWrite(tab_out[i],HIGH);
    }
    delay(50);
    Serial.print("Kolumna: "); Serial.println(j);    
    digitalWrite(tab_out[j],LOW);
    delay(50);
    for(int l=0;l<7;l++){
      Serial.print(digitalRead(tab_read[l]));  
      delay(50);    
    }
    Serial.println(digitalRead(tab_read[7]));
  }
}