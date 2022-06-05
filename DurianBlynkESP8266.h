/*
 * DurianBlynkESP8266.h
 *
 * Library for running Blynk in Durian UNO + ESP8266
 * Author: Ng Kwang Chen MYBOTIC @ www.mybotic.com.my
 * Date  : 25 April 2022
 *
 * Copyright reserved
 */

#ifndef DurianBlynkESP8266_h
#define DurianBlynkESP8266_h

#define DBLYNK_PRINT Serial

#ifdef DBLYNK_PRINT
   #define DBLYNK_PRINT_H DBLYNK_PRINT
#endif

#define OK  1
#define NOK 0

#define V0 0
#define V1 1
#define V2 2
#define V3 3
#define V4 4
#define V5 5
#define V6 6
#define V7 7
#define V8 8
#define V9 9
#define V10 10
#define V11 11
#define V12 12
#define V13 13
#define V14 14
#define V15 15
#define V16 16
#define V17 17
#define V18 18
#define V19 19
#define V20 20
#define V21 21
#define V22 22
#define V23 23
#define V24 24
#define V25 25
#define V26 26
#define V27 27
#define V28 28
#define V29 29
#define V30 30
#define V31 31
#define V32 32
#define V33 33
#define V34 34
#define V35 35
#define V36 36
#define V37 37
#define V38 38
#define V39 39
#define V40 40
#define V41 41
#define V42 42
#define V43 43
#define V44 44
#define V45 45
#define V46 46
#define V47 47
#define V48 48
#define V49 49
#define V50 50
#define V51 51
#define V52 52
#define V53 53
#define V54 54
#define V55 55
#define V56 56
#define V57 57
#define V58 58
#define V59 59
#define V60 60
#define V61 61
#define V62 62
#define V63 63
#define V64 64
#define V65 65
#define V66 66
#define V67 67
#define V68 68
#define V69 69
#define V70 70
#define V71 71
#define V72 72
#define V73 73
#define V74 74
#define V75 75
#define V76 76
#define V77 77
#define V78 78
#define V79 79
#define V80 80
#define V81 81
#define V82 82
#define V83 83
#define V84 84
#define V85 85
#define V86 86
#define V87 87
#define V88 88
#define V89 89
#define V90 90
#define V91 91
#define V92 92
#define V93 93
#define V94 94
#define V95 95
#define V96 96
#define V97 97
#define V98 98
#define V99 99
#define V100 100
#define V101 101
#define V102 102
#define V103 103
#define V104 104
#define V105 105
#define V106 106
#define V107 107
#define V108 108
#define V109 109
#define V110 110
#define V111 111
#define V112 112
#define V113 113
#define V114 114
#define V115 115
#define V116 116
#define V117 117
#define V118 118
#define V119 119
#define V120 120
#define V121 121
#define V122 122
#define V123 123
#define V124 124
#define V125 125
#define V126 126
#define V127 127

#define UART_TIMEOUT 5 //in ms
#define GENERAL_DELAY 80 //ms
#define READ_DATA_TIMEOUT 1000 //in ms

class DBlynk {
  public:
    typedef void (*onData)(uint8_t mux_id, uint32_t len, void* ptr);
  
    DBlynk(Stream* uart = &Serial);
    int begin(String TempID, String DeviceName, String auth, String ssid, String pass);
    short resetEsp(void);
    short setEspBaudRate(short sBaudRate);
    String getEspFirmwareVersion(void);
    String getMacAddress(void);
    void run(void);
    short connected();
    
    void virtualWrite(short sVpin, double dValue1);
    void virtualWrite(short sVpin, String szValue1);   
    void virtualWrite(short sVpin, double dValue1, double dValue2);
    void virtualWrite(short sVpin, double dValue1, String szValue2);
    void virtualWrite(short sVpin, String szValue1, double dValue2);
    void virtualWrite(short sVpin, String szValue1, String szValue2);    
    void virtualWrite(short sVpin, double dValue1, double dValue2, double dValue3);
    void virtualWrite(short sVpin, double dValue1, double dValue2, String szValue3);
    void virtualWrite(short sVpin, double dValue1, String szValue2, double dValue3);
    void virtualWrite(short sVpin, double dValue1, String szValue2, String szValue3);
    void virtualWrite(short sVpin, String szValue1, double dValue2, double dValue3);
    void virtualWrite(short sVpin, String szValue1, double dValue2, String szValue3);
    void virtualWrite(short sVpin, String szValue1, String szValue2, double dValue3);
    void virtualWrite(short sVpin, String szValue1, String szValue2, String szValue3);
    void virtualWrite(short sVpin, double dValue1, double dValue2, double dValue3, double dValue4);
    void virtualWrite(short sVpin, double dValue1, double dValue2, double dValue3, String szValue4);
    void virtualWrite(short sVpin, double dValue1, double dValue2, String szValue3, double dValue4);
    void virtualWrite(short sVpin, double dValue1, double dValue2, String szValue3, String szValue4);
    void virtualWrite(short sVpin, double dValue1, String szValue2, double dValue3, double dValue4);
    void virtualWrite(short sVpin, double dValue1, String szValue2, double dValue3, String szValue4);
    void virtualWrite(short sVpin, double dValue1, String szValue2, String szValue3, double dValue4);
    void virtualWrite(short sVpin, double dValue1, String szValue2, String szValue3, String szValue4);
    void virtualWrite(short sVpin, String szValue1, double dValue2, double dValue3, double dValue4);
    void virtualWrite(short sVpin, String szValue1, double dValue2, double dValue3, String szValue4);
    void virtualWrite(short sVpin, String szValue1, double dValue2, String szValue3, double dValue4);
    void virtualWrite(short sVpin, String szValue1, double dValue2, String szValue3, String szValue4);
    void virtualWrite(short sVpin, String szValue1, String szValue2, double dValue3, double dValue4);
    void virtualWrite(short sVpin, String szValue1, String szValue2, double dValue3, String szValue4);
    void virtualWrite(short sVpin, String szValue1, String szValue2, String szValue3, double dValue4);
    void virtualWrite(short sVpin, String szValue1, String szValue2, String szValue3, String szValue4);
    
    void setProperty(short sVpin, String szValue1, String szValue2);
    void setProperty(short sVpin, String szValue1, double dValue2);
    void setProperty(short sVpin, String szValue1, String szValue2, String szValue3);
    void setProperty(short sVpin, String szValue1, double dValue2, String szValue3);
    void setProperty(short sVpin, String szValue1, String szValue2, String szValue3, String szValue4);

    void notify(String szMsg);
    void email(String szEmail_Add, String szEmail_Subject, String szEmail_Msg);
    void logEvent(String szEvent_Name, String szEvent_Description);
    int read_asInt(short sVirtualPin, short sDataArray = 0);
    float read_asFloat(short sVirtualPin, short sDataArray = 0);
    double read_asDouble(short sVirtualPin, short sDataArray = 0);
    String read_asStr(short sVirtualPin, short sDataArray = 0);

    void terminal_print(short sVirtualPin, String szData);
    void terminal_println(short sVirtualPin, String szData);
    void terminal_println(short sVirtualPin);
    void virtualClear(short sVirtualPin);

    int hour(void);
    int minute(void);
    int second(void);
    int day(void);
    int month(void);
    int year(void);
        
  private:
    short gsShowSerialPrint = -1;
    Stream *m_puart; /* The UART to communicate with ESP8266 */
    Stream *m_DebugUart; /* The UART to communicate with ESP8266 */
    unsigned long gultimeout = 0;
    unsigned long gulStartCheckConnectionTimer = 0;
    char gszSerialData[50];
    unsigned short gusSerialByteIndex = 0;
    unsigned short m_BlynkRunning = 0;
    unsigned short m_BlynkConnected = 0;
    short gsWidgetVirtualPin = -1;
    char gszWidgetData[20];

    short sRead_Serial_Data(char *szData, unsigned short usSize);
    short sSet_Blynk_TempID(String TempID);
    short sSet_Blynk_DeviceName(String DeviceName);
    short sSet_Blynk_SSID(String ssid);
    short sSet_Blynk_Pass(String pass);
    short sSet_Blynk_AuthToken(String auth);
    
};

#endif // DurianBlynkESP8266_h
