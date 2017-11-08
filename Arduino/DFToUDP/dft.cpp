#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "dft.h"

//Constantes de configuracion general
const uint32_t mascMuest = nMuestras - 1; //Mascara para modulo
const uint32_t nIterCordic = 16;  //Num. de iteraciones CORDIC

//Constantes precomputadas en tiempo de ejecucion
int32_t tSen[nMuestras];  //Tabla de seno
int32_t tCos[nMuestras];  //Tabla de coseno
int32_t ganCordicInv;     //Ganancia cordic (inversa)

//Bufferes de datos de trabajo
int32_t audioData[nMuestras];   //Datos en el dominio del tiempo
COMPLEJO dftData[nPuntos];      //Datos en el dominio de la frec.
int32_t magData[nPuntos];       //Magnitudes dominio frecuencia.

//Macro utilizada para hacer multiplicaciones de punto fijo
#define FPOINT_MULT(a, b) ((a * b) >> 8)

//Declaracion previa de funciones locales
static int32_t fPoint_cordicMag(const COMPLEJO *c);

void precalcularTablas() {
  uint32_t i;
  float val;
  float ganCordic = 1.0;

  //Precomputa las tablas de seno y coseno
  for (i = 0; i < nMuestras; i++) {
    //Calcula el valor del seno en formato float
    val = sin(-2.0 * M_PI * float(i) / float(nMuestras));
    //Lo convierte a punto fijo y almacena
    tSen[i] = round(val * 256.0);

    //Similar para el coseno
    val = cos(-2.0 * M_PI * float(i) / float(nMuestras));
    tCos[i] = round(val * 256.0);
  }

  //Calcula la ganancia cordic para el numero de iteraciones
  //configurado en formato float
  for (i = 0; i < nIterCordic; i++)
    //Agrega a la productoria las magnitudes de cada operacion
    //de rotacion de angulo
    ganCordic *= sqrt(1.0 + pow(2.0, 2.0 * -float(i)));

  //Calcula la ganancia inversa y la almacena en punto fijo
  ganCordicInv = round(1.0 / ganCordic * 256);
}

void fPoint_dft() {
  uint32_t i, j;
  uint32_t angulo;
  int32_t real, img;

  //Itera para todos los valores deseados en el dominio de la
  //frecuencia
  for (j = puntoIni; j < puntoIni + nPuntos; j++) {
    //Inicializa los acumuladores (integradores) a cero
    real = 0;
    img = 0;

    //Itera para todas las muestras en el dominio del tiempo
    for (i = 0; i < nMuestras; i++) {
      //Computa el angulo de forma modular
      angulo = (i * j) & mascMuest;
      //Acumula los productos de los datos de entrada por las
      //funciones trigonometricas
      real += FPOINT_MULT(audioData[i], tCos[angulo]);
      img += FPOINT_MULT(audioData[i], tSen[angulo]);
    }
    //Almacena los resultados en el arreglo de salida
    dftData[j - puntoIni].re = real;
    dftData[j - puntoIni].im = img;
  }

  //asm("ldr r0, 5");
}

void fPoint_mag() {
  uint32_t i;

  //Computa las magnitudes para todos los datos en el dominio de
  //la frecuencia
  for (i = 0; i < nPuntos; i++) {
    magData[i] = fPoint_cordicMag(&dftData[i]);
  }
}

static int32_t fPoint_cordicMag(const COMPLEJO *c) {
  uint32_t i;
  COMPLEJO valOrig;
  COMPLEJO valNuevo;

  //Toma el valor original asegurandose que la parte real sea
  //siempre positiva (el fasor nunca apunta a la izquierda)
  valOrig.re = abs(c->re);
  valOrig.im = c->im;

  //Realiza el proceso de aproximacion de fase segun la cantidad
  //de iteraciones configurada
  for (i = 0; i < nIterCordic; i++) {
    if (valOrig.im < 0) {
      //Si la fase es negativa rota a la izquierda
      valNuevo.re = valOrig.re - (valOrig.im >> i);
      valNuevo.im = valOrig.im + (valOrig.re >> i);
    }
    else {
      //Si la fase es positiva rota a la derecha
      valNuevo.re = valOrig.re + (valOrig.im >> i);
      valNuevo.im = valOrig.im - (valOrig.re >> i);
    }
    //Suplanta el valor original para la siguiente iteracion
    valOrig.re = valNuevo.re;
    valOrig.im = valNuevo.im;
  }

  //Tras la aproximacion el fasor esta casi sobre el eje
  //horizontal positivo y la magnitud se aproxima a la parte real
  return FPOINT_MULT(valOrig.re, ganCordicInv);
}

