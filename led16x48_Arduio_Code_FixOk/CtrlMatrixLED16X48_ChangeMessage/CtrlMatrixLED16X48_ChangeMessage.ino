//#include <SoftwareSerial.h>
#include "LedControl.h"
#define WORD_LIMIT 20
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 10 is connected to the DataIn 
 pin 8 is connected to the CLK 
 pin 9 is connected to LOAD 
 ***** Please set the number of devices you have *****
 But the maximum default of 8 MAX72XX wil also work.
 */
int PIN_CLK = 2;
int PIN_CS = 3; // chip select
int PIN_DIN = 4;

/* we always wait a bit between updates of the display */
int DevNum = 12;
int Light = 3;

LedControl lc(PIN_DIN, PIN_CLK, PIN_CS, DevNum);

int WalkSize = 7;
int WalkPos = 0;

int WordSize = 4;
int WordPos = 0;
unsigned long oldTime = 0;
unsigned long oldWalkTime = 0;
unsigned long oldLoopTime = 0;
int loopDelay = 10;
int StartTime = 50;
int StopTime = 0;
int OffsetPos = 0;
int Pos = 0;
byte LedBuf[3][32] = {0};       //3個中文
int loopPos = 0;

bool StopLoop = false;

// serial control
int bufCount = 0;
char inputString[300] = {0}; //650
bool stringComplete = false;
int SetMsgSize = 0;

byte MaxMsgSize = WORD_LIMIT;
byte MsgBuf[WORD_LIMIT][32] = {0}; 

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  for(int addr = 0; addr < DevNum; addr ++)
  {
      /*The MAX72XX is in power-saving mode on startup*/
      lc.shutdown(addr,false);
      /* Set the brightness to a medium values */
      lc.setIntensity(addr, Light);
      /* and clear the display */
      lc.clearDisplay(addr);
  }

  const byte* val1 = NULL;
  int len = 32;
  for(int i = 0; i < 4; ++ i)
  {
    val1 = lc.GetArrayByIndex(i);
    memcpy(MsgBuf[i], val1, len);
  }
}

void loop() // run over and over
{
    if(StopLoop)
      return;

    switch(loopPos) {
      case 0:
        if( WordPos == WordSize )
          ++loopPos;
        loopDelay = 10;
        ShiftLeftWord();
        break;
      case 1:
        loopDelay = 300;
        if(SignleWord())
          loopPos = 0;
        break;
    }
}

bool decideLoopTime()
{
    unsigned long currentTime = millis();
    bool hit = false;
    if( currentTime - oldLoopTime >= loopDelay )
    {
        hit = true;
        oldLoopTime = currentTime;
    }

    return hit;  
}

// if ture than exit loop
bool SignleWord()
{
    if(! decideLoopTime())
      return false;

    const byte* val1 = NULL;
    if(WordPos >= WordSize)
    {
        WordPos = 0;
        return true;
    }

    // copy 4 words into LED buffer
    int len = 32;
    for(int i = 0; i < 4; i++)
    {
        if(WordPos >= WordSize)
            WordPos = 0;
        val1 = MsgBuf[WordPos++];
        memcpy(LedBuf[i], val1, len);
    }

    drawTwoRowScreen();
    return false;
}

void ShiftLeftWord()
{
    if(! decideLoopTime())
        return;

    const byte* val1 = NULL;
    byte tmp[32] = {0};
    int len = 32;
    int wordOffset = 18;
    if(WordPos > WordSize)
      WordPos = 0;

    val1 = MsgBuf[WordPos];
    memcpy(tmp, val1, len);

    // shift two word from memory first
     for (byte k = 0; k < 16; k += 2)
     {
      // Up-Left word with high byte
        LedBuf[0][k] <<= 1;
        LedBuf[0][k] |= ((LedBuf[0][k+1] & 0x80) >> 7);
        // Up-Left word with low byte
        LedBuf[0][k+1] <<= 1;
        // middle word with last bit
        LedBuf[0][k+1] |= ((LedBuf[1][k] & 0x80) >> 7);

        // Down-Left word with high byte
        LedBuf[0][k+16] <<= 1;
        LedBuf[0][k+16] |= ((LedBuf[0][k+16+1] & 0x80) >> 7);
        // Down-Left word with low byte
        LedBuf[0][k+16+1] <<= 1;
        // middle word with last bit
        LedBuf[0][k+16+1] |= ((LedBuf[1][k+16] & 0x80) >> 7);



        // Up-Left word with high byte
        LedBuf[1][k] <<= 1;
        LedBuf[1][k] |= ((LedBuf[1][k+1] & 0x80) >> 7);
        // Up-Left word with low byte
        LedBuf[1][k+1] <<= 1;
        // middle word with last bit
        LedBuf[1][k+1] |= ((LedBuf[2][k] & 0x80) >> 7);

         // Down-Left word with high byte
        LedBuf[1][k+16] <<= 1;
        LedBuf[1][k+16] |= ((LedBuf[1][k+16+1] & 0x80) >> 7);
        // Down-Left word with low byte
        LedBuf[1][k+16+1] <<= 1;
        // middle word with last bit
        LedBuf[1][k+16+1] |= ((LedBuf[2][k+16] & 0x80) >> 7);


    
        // Up-Right word with high byte
        LedBuf[2][k] <<= 1;
        LedBuf[2][k] |= ((LedBuf[2][k+1] & 0x80) >> 7);
        LedBuf[2][k+1] <<= 1;

        // Down-Right word with high byte
        LedBuf[2][k+16] <<= 1;
        LedBuf[2][k+16] |= ((LedBuf[2][k+16+1] & 0x80) >> 7);
        LedBuf[2][k+16+1] <<= 1;

        // get single word and shift 1 bit ony by one
        if(Pos <= 7) // high byte
        {
          tmp[k] <<= OffsetPos;
          tmp[k+16] <<= OffsetPos;
        }
        else //low byte
        {
          tmp[k] = tmp[k+1] << OffsetPos;
          tmp[k+16] = tmp[k+16+1] << OffsetPos;
        }

        // Up-Right word with last bit
        LedBuf[2][k+1] |= ((tmp[k] & 0x80) >> 7);
        // Down-Right word with last bit
        LedBuf[2][k+16+1] |= ((tmp[k+16] & 0x80) >> 7);
     }
    
     ++ OffsetPos;
     ++ Pos;
    
     // get next world
     if(OffsetPos >= 8)
        OffsetPos = 0;
     if(Pos >= 16)
     {
        ++ WordPos;
        Pos = 0;
     }

     drawTwoRowScreen();
}

void drawTwoRowScreen()
{
    // Turn on each row in series
    // 16x64 matrix(2x8 LED) order as below:
    // [05][04][03][02][01][00]  00:head, 15:tail
    // [11][10][09][08][07][06]
     for (byte k = 0, i = 0; k < 16; k += 2, ++ i)
     {
          lc.resetStatus();
  
          lc.setRow(1, i, LedBuf[2][k]);
          lc.setRow(0, i, LedBuf[2][k+1]);
          lc.setRow(7, i, LedBuf[2][k+16]);
          lc.setRow(6, i, LedBuf[2][k+16+1]);

          lc.setRow(3, i, LedBuf[1][k]);
          lc.setRow(2, i, LedBuf[1][k+1]);
          lc.setRow(9, i, LedBuf[1][k+16]);
          lc.setRow(8, i, LedBuf[1][k+16+1]);
  
          lc.setRow(5, i, LedBuf[0][k]);
          lc.setRow(4, i, LedBuf[0][k+1]);
          lc.setRow(11, i, LedBuf[0][k+16]);
          lc.setRow(10, i, LedBuf[0][k+16+1]);
          lc.action();
     }
}

//
// serial message process
//

void SetMsgBuf(char* str, int len)
{
  // Serial.println("SetMsgBuf");
  // Serial.print("len=");
  // Serial.print(len);

    int MsgSize = len/32;

    if( MsgSize > MaxMsgSize )
        MsgSize = MaxMsgSize;

    int index = 0;
    for(int i = 0; i < MsgSize; ++ i)
    {
        for(int j = 0; j < 32; ++ j)
        {
            index = i * 32 + j;
            if(index >= len)
                break;

            MsgBuf[i][j] = (byte)str[index]; 
        }
    }

    WordSize = MsgSize;
    // Serial.print("WordSize=");
    // Serial.print(WordSize);
}

void AddMsgBuf(char* str, int len)
{
  // Serial.println("AddMsgBuf");
  // Serial.print("len=");
  // Serial.print(len);
    int MsgSize = len/32;

    if( MsgSize + WordSize > MaxMsgSize )
        MsgSize = MaxMsgSize;

    int index = 0;
    for(int i = 0; i < MsgSize; ++ i)
    {
        for(int j = 0; j < 32; ++ j)
        {
            index = i * 32 + j;
            if(index >= len)
                break;

            MsgBuf[i+WordSize][j] = (byte)str[index]; 
        }
    }

    WordSize += MsgSize;
    // Serial.print("WordSize=");
    // Serial.print(WordSize);
}

void ParseMsg(void)
{
  if(stringComplete)
  {
     char* key = "SetMsg=";
     char* key2 = "AddMsg=";
     char* ptr = NULL;
     ptr = strstr(inputString, key);
     if(ptr)
     {
        //  Serial.println("str SetMsg");
         int offset = strlen(key) + 2; // without 2byte message size
         SetMsgBuf(ptr+offset, bufCount-offset);
     }
     else// if(ptr2)
     {
         ptr = strstr(inputString, key2);
         if(ptr)
         {
            //  Serial.println("str AddMsg");
             int offset = strlen(key2) + 2; // without 2byte message size
             AddMsgBuf(ptr+offset, bufCount-offset);
         }
     }

     bufCount = 0;
     stringComplete = false;
     memset(inputString, 0, sizeof(inputString));
  }
}

void serialEvent()
{
  char* key = "SetMsg=";
  int keyLen = strlen(key);

  while(Serial.available())
  {
      char inChar = (char)Serial.read();
      // Serial.print((unsigned char)inChar, HEX);
      // Serial.print(",");
      if(bufCount >= sizeof(inputString) - 1)
      {
          SetMsgSize = 0;
          stringComplete = true;
          continue;
      }

      inputString[bufCount++] = inChar;

      // with 2 byte message size
      if( SetMsgSize == 0 && bufCount >= keyLen + 2 )
      {
          SetMsgSize = (int)((int)inputString[keyLen] * 256);
          SetMsgSize += (int)(unsigned char)(inputString[keyLen+1]);
          // Serial.print("SetMsgSize=");
          // Serial.println(SetMsgSize);
      }

      if( SetMsgSize && bufCount >= (SetMsgSize+keyLen) )
      {
          // Serial.print("keyLen=");
          // Serial.println(keyLen);
          // Serial.print("SetMsgSize=");
          // Serial.println(SetMsgSize);
          // Serial.print("bufCount=");
          // Serial.println(bufCount);
          
          SetMsgSize = 0;
          stringComplete = true;
          break;
      }
  }
// Serial.print("bufCount=");
// Serial.println(bufCount);
  if(stringComplete)
      ParseMsg();
}
