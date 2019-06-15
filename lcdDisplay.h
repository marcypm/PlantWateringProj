#ifndef lcdDisplay_h
#define lcdDisplay_h


class dclass
{
  public:
    dclass();
    void SETUP();
    void resetLCD();
    void updateLCD(int mode, int moisture, int thresh, unsigned long lastWater);
    void waterLCD(int mode);
    void emptyTankLCD(int mode, int moisture);
    void justPollLCD(int mode, int moisture);
    void stopLCD(int mode);

};

extern dclass lcd;

#endif
