//Se usará atmega 88P y programado a 4 MHz oscilador interno
//Las salidas van a un modulo rele, se activa con LOW
//No se aplica un contador a los tiempos de las horas programadas debido a que genera problemas de desface en los tiempos
//Se usa horas estáticas, no hay problemas de desface en los tiempos
#include <TM1637Display.h>

#define CLK 5
#define DIO 6

#define botonCero 11
#define botonUno 8
#define botonDos 9
#define botonTres 10
#define menu 12
#define subMenu 13
#define sensor 7
int sumaSensor = 0;
int entradaSensorNew = 0;
int entradaSensorOld = 0;

const int pinesSalida[] = {A3, A2, A1, A0};

byte estadoCero = 0;
byte estadoUno = 0;
byte estadoDos = 0;
byte estadoTres = 0;

int estadoActivacion[4];

byte botonCeroOld = 1;
byte botonUnoOld = 1;
byte botonDosOld = 1;
byte botonTresOld = 1;
byte menuOld = 1;
byte subMenuOld = 1;

byte botonCeroNew;
byte botonUnoNew;
byte botonDosNew;
byte botonTresNew;
byte menuNew;
byte subMenuNew;

byte numero = 0;
byte num = 0;
byte num1 = 0;
byte num2 = 0;
byte num3 = 0;
byte num4 = 0;

byte tiempoAnterior = 0;
byte hora = 0;
byte minuto = 0;
byte segundo = 0;

int cronograma[4][4][2];


unsigned long tiempoInicioParpadeo;
unsigned long tiempoFinParpadeo = 0;


TM1637Display display(CLK, DIO);

const uint8_t programA[] = {
  SEG_A | SEG_B | SEG_G | SEG_F | SEG_E | SEG_C
};
const uint8_t programC[] = {
  SEG_A | SEG_F | SEG_E | SEG_D
};
const uint8_t programE[] = {
  SEG_A | SEG_G | SEG_F | SEG_E | SEG_D
};
const uint8_t programF[] = {
  SEG_A | SEG_F | SEG_E | SEG_G
};
const uint8_t program_[] = {
  SEG_D
};

const uint8_t On[] = {
  SEG_A | SEG_F | SEG_E | SEG_D |SEG_C | SEG_B, 
  SEG_G | SEG_E | SEG_C
};
const uint8_t OF[] = {
  SEG_A | SEG_F | SEG_E | SEG_D |SEG_C | SEG_B, 
  SEG_A | SEG_F | SEG_E | SEG_G
};
const uint8_t horas[] = {
  SEG_B
};
const uint8_t minutos[] = {
  SEG_F | SEG_B
};
const uint8_t programarHora[] = {
  SEG_A | SEG_B |SEG_G | SEG_F | SEG_E,
  SEG_F | SEG_B |SEG_G | SEG_C | SEG_E,
  SEG_B,
  SEG_F | SEG_B
};
int candado[4];

byte displayVisible = 1;

int arrayTiemposParpadeoHIGH[4][40];
int arrayTiemposParpadeoLOW[4][40];
int arrayVariablesPorSalida[4][8];

void setup()
{
  display.setBrightness(3);
  display.showNumberDecEx(0, 0b00000000, true, 2, 0);
  display.showNumberDec(0, true, 2, 2);

  pinMode(botonCero, INPUT);
  pinMode(botonUno, INPUT);
  pinMode(botonDos, INPUT);
  pinMode(botonTres, INPUT);
  pinMode(menu, INPUT);
  pinMode(subMenu, INPUT);
  pinMode(sensor, INPUT); 

  attachInterrupt(digitalPinToInterrupt(2), pulso, RISING);// señal de oscilador de cristal 32768 KHz con divisor cd4060

  for (int i = 0; i < 4; i++) {
    candado[i] = 0;
    estadoActivacion[i] = 0;
    digitalWrite(pinesSalida[i], 1);
    pinMode(pinesSalida[i], OUTPUT);
  }
}
void pulso(){
  segundo++;
}
void subir_bajar_horas(){
  botonDosNew = digitalRead(botonDos);
  if(botonDosNew == 1 && botonDosOld == 0){
    
    cronograma[num][num1-1][0]++;
    cronograma[num][num1-1][0] = cronograma[num][num1-1][0] % 24;
    display.showNumberDec(cronograma[num][num1-1][0], true, 2, 2);
  } 
  botonDosOld = botonDosNew;

  botonTresNew = digitalRead(botonTres);
  if(botonTresNew == 1 && botonTresOld == 0){
    if (cronograma[num][num1-1][0] > 0) {
      cronograma[num][num1-1][0]--;
    } else {
      cronograma[num][num1-1][0] = 23;  // Si cronograma[num][num1-1][0 es 0, establecerlo a 23
    }
    display.showNumberDec(cronograma[num][num1-1][0], true, 2, 2);
  } 
  botonTresOld = botonTresNew;
}
void subir_bajar_minutos(){
  botonDosNew = digitalRead(botonDos);
  if(botonDosNew == 1 && botonDosOld == 0){
    cronograma[num][num1-1][1]++;
    cronograma[num][num1-1][1] = cronograma[num][num1-1][1] % 60;
    display.showNumberDec(cronograma[num][num1-1][1], true, 2, 2);
  } 
  botonDosOld = botonDosNew;

  botonTresNew = digitalRead(botonTres);
  if(botonTresNew == 1 && botonTresOld == 0){
    if (cronograma[num][num1-1][1] > 0) {
      cronograma[num][num1-1][1]--;
    } else {
      cronograma[num][num1-1][1] = 59;  // Si cronograma[num][num1-1][1] es 0, establecerlo a 23
    }
    display.showNumberDec(cronograma[num][num1-1][1], true, 2, 2);
  } 
  botonTresOld = botonTresNew;
}
void temporizador(byte valor){//Aqui asignamos las horas programadas
  botonCeroNew = digitalRead(botonCero);
  if(botonCeroNew == 1 && botonCeroOld == 0){
    num2++;
    num2 = num2 % 3;
  } 
  botonCeroOld = botonCeroNew;

  switch(num2){
    case 0:
      display.showNumberDec((valor - 1), false, 1, 1);
      display.showNumberDec(0, true, 2, 2);
    break;
    case 1://Temporizar horas
      display.setSegments(horas, 1, 1);
      display.showNumberDec(cronograma[num][num1-1][0], true, 2, 2);
      subir_bajar_horas();
    break;
    case 2://Temporizar minutos
      display.setSegments(minutos, 1, 1);
      display.showNumberDec(cronograma[num][num1-1][1], true, 2, 2);
      subir_bajar_minutos();
    break;
  }
}
void funcionSubMenu(){
      subMenuNew = digitalRead(subMenu);
      if(subMenuNew == 1 && subMenuOld == 0){
        num1++;
        num1 = num1 % 5;
        num2 = 0;
      }
      subMenuOld = subMenuNew;

      switch(num1){
        case 0://Determinamos la activacion del tiempo de actividad con On y lo desactivamos con OF, por defecto empieza con OF
          botonCeroNew = digitalRead(botonCero);
          if(botonCeroNew == 1 && botonCeroOld == 0){
            if(estadoActivacion[num] == 0){
              estadoActivacion[num] = 1;//Con valor 1 se activa 
              candado[num] = 0;
            }else{
              estadoActivacion[num] = 0;//Con valor 0 se desactiva 
              candado[num] = 1;
              digitalWrite(pinesSalida[num], 1);
            }
          }
          botonCeroOld = botonCeroNew;
          if(estadoActivacion[num] == 0){
            display.setSegments(OF, 2, 2);
          }else{
            display.setSegments(On, 2, 2);
          }
          display.setSegments(program_, 1, 1);
        break;
        case 1://Inicio tiempo de actividad
          temporizador(num1);
        break;
        case 2://Fin tiempo de actividad
          temporizador(num1);
        break;
        case 3://Duración de tiempo encendido dentro de tiempo de actividad
          temporizador(num1);
        break;
        case 4://Duración de tiempo apagado dentro de tiempo de actividad
          temporizador(num1);
        break;
      }
}
//arrayVariablesPorSalida[n_s][0] ==> 0 periodoTotalHIGH
//arrayVariablesPorSalida[n_s][1] ==> 1 numVecesEnPeriodoHIGH
//arrayVariablesPorSalida[n_s][2] ==> 2 periodoTotalLOW
//arrayVariablesPorSalida[n_s][3] ==> 3 numVecesEnPeriodoLOW
//arrayVariablesPorSalida[n_s][4] ==> 4 variableControlUno
//arrayVariablesPorSalida[n_s][5] ==> 5 variableControlDos
//arrayVariablesPorSalida[n_s][6] ==> 6 numTiempoHIGH
//arrayVariablesPorSalida[n_s][7] ==> 7 numTiempoLOW
void procesadoTiempos(byte n_s){

  arrayVariablesPorSalida[n_s][0] = ((cronograma[n_s][1][0] * 60) + cronograma[n_s][1][1]) - ((cronograma[n_s][0][0] * 60) + cronograma[n_s][0][1]);//en minutos
  arrayVariablesPorSalida[n_s][1] = arrayVariablesPorSalida[n_s][0] / ((((cronograma[n_s][2][0] * 60) + cronograma[n_s][2][1])) + (((cronograma[n_s][3][0] * 60) + cronograma[n_s][3][1])));
  arrayVariablesPorSalida[n_s][6] = 0;

  arrayVariablesPorSalida[n_s][2] =  ((cronograma[n_s][1][0] * 60) + cronograma[n_s][1][1]) - ((((cronograma[n_s][0][0] * 60) + cronograma[n_s][0][1]) + ((cronograma[n_s][2][0] * 60) + cronograma[n_s][2][1])));//en minutos
  arrayVariablesPorSalida[n_s][3] = arrayVariablesPorSalida[n_s][2] / ((((cronograma[n_s][2][0] * 60) + cronograma[n_s][2][1])) + (((cronograma[n_s][3][0] * 60) + cronograma[n_s][3][1])));
  arrayVariablesPorSalida[n_s][7] = 0;              

  for (int i = 0; i <= arrayVariablesPorSalida[n_s][1] && arrayVariablesPorSalida[n_s][4]; i++){//Tomar en cuenta que se aumenta un dato inicial numVecesEnPeridoHIGH + 1
    if(i == 0){
      arrayTiemposParpadeoHIGH[n_s][i] = ((cronograma[n_s][0][0] * 60) + cronograma[n_s][0][1]);
    }else{
      arrayTiemposParpadeoHIGH[n_s][i] = ((cronograma[n_s][0][0] * 60) + cronograma[n_s][0][1]) + (((((cronograma[n_s][2][0] * 60) + cronograma[n_s][2][1])) + (((cronograma[n_s][3][0] * 60) + cronograma[n_s][3][1]))) * i);
    }
    if(arrayVariablesPorSalida[n_s][1] + 1 == i ){//+ 1 dato inicial si i == 0
      arrayVariablesPorSalida[n_s][4] = false;
    }
  }
  for (int i = 0; i <= arrayVariablesPorSalida[n_s][3] && arrayVariablesPorSalida[n_s][5]; i++){//Tomar en cuenta que se aumenta un dato inicial numVecesEnPeridoLOW + 1
    if(i == 0){
      arrayTiemposParpadeoLOW[n_s][i] = (((cronograma[n_s][0][0] * 60) + cronograma[n_s][0][1]) + ((cronograma[n_s][2][0] * 60) + cronograma[n_s][2][1]));
    }else{
      arrayTiemposParpadeoLOW[n_s][i] = (((cronograma[n_s][0][0] * 60) + cronograma[n_s][0][1]) + ((cronograma[n_s][2][0] * 60) + (cronograma[n_s][2][1])) + ((((cronograma[n_s][2][0] * 60) + cronograma[n_s][2][1])) + (((cronograma[n_s][3][0] * 60) + cronograma[n_s][3][1]))) * i);
    }
    if(arrayVariablesPorSalida[n_s][3] + 1 == i ){//+ 1 dato inicial si i == 0
      arrayVariablesPorSalida[n_s][5] = false;
    }
  }
}
void ejecucionDeTiempos(byte l_s, byte _salida){
  if (((hora * 60) + minuto) >= ((cronograma[l_s][0][0] * 60) + cronograma[l_s][0][1]) &&
  ((hora * 60) + minuto) < ((cronograma[l_s][1][0] * 60) + cronograma[l_s][1][1])){// se mide en mitutos
    if(candado[l_s] == 0){
      candado[l_s] = 1;
    }
    if (((hora * 60) + minuto) >= arrayTiemposParpadeoHIGH[l_s][arrayVariablesPorSalida[l_s][6]]){// 00 + 00 : 15 + 30 = 00 : 45 -> + 30 
      digitalWrite(_salida, 0);
      arrayVariablesPorSalida[l_s][6] ++;
    }
    if (((hora * 60) + minuto) >= arrayTiemposParpadeoLOW[l_s][arrayVariablesPorSalida[l_s][7]]){// 00 + 00 : 15 + 30 = 00 : 45 -> + 30 
      digitalWrite(_salida, 1);
      arrayVariablesPorSalida[l_s][7] ++;
    }
  }else {
    if (candado[l_s] == 1) {
      digitalWrite(_salida, 1);
      candado[l_s] = 0;
    }
  }
}
void loop()
{
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Reloj tiempo real
  //El reloj se ejecutará en todo momento
  if (tiempoAnterior != segundo){
    tiempoAnterior = segundo;
    
    if (segundo == 120){
      segundo = 0;
      minuto++;
    }
    if (minuto == 60){
      minuto = 0;
      hora++;
    }
    if (hora == 24){
      hora = 0;
    }
  } 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////      

  menuNew = digitalRead(menu);
  if (menuNew == 1 && menuOld == 0){
    num++;
    num = num % 6;
    num1 = 0;
  }
  menuOld = menuNew;
  switch(num){//Menú principal Boton Menu
    case 0://Salida Cero A
      arrayVariablesPorSalida[0][4] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[0][5] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[1][4] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[1][5] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[2][4] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[2][5] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[3][4] = true;// Se habilita procesadoTiempos()
      arrayVariablesPorSalida[3][5] = true;// Se habilita procesadoTiempos()

      display.setSegments(programA, 1, 0);
      funcionSubMenu();
    break;
    
    case 1://Salida Uno C
      display.setSegments(programC, 1, 0);
      funcionSubMenu();
    break;
    
    case 2://Salida Dos E
      display.setSegments(programE, 1, 0);
      funcionSubMenu();
    break;
    
    case 3://Salida Tres F
      display.setSegments(programF, 1, 0);
      funcionSubMenu();
    break;

    case 4://Programar hora
      subMenuNew = digitalRead(subMenu);
      if(subMenuNew == 1 && subMenuOld == 0){
        num3++;
        num3 = num3 % 2;
      }
      subMenuOld = subMenuNew;
      switch (num3){
        case 0:// Se asigna las horas programadas al arrayTiemposParpadeo
          procesadoTiempos(0);//asignacion salida 0
          procesadoTiempos(1);//asignacion salida 1
          procesadoTiempos(2);//asignacion salida 2
          procesadoTiempos(3);//asignacion salida 3
          display.setSegments(programarHora, 4, 0);
          // esperar hasta que se ejecute en todas las salidas procesadoTiempos(i)
        break;
        case 1://Programación de la hora
          ////////////Parpadeo/////////////
          tiempoInicioParpadeo = millis();
          if(tiempoInicioParpadeo - tiempoFinParpadeo >= 250){
            tiempoFinParpadeo = tiempoInicioParpadeo;
            if (displayVisible) {
              display.setSegments("", 2, 0);
              display.setSegments("", 2, 2);
            } else {
              display.showNumberDecEx(hora, 0b01000000, true, 2, 0);
              display.showNumberDec(minuto, true, 2, 2);
            }
            displayVisible = !displayVisible;
          }
          /////////////////////////////////////
              
          /////////////////////////////////////Botones 0 y 2 para aumentar hora y minuto, botones 1 y 3 para bajar hora y minuto
          botonCeroNew = digitalRead(botonCero);
          if(botonCeroNew == 1 && botonCeroOld == 0){
            hora++;
            hora = hora % 24;
            display.showNumberDecEx(hora, 0b01000000, true, 2, 0);
          }
          botonCeroOld = botonCeroNew;

          botonUnoNew = digitalRead(botonUno);
          if(botonUnoNew == 1 && botonUnoOld == 0){
            if (hora > 0) {
              hora--;
            } else {
              hora = 23;  
            }
            display.showNumberDecEx(hora, 0b01000000, true, 2, 0);
          }
          botonUnoOld = botonUnoNew;

          botonDosNew = digitalRead(botonDos);
          if(botonDosNew == 1 && botonDosOld == 0){
            minuto++;
            minuto = minuto % 60;
            display.showNumberDec(minuto, true, 2, 2);
          }
          botonDosOld = botonDosNew;

          botonTresNew = digitalRead(botonTres);
          if(botonTresNew == 1 && botonTresOld == 0){
            if (minuto > 0) {
              minuto--;
            } else {
              minuto = 59;  
            }
            display.showNumberDec(minuto, true, 2, 2);
          }
          botonTresOld = botonTresNew;
        break;
      }
    break;
    case 5:
      subMenuNew = digitalRead(subMenu);
      if (subMenuNew == 1 && subMenuOld == 0){
        num4++;
        num4 = num4 % 2;
      }
      subMenuOld = subMenuNew;


      switch (num4){
        case 0:
          
          display.showNumberDecEx(hora, 0b01000000, true, 2, 0);
          display.showNumberDec(minuto, true, 2, 2); //segundo/2 para mostrar 60 segundos

          /////////////Ejecución de Cronograma/////////////////////////////////////////////////////////////////////
          if (estadoActivacion[0] == 1){
            ejecucionDeTiempos(0, pinesSalida[0]);
          }
          if (estadoActivacion[1] == 1){
            ejecucionDeTiempos(1, pinesSalida[1]);
          }
          if (estadoActivacion[2] == 1){
            ejecucionDeTiempos(2, pinesSalida[2]);
          }
          if (estadoActivacion[3] == 1){
            ejecucionDeTiempos(3, pinesSalida[3]);
          }
          //////////////////////////////////////////////////////////////////////////////////
          //Aqui se puede cambiar el estado de las salidas
          botonCeroNew = digitalRead(botonCero);
          if (botonCeroNew == 1 && botonCeroOld == 0){
            if(estadoCero == 0){
              digitalWrite(pinesSalida[0], 0);
                estadoCero = 1;
                
            }else {
              digitalWrite(pinesSalida[0], 1);
              estadoCero = 0;
            }
          } 
          botonCeroOld = botonCeroNew;

          botonUnoNew = digitalRead(botonUno);
          if (botonUnoNew == 1 && botonUnoOld == 0){
            if (estadoUno == 0){
              digitalWrite(pinesSalida[1], 0);
                estadoUno = 1;
                
            }else {
              digitalWrite(pinesSalida[1], 1);
              estadoUno = 0;
            }
          }
          botonUnoOld = botonUnoNew;

          botonDosNew = digitalRead(botonDos);
          if (botonDosNew == 1 && botonDosOld == 0){
            if (estadoDos == 0){
              digitalWrite(pinesSalida[2], 0);
                estadoDos = 1;
            }else {
              digitalWrite(pinesSalida[2], 1);
              estadoDos = 0;
            }
          }
          botonDosOld = botonDosNew;

          botonTresNew = digitalRead(botonTres);
          if (botonTresNew == 1 && botonTresOld == 0){
            if (estadoTres == 0){
              digitalWrite(pinesSalida[3], 0);
                estadoTres = 1;
            }else {
              digitalWrite(pinesSalida[3], 1);
              estadoTres = 0;
            }
          }
          botonTresOld = botonTresNew;
        break;

        case 1://Aqui se toma los datos del sensor Pin 7
          //Sensor
          entradaSensorNew = digitalRead(sensor);
          if(entradaSensorNew == 1 && entradaSensorOld == 0){
            if(hora > 00 && minuto > 00){
              sumaSensor++;
            }else{
              sumaSensor = 0;
            }
          }    
          entradaSensorOld = entradaSensorNew;
          display.showNumberDec(sumaSensor, false, 4, 0);
        break;
      }
    break;
  }
}