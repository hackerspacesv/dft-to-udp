void configurarAdcDma() {
  //Compensacion de buffer apagada
  ADC->REFCTRL.bit.REFCOMP = 0;
  //Referencia de ADC a INTVCC1 (1/2 VDDANA = 1.65V)
  ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;

  //Coeficiente de ajuste de resolucion por defecto (0)
  ADC->AVGCTRL.bit.ADJRES = 0;
  //El numero de muestras se establece a solo una:
  ADC->AVGCTRL.bit.SAMPLENUM = ADC_AVGCTRL_SAMPLENUM_1_Val;

  //Tiempo de muestreo a 1/2 clock de ADC
  ADC->SAMPCTRL.bit.SAMPLEN = 0;

  //Divisor de frecuencia a 32. Arduino ide pre-configura el
  //reloj del ADC para usar el reloj generico 0, el cual a su
  //vez esa configurado para usar el PLL de 48MHz.
  //Fclkadc = 48MHz / 32 = 1.5MHz (el maximo es 2.1MHz)
  ADC->CTRLB.bit.PRESCALER = ADC_CTRLB_PRESCALER_DIV32_Val;
  //Resolucion selccionada de 8 bits  
  ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_8BIT_Val;
  //Correccion digital apagada
  ADC->CTRLB.bit.CORREN = 0;
  //Modo de operacion de conversion simple
  ADC->CTRLB.bit.FREERUN = 0;
  //Ajuste a la izquierda apagado (ajusta a la derecha)
  ADC->CTRLB.bit.LEFTADJ = 0;
  //Modo diferencial apagado (usa entradas individuales)
  ADC->CTRLB.bit.DIFFMODE = 0;

  //Modo de comparacion de ventana apagado
  ADC->WINCTRL.bit.WINMODE = ADC_WINCTRL_WINMODE_DISABLE_Val;

  //Ganancia configurada a 1/2 (para usar el rango completo de
  //VDDANA = 3.3V)
  ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;
  //Modo de escaneo de multiples canales apagado
  ADC->INPUTCTRL.bit.INPUTOFFSET = 0;
  ADC->INPUTCTRL.bit.INPUTSCAN = 0;
  //La seleccion de canal negativo se coloca a tierra
  ADC->INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
  //Se preselecciona el canal 0 del ADC
  ADC->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_PIN0_Val;

  ADC->EVCTRL.bit.WINMONEO = 0;
  ADC->EVCTRL.bit.RESRDYEO;
  ADC->EVCTRL.bit.SYNCEI;
  ADC->EVCTRL.bit.STARTEI;

  //Habilita el ADC
  ADC->CTRLA.bit.ENABLE = 1;
}

