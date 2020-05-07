#include <Arduino.h>

// Piny wejściowe i wyjsciowe macierzy 8x8 dla szachownicy 
int tab_out[8]={13,12,14,27,26,25,33,32};
int tab_read[8]={15,22,4,16,17,5,18,19};
void odczyt();
void initialize();
void wypisz();

const char* TablicaPozycji[8][8]={{"A1","A2","A3","A4","A5","A6","A7","A8"},
                                  {"B1","B2","B3","B4","B5","B6","B7","B8"},
                                  {"C1","C2","C3","C4","C5","C6","C7","C8"},
                                  {"D1","D2","D3","D4","D5","D6","D7","D8"},
                                  {"E1","E2","E3","E4","E5","E6","E7","E8"},
                                  {"F1","F2","F3","F4","F5","F6","F7","F8"},
                                  {"G1","G2","G3","G4","G5","G6","G7","G8"},
                                  {"H1","H2","H3","H4","H5","H6","H7","H8"},
                                 };

int PreviousState[8][8]; //Tablica z poprzednim odczytem
int state[8][8]; // aktualnie odcztywany stan do porównania z poprzednim z tablicy
int reading[8][8]; // 

unsigned long timer = 10; 
unsigned long debounceDelay = 15; 
unsigned long lastDebounceTime[8][8];// czas debouncingu na odczycie kontaktronu 


void setup() {
  
    initialize();
    
}

void loop() {

 odczyt();
 if(millis()-timer>9000)
 {
   wypisz();
   Serial.println();
   Serial.println();
   Serial.println();
   Serial.println();
   timer=millis();
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

    for (int i=0;i<8;i++){
    
      digitalWrite(tab_out[i],LOW); // ustawienie kolumny w stanie niskim do odczytu        
    
      for(int j=0;j<8;j++)
      {
        reading[i][j]= digitalRead(tab_read[j]);
        state[i][j]= reading[i][j];
        PreviousState[i][j]=reading[i][j];
      }

      digitalWrite(tab_out[i],LOW);
    }

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
        Serial.print("Reset"); Serial.println(TablicaPozycji[i][j]); Serial.println(reading[i][j]);  // resetuj timer debouncera     
      }
      if( ( millis()-lastDebounceTime[i][j] ) > debounceDelay)
      {
        if(reading[i][j]!=state[i][j])

          state[i][j]=reading[i][j] ;// zapisz odczyt jako aktualny stan 

          if(state[i][j]==0){
           // Serial.println("Postawiono figurę na pozycji: " ); Serial.println(TablicaPozycji[i][j]);
          } else
          {
            // Serial.println("Podniesiono figurę z pozycji: " ); Serial.println(TablicaPozycji[i][j]);
          }
          
      } 
      PreviousState[i][j]=reading[i][j]; // zapisz odczyt jako stan poprzedni        
    }
        
    digitalWrite(tab_out[i],HIGH); // ustawienie kolumny w stanie niskim do odczytu
  }
}

void wypisz()
{
  for(int i =0; i<8 ; i++)
  {
    if(i==0)
    {
        Serial.println("________________");
    }
    
    
    for(int j =0; j<8 ; j++)
    {
      Serial.print(state[i][j]);Serial.print("|");
    }
    if(i==7)
    {
        Serial.println("________________");
    }
    else
    {
      Serial.println();
    }
    

  }
}
