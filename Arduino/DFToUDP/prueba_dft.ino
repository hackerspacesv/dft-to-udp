void pruebaDft() {
  for (int i = 0; i < nMuestras; i++) {
    audioData[i] = round(-cos(1.0 * 2.0 * PI * float(i) / float(nMuestras)) * 256.0);
    //audioData[i] = (i >= nMuestras / 2)? 255: 0;
  }

  unsigned long tIni = micros();
  fPoint_dft();
  fPoint_mag();
  unsigned long tFin = micros();
  SerialUSB.print("Tiempo DFT: ");
  SerialUSB.println(tFin - tIni);

  for (int i = 0; i < nPuntos; i++) {
    f_point_printComplex(&dftData[i]);
    SerialUSB.print(" | ");
    fPoint_printReal(magData[i]);
    SerialUSB.println();
  }
}

void f_point_printComplex(COMPLEJO *num) {
  SerialUSB.print(float(num->re) / 256.0);
  SerialUSB.print(" + j");
  SerialUSB.print(float(num->im) / 256.0);
}

void fPoint_printReal(int32_t num) {
  SerialUSB.print(float(num) / 256.0);
}

