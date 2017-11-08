#ifndef DFT_H
#define DFT_H

const uint32_t nMuestras = 512; //Solo potencias de 2
const uint32_t nPuntos = 60;    //Puntos a computar para la DFT
const uint32_t puntoIni = 1;    //Primera componente de frec.

typedef struct {
  int32_t re;
  int32_t im;
} COMPLEJO;

extern int32_t audioData[];
extern COMPLEJO dftData[];
extern int32_t magData[nPuntos];

void precalcularTablas();
void fPoint_dft();
void fPoint_mag();

#endif //DFT_H

