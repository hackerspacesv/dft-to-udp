#include <WiFi101.h>
#include <WiFiUdp.h>

#include "dft.h"

char ssid[] = "Tu red aqui";
char pass[] = "Tu clave aqui";
IPAddress dirRemota(192, 168, 2, 99);

WiFiUDP udp;

//60 puntos, 2 repeticiones, 4 bytes por elemento
char bufferTx[nPuntos * 2 * 4];
uint8_t tablaColor[nPuntos * 3];  //3 colores (RGB)

void setup() {
  int estado;

  Serial.begin(9600);
  //while (!Serial);

  precalcularTablas();
  configurarAdcDma();

  do {
    Serial.print("Conectando a la red: ");
    Serial.println(ssid);
    estado = WiFi.begin(ssid, pass);
    if (estado != WL_CONNECTED) delay(1000);
  } while (estado != WL_CONNECTED);
  Serial.println("Conexion exitosa");

  udp.begin(1337);

  //pruebaDft();
}

void loop() {
  int32_t dato;
  uint32_t i;

  generarTablaColor();
  capturarAudio();
  fPoint_dft();
  fPoint_mag();

  if (udp.beginPacket(dirRemota, 1337)) {
    for (i = 0; i < nPuntos; i++) {
      dato = magData[i] / 16;
      if (dato < 32) dato = 0;

      //Primer led de la pareja
      bufferTx[i * 8 + 0] = i * 2;                //Indice
      bufferTx[i * 8 + 1] =
        (int)dato * tablaColor[i * 3 + 0] / 255;  //R
      bufferTx[i * 8 + 2] =
        (int)dato * tablaColor[i * 3 + 1] / 255;  //G
      bufferTx[i * 8 + 3] =
        (int)dato * tablaColor[i * 3 + 2] / 255;  //B
      //bufferTx[i * 8 + 2] = dato;     //Solo verde

      //Segundo led de la pareja
      bufferTx[i * 8 + 4] = i * 2 + 1;
      bufferTx[i * 8 + 5] =
        (int)dato * tablaColor[i * 3 + 0] / 255;  //R
      bufferTx[i * 8 + 6] =
        (int)dato * tablaColor[i * 3 + 1] / 255;  //G
      bufferTx[i * 8 + 7] =
        (int)dato * tablaColor[i * 3 + 2] / 255;  //B
      //bufferTx[i * 8 + 6] = dato;     //Solo verde
    }
    udp.write(bufferTx, sizeof(bufferTx));
    udp.endPacket();
    //Serial.println("Dato enviado");
  }
  else
    Serial.println("Error al conectar");


  //for (uint16_t i = 0; i < nMuestras; i++)
  //  Serial.println(audioData[i]);
  /*
    static bool estado = false;
    if (udp.beginPacket(dirRemota, 1337)) {
      bufferTx[1] = estado? 255: 0;
      bufferTx[2] = estado? 255: 0;
      bufferTx[3] = estado? 255: 0;
      estado = !estado;
      udp.write(bufferTx, 4);
      udp.endPacket();
      Serial.println("Dato enviado");
    }
    else {
      Serial.println("Error al conectar");
    }
    Serial.println(analogRead(0));
    delay(10);
  */
  //delay(10);
}

void capturarAudio() {
  int i;

  unsigned long tIni = micros();
  for (i = 0; i < 512; i++) {
    audioData[i] = analogRead(0) - 128;
    //Serial.println(audioData[i]);
    delayMicroseconds(1.0e6 / 11025.0 - 12);
  }
  unsigned long tFin = micros();
  //Serial.println(tFin - tIni);
}

void generarTablaColor() {
  const unsigned int f1 = nPuntos * 1 / 3;
  const unsigned int f2 = nPuntos * 2 / 3;
  const unsigned int t1 = f1 - 0;
  const unsigned int t2 = f2 - f1;
  const unsigned int t3 = nPuntos - f2;
  int i;

  for (i = 0; i < f1; i++) {
    tablaColor[i * 3 + 0] = (f1 - i) * 255 / t1; //R
    tablaColor[i * 3 + 1] = (i - 0) * 255 / t1;  //G
    tablaColor[i * 3 + 2] = 0;                   //B
  }

  for (i = f1; i < f2; i++) {
    tablaColor[i * 3 + 0] = 0;                   //R
    tablaColor[i * 3 + 1] = (t2 - (i - f1)) * 255 / t2; //G
    tablaColor[i * 3 + 2] = (i - f1) * 255 / t2;  //B
  }

  for (i = f2; i < nPuntos; i++) {
    tablaColor[i * 3 + 0] = (i - f2) * 255 / t3;  //R
    tablaColor[i * 3 + 1] = 0;                   //G
    tablaColor[i * 3 + 2] = (t3 - (i - f2)) * 255 / t3; //B
  }
}

