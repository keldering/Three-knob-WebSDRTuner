
#include <Wire.h>
#include <TinyUSB_Mouse_and_Keyboard.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define CLK1 1  //Rot1
#define DATA1 2 //Rot1
#define MOD 0   //Rot1
#define CLK2 7  //Rot2
#define DATA2 8 //Rot2
#define FRQ 9   //Rot2
#define CLK3 3  //Rot3
#define DATA3 6 //Rot3
#define MSC 10  //Rot3

static uint8_t PrNxt1 = 0;
static uint16_t store1=0;
static uint8_t PrNxt2 = 0;
static uint16_t store2=0;
static uint8_t PrNxt3= 0;
static uint16_t store3=0;
static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};
static int8_t modu=0;
static int8_t prvmodu=0;
static int8_t freq=0;
static int8_t prvfreq=0;
static int8_t misc=0;
static int8_t prvmisc=0;
static int8_t c1,val1;
static int8_t c2,val2;
static int8_t c3,val3;
static int vol=3;
static int initial_band=0; //future use reminder

static int modulation_codes[] = {97,97,65,65,65,65,65,100,108,108,117,117,99,99,102,113};

//  For RaspSDR 6m and IPB25               0       1         2       3        4        5        6       7     8       9
//static char *band_display_codes[] =   {"LW"  ,"MW"    ,"120m" ,"90m"   ,"75m"   ,"60m"    ,"49m"  ,"41m"  ,"31m"  ,"25m"  ,
//                                      "22m"  ,"19m"   ,"16m"  ,"15m"   ,"13m"   ,"11m"    ,"VLF"  ,"LF"   ,"NDB"  ,"DGPS" ,
//                                      "TM2.5","TM3.33","TM5"  ,"TM7.85","TM 10" ,"TM14.67","TM15" ,"TM20" ,"TM25" ,"LF"   ,
//                                      "MF"   ,"160m" ,"80m"   ,"60m"  ,"40m"    ,"30m"    ,"20m"  ,"17m"  ,"15m"  ,"12m"  ,
//                                      "10m"  , "6m"  ,"IPB20" ,"IPB17","IPB15" ,"IPB12" ,"IPB10"  ,"3594" ,"4558" ,"5154" ,
//                                      "5156L","5292B","6928V" ,"7039" ,"7509 " ,"8000C" ,"8495"   ,"10872","13528","16332","20048"};
       
       //  For KiwiSDR no 6m and no IPB25   0       1         2       3        4        5        6       7     8       9
static char *band_display_codes[] =   {"LW"  ,"MW"    ,"120m" ,"90m"   ,"75m"   ,"60m"    ,"49m"  ,"41m"  ,"31m"  ,"25m"  ,
                                      "22m"  ,"19m"   ,"16m"  ,"15m"   ,"13m"   ,"11m"    ,"VLF"  ,"LF"   ,"NDB"  ,"DGPS" ,
                                      "TM2.5","TM3.33","TM5"  ,"TM7.85","TM 10" ,"TM14.67","TM15" ,"TM20" ,"LF"   ,
                                      "MF"   ,"160m" ,"80m"   ,"60m"  ,"40m"    ,"30m"    ,"20m"  ,"17m"  ,"15m"  ,"12m"  ,
                                      "10m"  ,"IPB20" ,"IPB17","IPB15" ,"IPB12" ,"IPB10"  ,"3594" ,"4558" ,"5154" ,
                                      "5156L","5292B","6928V" ,"7039" ,"7509 " ,"8000C" ,"8495"   ,"10872","13528","16332","20048"};                               
                                      
                                      
//BROADCAST/UTILITY/DGPS                   
//   0,    1,      2,     3,     4,      5,     6,     7,    8,     9,    10,   11,   12,    13,    14,    15,    16,    17,     18,      19,       20,       21,   22,       23,      24,       25,      26,     27,    28,    
// "LW","MW", "120m", "90m", "75m", "60m" ,"49m" ,"41m" ,"31m", "25m", "22m","19m","16m", "15m", "13m", "11m","VLF" , "LF" , "NDB" , "DGPS" , "TM2.5" , "TM3.33","TM5","TM7.85" ,"TM 10" ,"TM14.67","TM15"  ,"TM20" ,"TM25",
//AMATEUR/BEACONS/MARKERS
//  29,    30,     31,    32,    33,    34,    35,    36,    37,    38,    39,    40,     41,     42,      43,      44,      45,      46,     47,     48,     49,      50,      51,      52,     53,     54,      55,     56,      57,      58,      59,    60
//"LF",  "MF", "160m", "80m", "60m", "40m", "30m", "20m", "17m", "15m", "12m", "10m",  "6m", "IPB20", "IPB17", "IPB15", "IPB12", "IPB10", "3594", "4558", "5154", "5156L", "5292B", "6928V", "7039", "7509", "8000C", "8495", "10872", "13528", "16332", "20048"        
            

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//----------------------------------------------------------------------------------Rotary functions 
int8_t read_rotary_1() {
  PrNxt1 <<= 2;
  if (digitalRead(DATA1)) PrNxt1 |= 0x02;
  if (digitalRead(CLK1)) PrNxt1 |= 0x01;
  PrNxt1 &= 0x0f;   // If valid then store as 16 bit data.
   if  (rot_enc_table[PrNxt1] ) {
      store1 <<= 4;
      store1 |= PrNxt1;     
      if ((store1&0xff)==0x2b) return -1;
      if ((store1&0xff)==0x17) return 1;
   }
   return 0;
}

int8_t read_rsw1() {
  if (!digitalRead(MOD)){
    delay(300);    
    modu += 1;   
    if(modu>=3)modu=0;
  }
  return modu;
}

int8_t read_rotary_2() {
  PrNxt2 <<= 2;
  if (digitalRead(DATA2)) PrNxt2 |= 0x02;
  if (digitalRead(CLK2)) PrNxt2 |= 0x01;
  PrNxt2 &= 0x0f;
   if  (rot_enc_table[PrNxt2] ) {
      store2 <<= 4;
      store2 |= PrNxt2;
      if ((store2&0xff)==0x2b) return -1;
      if ((store2&0xff)==0x17) return 1;
   }
   return 0;
}

int8_t read_rsw2() {
  if (!digitalRead(FRQ)){
    delay(300);    
    freq += 1;   
    if(freq>=4)freq=0;
  }
  return freq;
}

// A valid CW->(-1)or CCW<-(+1); invalid returns 0 MSC
int8_t read_rotary_3() {
  PrNxt3 <<= 2; 
  
  if (digitalRead(DATA3)) PrNxt3 |= 0x02;
  if (digitalRead(CLK3)) PrNxt3 |= 0x01;
  PrNxt3 &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[PrNxt3] ) {
      store3 <<= 4;
      store3 |= PrNxt3;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((store3&0xff)==0x2b) return -1;
      if ((store3&0xff)==0x17) return 1;
     
   }
   return 0;
}

int8_t read_rsw3() {
  if (!digitalRead(MSC)){
    delay(300);    
    misc += 1;
    if(misc>=5)misc=0;
  }
  return misc;
}
//----------------------------------------------------------------------------------Display routines
void disp_init(){
    display.clearDisplay();display.display();
    display.setCursor(0,0); display.print ("Left-|--Mid--|-Right");
    if (misc==0) {display.setCursor(0,10);display.print("-------LOCKED-------");} else
                 {display.setCursor(0,9); display.print ("-MOD-|--FRQ--|-DISP-");}
    display.display();
}
void disp_all(){
    display.clearDisplay();display.display();
    display.setCursor(0,0); display.println ("Left-|--Mid--|-Right");
    if (misc==0) {display.setCursor(0,9);  display.print("-------LOCKED-------");} else
                 {display.setCursor(0,9);  display.print("-MOD-|--FRQ--|-DISP-");}
    
    if (modu==0){ 
        //static int keyb_codes[] = {97,97,65,65,65,65,65,100,108,108,117,117,99,99,102,113};
        display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-");                                                       
        if (c1==0)  {display.setCursor(0,18); display.print("AM");}
        if (c1==1)  {display.setCursor(0,18); display.print("AMN");}    
        if (c1==2)  {display.setCursor(0,18); display.print("SAM");}
        if (c1==3)  {display.setCursor(0,18); display.print("SAL");}
        if (c1==4)  {display.setCursor(0,18); display.print("SAU");}
        if (c1==5)  {display.setCursor(0,18); display.print("SAS");}
        if (c1==6)  {display.setCursor(0,18); display.print("QAM");}     
        if (c1==7)  {display.setCursor(0,18); display.print("DRM");}             
        if (c1==8)  {display.setCursor(0,18); display.print("LSB");}
        if (c1==9)  {display.setCursor(0,18); display.print("LSN");}             
        if (c1==10)  {display.setCursor(0,18); display.print("USB");}
        if (c1==11)  {display.setCursor(0,18); display.print("USN");}             
        if (c1==12)  {display.setCursor(0,18); display.print("CW");}
        if (c1==13)  {display.setCursor(0,18); display.print("CWN");}    
        if (c1==14){display.setCursor(0,18); display.print("NBFM");}        
        if (c1==15){display.setCursor(0,18); display.print("IQ");}    
    }
    if (modu==2){
     display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-");  
     display.setCursor(0,18); display.print("VOL");  display.setCursor(20,18); display.print(c1);     
    }
     if (freq==0) {
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(30,18); display.print   ("|FAST   | ");
     }
     if (freq==1) {
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(30,18);    display.print("|SLOW   |");  
     }
     if (freq==2) {
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(30,18);    display.print("|PASS   |");  
     }

     if (freq==3) {
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(30,18);    display.print("|BAND   |");
      display.setCursor(30,28); display.print(band_display_codes[c2]);
     }
     if (misc==1){
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(77,18); display.print("|ZOOM");  
     }
     if (misc==2){
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(77,18); display.print("WF Min");  
     }
     if (misc==3){
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(77,18); display.print("WF floor");  
     }
     if (misc==4){
      display.setCursor(0,9); display.print("-MOD-|--FRQ--|-DISP-"); 
      display.setCursor(77,18); display.print("WF rate");  
     }
     if (misc==0){
      disp_init();
     }
     display.display(); 
     
}

//----------------------------------------------------------------------------------Initial Setup routines
void setup() {
  pinMode(CLK1, INPUT);
  pinMode(CLK1, INPUT_PULLUP);
  pinMode(DATA1, INPUT);
  pinMode(DATA1, INPUT_PULLUP);
  pinMode(CLK2, INPUT);
  pinMode(CLK2, INPUT_PULLUP);
  pinMode(DATA2, INPUT);
  pinMode(DATA2, INPUT_PULLUP);
  pinMode(CLK3, INPUT);
  pinMode(CLK3, INPUT_PULLUP);
  pinMode(DATA3, INPUT);
  pinMode(DATA3, INPUT_PULLUP);
  pinMode(MOD, INPUT);
  pinMode(MOD, INPUT_PULLUP);
  pinMode(FRQ, INPUT);
  pinMode(FRQ, INPUT_PULLUP);
  pinMode(MSC, INPUT);
  pinMode(MSC, INPUT_PULLUP);
  
  Keyboard.begin();
  SerialUSB.begin (115200);
  SerialUSB.println("KY-040 Start:");
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  //display.drawBitmap(0,0,bitmap_ui, 128,64, SSD1306_WHITE);
  display.setTextSize(1);            
  display.setTextColor(SSD1306_WHITE);        
 
  display.setCursor(0,0); display.println ("Left-|--Mid--|-Right");
  display.setCursor(0,10); display.println("-------LOCKED-------");
  //display.setCursor(0,20); display.println("Rot3:");  
  display.display();
  val1=0;
  c1=0;
}



//----------------------------------------------------------------------------------Main Loop

void loop() {
if (misc!=0){ 

//----------------------------------------------------------------------------------Switch Handling MOD Rotary 1 SW
             prvmodu=modu; 
             modu=read_rsw1();
             if (prvmodu != modu) {
             
             SerialUSB.print("modu=");
             SerialUSB.println(modu,DEC);
             if (modu==2) c1=vol;// initial volume
             if (modu==0) c1=0;// initial modulation type
             disp_all();
             }
             ;
//----------------------------------------------------------------------------------Handling MOD Rotary Encoder 1
//            c1 c1
// a 97  AM   0  0
// A 65  SAM  1 -1 
// d 100 DRM  2 -2
// l 108 LSB  3 -3
// u 117 USB  4 -4
// c 99  CW   5 -5
// f 102 NFRM 6 -6
// q 113 IQ   7 -7   




   
//static int keyb_codes[] = {97,97,65,65,65,65,65,100,108,108,117,117,99,99,102,113};
//|------------------------------------------------------------------------------------------------------------------------|
//|IQ,NBFM,CWN,CW,USN,USB,LSN,LSB,DRM,SAM,SAL,SAU,SAS,QAM,AMN,AM,AMN,QAM,SAS,SAU,SAL,SAM,DRM,LSB,LSN,USB,USN,CW,CWN,NBFM,IQ|
//|15,14  ,13 ,12,11 ,10 ,9  ,8  ,7  ,6  ,5  ,4  ,3  ,2  ,1  ,0 ,1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,10 ,11 ,12,13 ,14  ,15|
//|-----------------------------------------------------------0++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++|
//|q  f    c   c  u   u   l   l   d   A   A   A   A   A   a   a  a   A   A   A   A   A   d   l   l   u   u   c  c   f    q |
//-------------------------------------------------------------------------------------------------------------------------|
 if((val1=read_rotary_1())) {  
     
    if (modu==0){
         //Keyboard.write(97);  
          if (c1==16) c1=0;
          if (c1<0) c1=15;
          Keyboard.write(modulation_codes[c1]);
          disp_all();
          SerialUSB.print("c1=");
          SerialUSB.print(c1,DEC);                  
          SerialUSB.print(" mod=");
          SerialUSB.println(modulation_codes[c1]);  
          c1 +=val1;          
    } //endif (modu==0)  
   //Handling VOL Rot1
    if (modu==2){
                 
          if ( PrNxt1==0x0b) {
                  
                  Keyboard.write(118);//V                                 
                  SerialUSB.println(c1,DEC);                 
                }                            
               
          
          if ( PrNxt1==0x07) {
                  Keyboard.write(86);//v
                  SerialUSB.println(c1,DEC);
               }                                                                                                                              
    c1 +=val1;  
    if (c1>=20) c1=20;
    if (c1<=0) c1=0;   
    disp_all(); 
    } 
}//end readrotary_1                       
         
 //----------------------------------------------------------------------------------Switch Handling FRQ Rotary 2 SW        
  //Switch Handling FRQ Rotary Button SW2
             prvfreq=freq; 
             freq=read_rsw2();
             if (prvfreq != freq){ 
             SerialUSB.println(freq,DEC);
             if (freq==3) c2=initial_band;// initial band
             disp_all();
             }
//----------------------------------------------------------------------------------Handling FRQ Rotary Encoder 2
  //Handling FRQ Rotary encoder2 
  //Broadcast / Utility etc.
        
int8_t valx=0;
int8_t freq3_valx;                 
            if( (val2=read_rotary_2()) ) {                                                                                                  //SerialUSB.print(c2);Serial.print(" ");
                if ( PrNxt2==0x0b) {
                  if  (freq==0){Keyboard.press(129); Keyboard.write(215); Keyboard.releaseAll();}  //FAST
                  if  (freq==1){Keyboard.write(215);}  //SLOW
                  if  (freq==2){Keyboard.write(112);}  //PASS p                                                                                                                       //                    59    0   1    2
                  if  (freq==3){Keyboard.write(66);}   //BAND B  CW        20048->LW->MW->120....                                
                }                            
                if ( PrNxt2==0x07) {
                   if (freq==0) {Keyboard.press(129); Keyboard.write(216); Keyboard.releaseAll();} // FAST
                   if (freq==1) {Keyboard.write(216);}  //SLOW
                   if (freq==2){Keyboard.write(80);}   //PASS P                                                                            2    1   0   59
                   if (freq==3){Keyboard.write(98);}   //BAND b CCW   ..  120<-MW<-LW<-20048
                       
                       //SerialUSB.print("c2=");
                       //SerialUSB.println(c2,DEC);
                       //SerialUSB.print("band=");
                       //SerialUSB.println(band_display_codes[c2]);
                 }                
                 if (freq==3)
                            
                       if (valx=read_rsw2()) { 
                       freq3_valx=0;  // future initial start in frequencyband  
                       }
                       //                    
            
            c2 +=val2;          
            //if (c2>60) c2=0;
            //if (c2<0) c2=60; //for RaspSDR 2 more entries in band_code[] 6m and IPB25
            if (c2>58) c2=0;
            if (c2<0) c2=58; //for KiwiSDR 2 less entries in band_code[] no 6m and no IPB25
            SerialUSB.print("c2=");
            SerialUSB.println(c2,DEC);
            SerialUSB.print("band=");
            SerialUSB.println(band_display_codes[c2]);
            disp_all();
           }  
                                     
 //----------------------------------------------------------------------------------Handling MISC Rotary Encoder 3  
    
             if( (val3=read_rotary_3()) ) {
                c3 +=val3;
                //SerialUSB.print(c3);Serial.print(" ");
                if ( PrNxt3==0x07) {
                   if  (misc==1){Keyboard.write(122);}// ZOOM z z->CW
                   if  (misc==2){Keyboard.write(119);}//WFmin w w=->CW
                   SerialUSB.print("c3=");
                   SerialUSB.print(c3,DEC);
                   SerialUSB.print(" val3=");
                   SerialUSB.println(val3,HEX);
                   
                }
          
                if ( PrNxt3==0x0b) {
                   if (misc==1)  Keyboard.write(90);//ZOOM Z Z<-CCW
                   if  (misc==2){Keyboard.write(87);}// WFmin W<-CCW
                   SerialUSB.print("c3=");
                   SerialUSB.print(c3,DEC);
                   SerialUSB.print(" val3=");
                   SerialUSB.println(val3,HEX);
                }
             }
      }// end if misc==0)            
            
          
 //----------------------------------------------------------------------------------Switch Handling MISC Rotary 3 SW3     
             prvmisc=misc;
             misc=read_rsw3();
             if (prvmisc != misc) {
              SerialUSB.println(misc,DEC);
              //if (misc==0){display.setCursor(1,10); display.print("*LOCKED*");} else {display.setCursor(1,10); display.print("*OPEN*");} 
              disp_all();                                         
             }
                                              
}