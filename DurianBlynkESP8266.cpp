/*
 * DurianBlynkESP8266.cpp
 *
 * Library for running Blynk in Durian UNO + ESP8266
 * Author: Ng Kwang Chen MYBOTIC @ www.mybotic.com.my
 * Date  : 25 April 2022
 *
 * Copyright reserved
 */

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include "DurianBlynkESP8266.h"

DBlynk::DBlynk(Stream* uart = &Serial): m_puart(uart)
{
  m_puart = uart;
}

int DBlynk::begin(String TempID, String DeviceName, String auth, String ssid, String pass)
{
  unsigned short usStatus = NOK;

  usStatus = resetEsp();  

  if(usStatus == OK)
  {
    sSet_Blynk_TempID(TempID);
    sSet_Blynk_DeviceName(DeviceName);
  }

  if(usStatus == OK)
  {
    usStatus = sSet_Blynk_SSID(ssid);
  }

  if(usStatus == OK)
  {
    sSet_Blynk_Pass(pass);
  }

  if(usStatus == OK)
  {
    usStatus = sSet_Blynk_AuthToken(auth);
  }

  if(usStatus == NOK)
  {
    #ifdef DBLYNK_PRINT_H
      m_DebugUart->print(F("Blynk begin fail...pls check esp!"));   
    #endif

    while(1);
  }
  else
  {
    m_BlynkRunning = 1;
    m_BlynkConnected = 1;
    gulStartCheckConnectionTimer = millis(); //Start Timer for checking connection  
  }
  
  return usStatus;
}

short DBlynk::sRead_Serial_Data(char *szData, unsigned short usSize)
{ 
  short sIsDone = 0;
  short sByte = 0;  
  
  if(m_puart->available())
  {
    gultimeout = millis();

    if(gusSerialByteIndex < usSize)
    {
      gszSerialData[gusSerialByteIndex] = m_puart->read();
      gusSerialByteIndex++;
    }
    else
    {
      m_puart->read();
    }
  }
  else
  {

    unsigned long guTime = millis() - gultimeout;
    
    if(guTime >= UART_TIMEOUT && (gusSerialByteIndex > 0)) //if over time, reset
    {
      sByte = gusSerialByteIndex;
      gusSerialByteIndex = 0;
      sIsDone = 1;

      strcpy(szData, gszSerialData);
      
      memset(gszSerialData, '\0', sizeof(gszSerialData));
    }
  }

  if(sIsDone == 0)
  {
    return -1;
  }
  else
  {
    return sByte;
  }
}

short DBlynk::resetEsp(void)
{
  char szData[50];
  short sDataLen = 0;
  unsigned short usStatus = NOK;
  char szActData[10];
  short k = 0;
  unsigned long ulStart_Timer = 0;

  #ifdef DBLYNK_PRINT_H
    m_DebugUart = &DBLYNK_PRINT_H;
    m_DebugUart->print(F("Reset ESP8266..."));   
  #endif

  m_puart->println(F("!RESET"));

  delay(500);

  memset(szData, '\0', sizeof(szData));
  memset(szActData, '\0', sizeof(szActData));

  ulStart_Timer = millis();

  do
  {
    sDataLen = sRead_Serial_Data(szData, sizeof(szData));

    if(sDataLen > 0)
    {
      //Serial.println(szData);
      for(short i=0; i<sDataLen; i++)
      {
        if(szData[i] == '!')
        {
          for(k=0; k<4; k++)
          {
            szActData[k] = szData[i+k];
          }
          break;
        }
      }

      //Serial.print("szActData: ");
      //Serial.println(szActData);
  
      if(strncmp(szActData, "!RST", 4) == 0)
      {
        usStatus = OK;
        break;
      }
      else
      {
        usStatus = NOK;
        break;
      }
    }
  }while(millis() - ulStart_Timer < READ_DATA_TIMEOUT); //excess 2s means fail

  m_puart->flush();

  #ifdef DBLYNK_PRINT_H
    if(usStatus == OK)
    {
      m_DebugUart->println(F("OK")); 
    }
    else
    {
      m_DebugUart->println(F("NOK")); 
      m_DebugUart->println(F("Fail to reset ESP, Reset manually by pressing RESET button..."));
    }
  #endif

  delay(500);

  return usStatus;
}

short DBlynk::sSet_Blynk_TempID(String TempID)
{
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szData[5];
  unsigned long ulStart_Timer = 0;
  
  //Blynk Template ID
  #ifdef DBLYNK_PRINT_H
    m_DebugUart->print(F("Blynk Template ID..."));   
  #endif

  if(TempID.length() > 0)
  {
    m_puart->print(F("!TEMP_ID-"));
    m_puart->print(TempID);
    m_puart->println(F(","));
  
    ulStart_Timer = millis();
  
    memset(szData, '\0', sizeof(szData));
  
    do
    {
      short sByte = sRead_Serial_Data(szData, sizeof(szData));
  
      if(sByte > 0)
      {
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  
    m_puart->flush();
  }

  #ifdef DBLYNK_PRINT_H
    if(usStatus == OK)
    {
      m_DebugUart->println(F("OK")); 
    }
    else
    {
      m_DebugUart->println(F("NOK")); 
    }  
  #endif  

  return usStatus;
}

short DBlynk::sSet_Blynk_DeviceName(String DeviceName)
{
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szData[5];
  unsigned long ulStart_Timer = 0;
  
  //Blynk Device Name
  #ifdef DBLYNK_PRINT_H
    m_DebugUart->print(F("Blynk Device Name..."));   
  #endif

  if(DeviceName.length() > 0)
  {
    m_puart->print(F("!DE_NAME-"));
    m_puart->print(DeviceName);
    m_puart->println(F(","));
  
    ulStart_Timer = millis();
  
    memset(szData, '\0', sizeof(szData));
  
    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  
    m_puart->flush();
  }

  #ifdef DBLYNK_PRINT_H
    if(usStatus == OK)
    {
      m_DebugUart->println(F("OK")); 
    }
    else
    {
      m_DebugUart->println(F("NOK")); 
    }  
  #endif  

  return usStatus;
}

short DBlynk::sSet_Blynk_SSID(String ssid)
{
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szData[5];
  unsigned long ulStart_Timer = 0;
  
  //Blynk Device Name
  #ifdef DBLYNK_PRINT_H
    m_DebugUart->print(F("SSID..."));   
  #endif

  if(ssid.length() > 0)
  {
    m_puart->print(F("!SSID-"));
    m_puart->print(ssid);
    m_puart->println(F(","));
  
    ulStart_Timer = millis();
  
    memset(szData, '\0', sizeof(szData));
  
    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));

  
    m_puart->flush();
  }

  #ifdef DBLYNK_PRINT_H
    if(usStatus == OK)
    {
      m_DebugUart->println(F("OK")); 
    }
    else
    {
      m_DebugUart->println(F("NOK")); 
    }  
  #endif  

  return usStatus;
}

short DBlynk::sSet_Blynk_Pass(String pass)
{
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szData[5];
  unsigned long ulStart_Timer = 0;
  
  //Blynk Device Name
  #ifdef DBLYNK_PRINT_H
    m_DebugUart->print(F("Password..."));   
  #endif

  if(pass.length() > 0)
  {
    m_puart->print(F("!PASS-"));
    m_puart->print(pass);
    m_puart->println(F(","));
  
    ulStart_Timer = millis();
  
    memset(szData, '\0', sizeof(szData));
  
    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  
    m_puart->flush();
  }

  #ifdef DBLYNK_PRINT_H
    if(usStatus == OK)
    {
      m_DebugUart->println(F("OK")); 
    }
    else
    {
      m_DebugUart->println(F("NOK")); 
    }  
  #endif  

  return usStatus;
}

short DBlynk::sSet_Blynk_AuthToken(String auth)
{
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szData[5];
  unsigned long ulStart_Timer = 0;
  
  //Blynk Device Name
  #ifdef DBLYNK_PRINT_H
    m_DebugUart->print(F("Blynk Auth Token..."));   
  #endif

  if(auth.length() > 0)
  {
    m_puart->print(F("!TOKEN-"));
    m_puart->print(auth);
    m_puart->println(F(","));
  
    ulStart_Timer = millis();
  
    memset(szData, '\0', sizeof(szData));
  
    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  
    m_puart->flush();
  }

  #ifdef DBLYNK_PRINT_H
    if(usStatus == OK)
    {
      m_DebugUart->println(F("OK")); 
    }
    else
    {
      m_DebugUart->println(F("NOK")); 
    }  
  #endif  

  //print Blynk message
  while(1)
  {
    if(m_puart->available())
    {
      char cData = m_puart->read();

      if(cData == '*')
      {
        break;  
      }
      else
      {
        m_DebugUart->print(cData);  
      }
    }
  }

  return usStatus;
}

short DBlynk::setEspBaudRate(short sBaudRate)
{
  unsigned short usStatus = OK;
  unsigned short usExit = 0;
  char szData[5]; 
  unsigned long ulStart_Timer = 0;

  if(m_BlynkRunning == 0)
  {
    usStatus = resetEsp();
  }

  if(usStatus == OK)
  {
    m_puart->print(F("!B_RATE-"));
    m_puart->print(sBaudRate);
    m_puart->println(F(","));
  
    ulStart_Timer = millis();
  
    memset(szData, '\0', sizeof(szData));

    usStatus = NOK;
  
    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  
    m_puart->flush();
  }

  delay(50);

  return usStatus;
}

String DBlynk::getEspFirmwareVersion(void)
{
  String szData = "NA";
  unsigned long ulStart_Timer = 0;   

  m_puart->println(F("!V"));

  ulStart_Timer = millis();

  do
  {
    if(m_puart->available())
    {
      szData = m_puart->readString();
    }
  }while(millis() - ulStart_Timer < READ_DATA_TIMEOUT); 

  return szData;
}

String DBlynk::getMacAddress(void)
{
  String szData = "NA";
  unsigned long ulStart_Timer = 0;   

  m_puart->println("!MAC");

  ulStart_Timer = millis();

  do
  {
    if(m_puart->available())
    {
      szData = m_puart->readString();
    }
  }while(millis() - ulStart_Timer < READ_DATA_TIMEOUT); 

  return szData;
}

void DBlynk::run(void)
{
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;

  if(millis() - gulStartCheckConnectionTimer >= READ_DATA_TIMEOUT*2) //1 seconds checking blynk connection
  {
    m_puart->print(F("!BC"));
    gulStartCheckConnectionTimer = millis();

    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {        
        if(strncmp(szData, "OK", 2) == 0)
        {
          usStatus = OK;  
        }
        else
        {
          usStatus = NOK; 
        }
        usExit = 1;
      }
          
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));

    if(usStatus == OK)
    {
      m_BlynkConnected = 1;
    }
    else
    {
      #ifdef DBLYNK_PRINT_H
        m_DebugUart->println(F("No Connection!"));         
      #endif 
      m_BlynkConnected = 0;
    }
  }
  
  {
    while(m_puart->available())
    {
      szData[0] = m_puart->read();
      if(szData[0] == '!')
      {
        delay(3);
        szData[1] = m_puart->read();
        delay(3);
        szData[2] = m_puart->read();
        delay(3);
        szData[3] = m_puart->read();
      }
  
      //m_DebugUart->print(char(szData[0]));
    }
   
    if(strncmp(szData, "!RST", 4) == 0)
    {
      #ifdef DBLYNK_PRINT_H
        m_DebugUart->println(F("Esp8266 reset!"));         
      #endif 
      m_BlynkConnected = 0;
    }
  }
}

short DBlynk::connected(void)
{
  return m_BlynkConnected;  
}

void DBlynk::virtualWrite(short sVpin, double dValue1)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2, double dValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2, String szValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2, double dValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2, String szValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2, double dValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2, String szValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2, double dValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2, String szValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2, double dValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDDDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2, double dValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDDDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2, String szValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDDSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, double dValue2, String szValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDDSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2, double dValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDSDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2, double dValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDSDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2, String szValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDSSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, double dValue1, String szValue2, String szValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WDSSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(dValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2, double dValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSDDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2, double dValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSDDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2, String szValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSDSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, double dValue2, String szValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSDSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2, double dValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSSDD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2, double dValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSSDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(dValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2, String szValue3, double dValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSSSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(dValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualWrite(short sVpin, String szValue1, String szValue2, String szValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WSSSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::setProperty(short sVpin, String szValue1, String szValue2)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!SPSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::setProperty(short sVpin, String szValue1, double dValue2)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!SPSD-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::setProperty(short sVpin, String szValue1, String szValue2, String szValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!SPSSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::setProperty(short sVpin, String szValue1, double dValue2, String szValue3)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!SPSDS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(dValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::setProperty(short sVpin, String szValue1, String szValue2, String szValue3, String szValue4)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!SPSSSS-"));
    m_puart->print(sVpin);
    m_puart->print(F("\\"));
    m_puart->print(szValue1);
    m_puart->print(F("\\"));
    m_puart->print(szValue2);
    m_puart->print(F("\\"));
    m_puart->print(szValue3);
    m_puart->print(F("\\"));
    m_puart->print(szValue4);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::notify(String szMsg)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!NOTIFY-"));
    m_puart->print(szMsg);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::email(String szEmail_Add, String szEmail_Subject, String szEmail_Msg)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!EMAIL-"));
    m_puart->print(szEmail_Add);
    m_puart->print(F("\\"));
    m_puart->print(szEmail_Subject);
    m_puart->print(F("\\"));
    m_puart->print(szEmail_Msg);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::logEvent(String szEvent_Name, String szEvent_Description)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!EVENT-"));
    m_puart->print(szEvent_Name);
    m_puart->print(F("\\"));
    m_puart->print(szEvent_Description);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

int DBlynk::read_asInt(short sVirtualPin, short sDataArray = 0)
{
  int iData = -99;
  char szData[20];
  char szDummyData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WP-"));
    m_puart->print(sVirtualPin);
    m_puart->print(F("\\"));
    m_puart->print(sDataArray);
    m_puart->println(F("\\"));

    memset(szData, '\0', sizeof(szData));
    memset(szDummyData, '\0', sizeof(szDummyData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!V-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!V-%[^\\]\\\n", szDummyData);
          iData = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iData;
}

float DBlynk::read_asFloat(short sVirtualPin, short sDataArray = 0)
{
  float fData = -99.99;
  char szData[20];
  char szDummyData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WP-"));
    m_puart->print(sVirtualPin);
    m_puart->print(F("\\"));
    m_puart->print(sDataArray);
    m_puart->println(F("\\"));

    memset(szData, '\0', sizeof(szData));
    memset(szDummyData, '\0', sizeof(szDummyData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!V-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!V-%[^\\]\\\n", szDummyData);
          fData = atof(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return fData;
}

double DBlynk::read_asDouble(short sVirtualPin, short sDataArray = 0)
{
  double dData = -99.99;
  char szData[20];
  char szDummyData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WP-"));
    m_puart->print(sVirtualPin);
    m_puart->print(F("\\"));
    m_puart->print(sDataArray);
    m_puart->println(F("\\"));

    memset(szData, '\0', sizeof(szData));
    memset(szDummyData, '\0', sizeof(szDummyData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!V-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!V-%[^\\]\\\n", szDummyData);
          dData = strtod(szDummyData, NULL);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  

  return dData;
}

String DBlynk::read_asStr(short sVirtualPin, short sDataArray = 0)
{
  char szData[50];
  char szDummyData[50];
  //String strData;
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;

  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!WZ-"));
    m_puart->print(sVirtualPin);
    m_puart->print(F("\\"));
    m_puart->print(sDataArray);
    m_puart->println(F("\\"));
    
    memset(szData, '\0', sizeof(szData));
    memset(szDummyData, '\0', sizeof(szDummyData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
    
        if(strncmp(szData, "!V-", 3) == 0)
        {
          sscanf(szData, "!V-%[^\\]\\\n", szDummyData);          
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return szDummyData;
}

void DBlynk::terminal_print(short sVirtualPin, String szData)
{
  if(m_BlynkConnected == 1)
  {
    virtualWrite(sVirtualPin, szData);
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::terminal_println(short sVirtualPin, String szData)
{
  if(m_BlynkConnected == 1)
  {
    virtualWrite(sVirtualPin, szData+'\n');
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::terminal_println(short sVirtualPin)
{
  if(m_BlynkConnected == 1)
  {
    virtualWrite(sVirtualPin, "\n");
  
    delay(GENERAL_DELAY);
  }
}

void DBlynk::virtualClear(short sVirtualPin)
{
  if(m_BlynkConnected == 1)
  {
    m_puart->print(F("!TC-"));
    m_puart->print(sVirtualPin);
    m_puart->println(F("\\"));
  
    delay(GENERAL_DELAY);
  }
}

int DBlynk::hour(void)
{
  int iHour = -1;
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szDummyData[10];
  
  memset(szDummyData, '\0', sizeof(szDummyData));
  
  if(m_BlynkConnected == 1)
  {
    m_puart->println(F("!HOUR"));

    memset(szData, '\0', sizeof(szData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!H-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!H-%[^\\]\\\n", szDummyData);
          iHour = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iHour;
}

int DBlynk::minute(void)
{
  int iMin = -1;
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szDummyData[10];
  
  memset(szDummyData, '\0', sizeof(szDummyData));
  
  if(m_BlynkConnected == 1)
  {
    m_puart->println(F("!MIN"));

    memset(szData, '\0', sizeof(szData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!M-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!M-%[^\\]\\\n", szDummyData);
          iMin = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iMin;
}

int DBlynk::second(void)
{
  int iSec = -1;
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szDummyData[10];
  
  memset(szDummyData, '\0', sizeof(szDummyData));
  
  if(m_BlynkConnected == 1)
  {
    m_puart->println(F("!SEC"));

    memset(szData, '\0', sizeof(szData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!S-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!S-%[^\\]\\\n", szDummyData);
          iSec = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iSec;
}

int DBlynk::day(void)
{
  int iDay = -1;
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szDummyData[10];
  
  memset(szDummyData, '\0', sizeof(szDummyData));
  
  if(m_BlynkConnected == 1)
  {
    m_puart->println(F("!DAY"));

    memset(szData, '\0', sizeof(szData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!D-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!D-%[^\\]\\\n", szDummyData);
          iDay = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iDay;
}

int DBlynk::month(void)
{
  int iMonth = -1;
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szDummyData[10];

  memset(szDummyData, '\0', sizeof(szDummyData));
  
  if(m_BlynkConnected == 1)
  {
    m_puart->println(F("!MON"));

    memset(szData, '\0', sizeof(szData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!M-", 3) == 0)
        {
          //Serial.println(szData);
          sscanf(szData, "!M-%[^\\]\\\n", szDummyData);
          iMonth = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iMonth;
}

int DBlynk::year(void)
{
  int iYear = -1;
  char szData[20];
  unsigned long ulStart_Timer = 0;
  unsigned short usStatus = NOK;
  unsigned short usExit = 0;
  char szDummyData[10];

  memset(szDummyData, '\0', sizeof(szDummyData));
  
  if(m_BlynkConnected == 1)
  {
    m_puart->println(F("!YEAR"));

    memset(szData, '\0', sizeof(szData));
    ulStart_Timer = millis();

    do
    {
      if(sRead_Serial_Data(szData, sizeof(szData)) > 0)
      {
        if(strncmp(szData, "!Y-", 3) == 0)
        {
          //Serial.println(szData);          
          sscanf(szData, "!Y-%[^\\]\\\n", szDummyData);
          iYear = atoi(szDummyData);
          usStatus = OK;
        }
        else
        {
          usStatus = NOK;
        }
        usExit = 1;
      }
      
    }while((usExit == 0) && (millis() - ulStart_Timer < READ_DATA_TIMEOUT));
  }

  return iYear;
}
