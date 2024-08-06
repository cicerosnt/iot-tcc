#include "Ada.h"

Ada::Ada(String d, int t, String tm) {
  dextro = d;
  trend = t;
  time = tm;
}

String Ada::getDextro() {
  return dextro;
}

int Ada::getTrend() {
  return trend;
}

String Ada::getTime() {
  return time;
}

void Ada::setDextro(String d) {
  dextro = d;
}

void Ada::setTrend(int t) {
  trend = t;
}

void Ada::setTime(String tm) {
  time = tm;
}

