#ifndef ADA_H
#define ADA_H

#include <Arduino.h>

class Ada {
  private:
    String dextro;
    int trend;
    String time;

  public:
    Ada(String d, int t, String tm);
    
    // Métodos para obter os valores dos atributos
    String getDextro();
    int getTrend();
    String getTime();
    
    // Métodos para definir os valores dos atributos
    void setDextro(String d);
    void setTrend(int t);
    void setTime(String tm);
};

#endif

