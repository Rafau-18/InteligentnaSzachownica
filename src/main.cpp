#include <Arduino.h>

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Button.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/Org_01.h>



//1. Change the following info
#define FIREBASE_HOST "chessboard-iot2020.firebaseio.com"
#define FIREBASE_AUTH "whgvLkMDEeQ7ej6ur0ZBSfB1Mgx8a1vc13HbRRxZ"
#define WIFI_SSID "Orange_Swiatlowod_09E0"
#define WIFI_PASSWORD "GQKH6GTEKWLC"
hw_timer_t * timer1 = NULL;
hw_timer_t * timer2 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

#define OLED_RESET -1 // LED_BUILTIN  
Adafruit_SSD1306 display1(128, 64, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(128, 64, &Wire, OLED_RESET);
#define XPOS 0
#define YPOS 1
#define DELTAY 2
int WhiteTime = 900;
int BlackTime = 900;
int W_Minute1;
int W_Minute2;
int W_Second1;
int W_Second2;
int B_Minute1;
int B_Minute2;
int B_Second1;
int B_Second2;

Button ButtonWhite(32);
Button ButtonBlack(35);

int tab_out[8]={15,2,4,16,17,5,18,19};
int tab_read[8]={13,12,14,27,26,25,33,34};
void odczyt();
void initialize();
void wypisz();
void wypiszFigury();
void Whitedisplay();
void Blackdisplay();
void WaitForStartPosition();
void WhiteMove(int moveFrom[2],char Piece, int moveTo[2]);
void BlackMove(int moveFrom[2],char Piece, int moveTo[2]);
void TakePiece(int moveFrom[2],char Piece);
void PutPiece(int moveTo[2],char Piece);
int move_from[2];
int move_to[2];
char moved_piece; 
char captured_piece;
char move_with_capture=0;

unsigned long timer = 10; 
unsigned long debounceDelay = 75; // czas debouncingu na odczycie kontaktronu 
unsigned long lastDebounceTime[8][8];
FirebaseData standane;
bool WhiteDisplayUpdate=0;
bool BlackDisplayUpdate=0;
bool ReadyToStart=0; 
bool StartFlag = 0;
bool FirstWhiteMove=0;
bool FirstBlackMove=0;
bool TurnBlack=0; //If 0 trun WHITE 
int PieceCount =0; 
int tmpCord[2];
bool move_done=0;
bool piece_taken=0;
bool piece_puted=0; 





const String TablicaPozycji[8][8]={{"A1","B1","C1","D1","E1","F1","G1","H1"},
                                  {"A2","B2","C2","D2","E2","F2","G2","H2"},
                                  {"A3","B3","C3","D3","E3","F3","G3","H3"},
                                  {"A4","B4","C4","D4","E4","F4","G4","H4"},
                                  {"A5","B5","C5","D5","E5","F5","G5","H5"},
                                  {"A6","B6","C6","D6","E6","F6","G6","H6"},
                                  {"A7","B7","C7","D7","E7","F7","G7","H7"},
                                  {"A8","B8","C8","D8","E8","F8","G8","H8"}                                                         
                                 };
const int InitialSetting0[8][8]={ {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {1,1,1,1,1,1,1,1},
                                  {0,0,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,0,0},
};
const char InitialSetting[8][8]={ {'R','N','B','Q','K','B','N','R'},
                                  {'P','P','P','P','P','P','P','P'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'p','p','p','p','p','p','p','p'},                                  
                                  {'r','n','b','q','k','b','n','r'},
};
char CurrentPieceState[8][8]=   { {'R','N','B','Q','K','B','N','R'},
                                  {'P','P','P','P','P','P','P','P'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'0','0','0','0','0','0','0','0'},
                                  {'p','p','p','p','p','p','p','p'},                                  
                                  {'r','n','b','q','k','b','n','r'},
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
int reading[8][8]=      {{1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1},
                         {1,1,1,1,1,1,1,1}}; // 



void IRAM_ATTR onTimer(){
  portENTER_CRITICAL_ISR(&timerMux0);
  WhiteTime-=1;
  WhiteDisplayUpdate=1; 
  //Serial.println("OnTimer");
  portEXIT_CRITICAL_ISR(&timerMux0);
}

void IRAM_ATTR onTimer2(){
  portENTER_CRITICAL_ISR(&timerMux1);
  BlackTime-=1;
  BlackDisplayUpdate=1; 
  //Serial.println("OnTimer2");
 portEXIT_CRITICAL_ISR(&timerMux1);
}

void setup() {
  
      

    ButtonWhite.begin();
    ButtonBlack.begin();   

    display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // display 1 address 0x3C
    display2.clearDisplay(); 
    display2.display();
    display2.clearDisplay(); 
    display2.setCursor(0,0);
    display2.setTextColor(WHITE, BLACK);
    display2.setTextSize(2);
    display2.println("Setup is OK");
    display2.display();

    display1.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // display 1 address 0x3C
    display1.clearDisplay(); 
    display1.display();
    display1.clearDisplay(); 
    display1.setCursor(0,0);
    display1.setTextColor(WHITE, BLACK);
    display1.setTextSize(2);
    display1.println("Setup is OK");
    display1.display(); 
    delay(500); 
    Serial.println("start timer ");
    timer1 = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
    timerAttachInterrupt(timer1, &onTimer, true); // edge (not level) triggered 
    timerAlarmWrite(timer1, 1000000, true); // 1000000 * 1 us = 1 s, autoreload true
    //timerAlarmEnable(timer1);
    timer2 = timerBegin(1, 80, true);
    timerAttachInterrupt(timer2, &onTimer2, true); // edge (not level) triggered 
    timerAlarmWrite(timer2, 1000000, true); // 1000000 * 1 us = 1 s, autoreload true
    
    delay(500);
    Whitedisplay();
    Blackdisplay();
    initialize();
    
}

void loop() {
  
  odczyt();
  if(ReadyToStart)
  {
    if(WhiteDisplayUpdate){
        Whitedisplay();
        WhiteDisplayUpdate=0;
      }  

      if(BlackDisplayUpdate){
        Blackdisplay();
        BlackDisplayUpdate=0;
      }
      
      if(ButtonWhite.released()){
        
        
        if (StartFlag && FirstWhiteMove && move_done)
        {
          timerAlarmEnable(timer2);
          timerAlarmDisable(timer1);
          move_done=0;
          move_with_capture=0;
          wypiszFigury();
          TurnBlack=!TurnBlack;
        }
        else if( StartFlag==1 && FirstWhiteMove==0 && move_done )
        {
          FirstWhiteMove=1;
          move_done=0;
          move_with_capture=0;
          wypiszFigury(); 
          TurnBlack=!TurnBlack;
        }
        else if(ReadyToStart && StartFlag==0 && FirstWhiteMove==0)
        {
          StartFlag=1; 
          wypiszFigury();  
          Serial.println("Let's play the GAME :)") ;   
          PieceCount=32;   
        }
        else if(ReadyToStart==0)
        {
          wypisz();  
        }
        
      }

      if(ButtonBlack.released()){

        if(FirstWhiteMove==1 && FirstBlackMove==0 && move_done)
        {
          FirstBlackMove=1;
          wypiszFigury();
          move_done=0;   
          move_with_capture=0;       
          TurnBlack=!TurnBlack;
          timerAlarmEnable(timer1);
        }
        else if (FirstWhiteMove && FirstBlackMove && move_done)
        {
          timerAlarmEnable(timer1);
          timerAlarmDisable(timer2);
          move_done=0; 
          move_with_capture=0; 
          wypiszFigury();
          TurnBlack=!TurnBlack;
        }
        
      }
  }
  else WaitForStartPosition();
  

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
      Serial.println(WIFI_SSID);
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //4. Enable auto reconnect the WiFi when connection lost
      Firebase.reconnectWiFi(true);

    


    Serial.println("Setup done");
    delay(1000);
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
          //Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j], 0);
          
          
        }
        else
        {
         // Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j], 1);         
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
            
            Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j],1);
            PieceCount+=1;
            Serial.print("Postawiono figurę na pozycji: " ); Serial.println(TablicaPozycji[i][j]); 
            if(StartFlag)
            {
              tmpCord[0]=i;
              tmpCord[1]=j;
              PutPiece(tmpCord, moved_piece);
            }
            
            Serial.println("Liczba figur"+String(PieceCount));  
            if(i==0 && j==0)
            {
              //Firebase.setBool(standane, "Status/A2", 1);
            }
          } 
          else
          {                       
            Firebase.setBool(standane, "Status/"+ TablicaPozycji[i][j], 0);
            PieceCount-=1;
            if(StartFlag)
            {
             tmpCord[0]=i;
             tmpCord[1]=j;
             TakePiece(tmpCord, CurrentPieceState[i][j]);
            }
           
            Serial.println("Podniesiono figurę z pozycji: " ); Serial.println(TablicaPozycji[i][j]);
            Serial.println("Liczba figur"+String(PieceCount));
             //timerAlarmDisable(timer1);
          }
         // wypisz();
        }          
          
      } 
      PreviousState[i][j]=reading[i][j]; // zapisz odczyt jako stan poprzedni        
    }
        
    digitalWrite(tab_out[i],HIGH);
    delay(5); // ustawienie kolumny w stanie niskim do odczytu
  }
}

void WaitForStartPosition(){
  int count=0;
  for (int i =0;i<8;i++){
    for(int j=0; j<8; j++)
    {
      if(state[i][j]==InitialSetting0[i][j])
      {
        count+=1;        
      }   
      //
      
    }
  }
  if (count == 64){
        ReadyToStart=true;
        Serial.println("Poprawne ustawienie");
        PieceCount=32;
      } 
      else ReadyToStart=false;
  // Serial.println(count);
}

void TakePiece(int moveFrom[2],char Piece)
{
  if(!TurnBlack)
  {
    if(isupper(Piece))
    {
      move_from[0]=moveFrom[0];
      move_from[1]=moveFrom[1];
      moved_piece=Piece;
      piece_taken=1;
      CurrentPieceState[moveFrom[0]][moveFrom[1]]='0';
    }
    else
    {
      move_with_capture=1;
      captured_piece=Piece;
    }
  }
  else
  {
    if(islower(Piece))
    {
      move_from[0]=moveFrom[0];
      move_from[1]=moveFrom[1];
      moved_piece=Piece;
      piece_taken=1;
      CurrentPieceState[moveFrom[0]][moveFrom[1]]='0';
    }
    else
    {
      move_with_capture=1;
      captured_piece=Piece;
    }
    
  }
  
};

void PutPiece(int moveTo[2],char Piece)
{
    if(moveTo[0] != move_from[0] || moveTo[1] != move_from[1])
    {
      if(piece_taken){
        move_to[0]=moveTo[0];
        move_to[1]=moveTo[1];
        piece_taken=0;
        CurrentPieceState[move_to[0]][move_to[1]]=Piece;
        move_done=1; 
                    
      }
      
    }
    else  
    {
      
      if(move_with_capture)
      {
        CurrentPieceState[moveTo[0]][moveTo[1]]=captured_piece;
        move_with_capture=0;        
      }
      else{
        CurrentPieceState[moveTo[0]][moveTo[1]]=Piece;
        piece_taken=0;
      }
      
      
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

void wypiszFigury()
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
      if(CurrentPieceState[i][j]=='0')
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
        Serial.print(CurrentPieceState[i][j]);Serial.print("|");
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

void Whitedisplay(){
    W_Minute1 = (WhiteTime / 60) / 10;
    W_Minute2 = WhiteTime / 60 - W_Minute1 * 10;
    W_Second1 = (WhiteTime - 60 * (W_Minute1 * 10 + W_Minute2)) / 10;
    W_Second2 = WhiteTime - 60 * (W_Minute1 * 10 + W_Minute2) - 10 * W_Second1;
    
    display1.clearDisplay(); 
    display1.setTextSize(2);
    display1.setCursor(40,0);
    display1.println(String("WHITE"));
    if(piece_taken)
    {
      display1.setTextSize(1);
      display1.setCursor(40,20); 
      display1.println(String(moved_piece));
    }else if(moved_piece && move_with_capture )
    {
      display1.setTextSize(1);
      display1.setCursor(40,20); 
      display1.println(String(moved_piece)+String("x")+String(TablicaPozycji[move_to[1]][move_to[1]] ));
    }
    else if (move_done )
    {
      display1.setTextSize(1);
      display1.setCursor(40,20); 
      display1.println(String(moved_piece)+String("->")+String(TablicaPozycji[move_to[1]][move_to[1]] ));
    }
    
       
    display1.setCursor(20,35);    
    display1.setTextSize(3);
    display1.println(String(W_Minute1)+String(W_Minute2)+String(":")+String(W_Second1)+String(W_Second2));
    display1.display();

}

void Blackdisplay(){

    B_Minute1 = (BlackTime / 60) / 10;
    B_Minute2 = BlackTime / 60 - B_Minute1 * 10;
    B_Second1 = (BlackTime - 60 * (B_Minute1 * 10 + B_Minute2)) / 10;
    B_Second2 = BlackTime - 60 * (B_Minute1 * 10 + B_Minute2) - 10 * B_Second1;
    
    display2.clearDisplay(); 
    display2.setTextSize(2);
    display2.setCursor(40,0);
    display2.println(String("BLACK"));    
    display2.setCursor(20,40);        
    display2.setTextSize(3);   
    display2.println(String(B_Minute1)+String(B_Minute2)+String(":")+String(B_Second1)+String(B_Second2));
    display2.display();
}
