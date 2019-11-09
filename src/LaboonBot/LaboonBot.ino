#include <ATDelay.h>

#include <TimerOne.h>
//#include <Servo.h>


#define BTN 4
#define LED 13
#define SW1 A2
#define SW2 A3
#define SENSOR_LINEA_DERECHO 7
#define SENSOR_LINEA_IZQUIERDO 6

#define MOTOR_DERECHO 9
#define MOTOR_IZQUIERDO 10

#define SENSOR_DERECHO A1
#define SENSOR_IZQUIERDO A0

/*
   VARIABLES ESTRATEGIAS
*/
int interruptores[] = {SW1, SW2};
int lectura_sw[] = {0, 0};
int estrategia = 0;

/*
   VARIABLES SENSORES RIVAL
*/
int enemigo_D = 0;
int enemigo_I = 0;
int adelante_D = 0;
int adelante_I = 0;

/*
   VARIABLES DE ARRANQUE
*/
long millisArranque = 0;
bool iniciado = false;

/*
   VARIABLES DE VELOCIDAD
*/
int vel_base = 850;
int vel_arranque = 1023;
int vel_max = 1023;
int vel_robot_D = 0;
int vel_robot_I = 0;

/*
   BANDERAS
*/

bool bandera11 = false;
bool bandera15 = false;

ATDelay estrg_1(140);
ATDelay estrg_2(140);
ATDelay estrg_3(240);
ATDelay estrg_3_1(200);
ATDelay estrg_4(250);
ATDelay estrg_4_1(350);
ATDelay estrg_7(230);
ATDelay estrg_7_1(200);
ATDelay estrg_7_2(250);
ATDelay estrg_9(2000);
ATDelay estrg_10(2000);
ATDelay estrg_11(100);
ATDelay estrg_16(200);
ATDelay estrg_16_1(200);
ATDelay siguelineas(200);
ATDelay turrabot(500);
ATDelay floor_1(500);
ATDelay floor_2(800);

//Servo motorDerecho;
//Servo motorIzquierdo;

void setup() {
  pinMode(MOTOR_DERECHO, OUTPUT);
  pinMode(MOTOR_IZQUIERDO, OUTPUT);

  pinMode(SENSOR_DERECHO, INPUT);
  pinMode(SENSOR_IZQUIERDO, INPUT);

  pinMode(SENSOR_LINEA_DERECHO, INPUT);
  pinMode(SENSOR_LINEA_IZQUIERDO, INPUT);

  pinMode(BTN, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

  pinMode(LED, OUTPUT);

  Serial.begin(9600);

  //  motorDerecho.attach(MOTOR_DERECHO);
  //  motorIzquierdo.attach(MOTOR_IZQUIERDO);
  //  motorDerecho.writeMicroseconds(2000);
  //  motorIzquierdo.writeMicroseconds(2000);
  //  delay(5000);
  //  motorDerecho.writeMicroseconds(1500);
  //  motorIzquierdo.writeMicroseconds(1500);
  //  delay(5000);

  Timer1.initialize(250);

  Timer1.pwm(MOTOR_DERECHO, 1023);
  Timer1.pwm(MOTOR_IZQUIERDO, 1023);
  delay(5000);
  Timer1.pwm(MOTOR_DERECHO, 768);
  Timer1.pwm(MOTOR_IZQUIERDO, 768);
  delay(5000);

  /*
     Lectura de estrategia
  */
  int binario = 0;
  for (int i = 1; i >= 0; i--) {
    lectura_sw[i] = !digitalRead(interruptores[i]);
    estrategia = estrategia + (lectura_sw[i] * binario);
    binario = binario * 2;
  }
  Serial.println(estrategia);


}

void loop() {

  //  comprobar_pines();
  //  return;

  /*
     Comprobación de sensores
  */
  if (estrategia == 8) {
    enemigo_D = analogRead(SENSOR_DERECHO);
    enemigo_I = analogRead(SENSOR_IZQUIERDO);
    if (enemigo_I > 200 || enemigo_D > 200) {
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }
    return;
  }

  if (!iniciado) {
    if (digitalRead(BTN)) {
      digitalWrite(LED, HIGH);
      //      millisArranque = millis();
    }

    if (!digitalRead(BTN)) {
      digitalWrite(LED, LOW);
      millisArranque = millis();
    } else if ((millis() >= (millisArranque + 5000)) && millisArranque > 0) {
      resetATDelay();
      iniciado = true;
    }
  } else {

    //Serial.print("\t\t");
    //Serial.println(estrategia);

    switch (estrategia) {

      case 0: //Salida de frente (rival mas lento)
        vel_robot_D = vel_base;
        vel_robot_I = vel_base;
        estrategia = 16;
        break;
      case 1: //Salida de lado giro a la derecha(rival mas lento)
        if (!estrg_1.check()) {
          vel_robot_D = -vel_arranque;
          vel_robot_I = vel_arranque;
        } else {
          estrategia = 16;
        }
        break;
      case 2: //Salida de lado giro a la izquierda (rival mas lento)
        if (!estrg_2.check()) {
          vel_robot_I = vel_arranque;
          vel_robot_D = -vel_arranque;
        } else {
          estrategia = 16;
        }
        break;
      case 3:  //Salida de espaldas (rival mas lento)
        if (!estrg_3.check()) {
          vel_robot_I = -vel_arranque;
          vel_robot_D = vel_arranque;
          estrg_3_1.reset();
        } else {
          if (!estrg_3_1.check()) {
            vel_robot_I = vel_max;
            vel_robot_D = vel_max;
          }
          else {
            estrategia = 16;
          }
        }
        break;
      case 4:  //Salida de frente (rival mas rapido)
        if (!estrg_4.check()) {
          vel_robot_I = vel_max;
          vel_robot_D = 80;
          estrg_4_1.reset();
        } else {
          if (!estrg_4_1.check()) {
            vel_robot_I = 50;
            vel_robot_D = vel_max;
          }
          else {
            estrategia = 16;
          }
        }
        break;
      case 5: //Salida de lado giro a la derecha (rival mas rapido)

        break;
      case 6: //Salida de lado giro a la izquierda (rival mas rapido)

        break;
      case 7: //Salida de espaldas (rival mas rapido)
        if (!estrg_7.check()) {
          vel_robot_I = -100;
          vel_robot_D = -vel_max;
          estrg_7_1.reset();
        } else {
          if (!estrg_7_1.check()) {
            vel_robot_I = -vel_max;
            vel_robot_D = -vel_max;
            estrg_7_2.reset();
          }
          else {
            if (!estrg_7_2.check()) {
              vel_robot_I = vel_max;
              vel_robot_D = vel_max;
            }
            else {
              estrategia = 16;
            }
          }
        }
        break;
      case 8:

        enemigo_D = analogRead(SENSOR_DERECHO);
        enemigo_I = analogRead(SENSOR_IZQUIERDO);
        if (enemigo_I > 200 || enemigo_D > 200) {
          digitalWrite(LED, HIGH);
        } else {
          digitalWrite(LED, LOW);
        }
        break;
      case 9:  //Medir fuerza
        if (!estrg_9.check()) {
          vel_robot_I = vel_max;
          vel_robot_D = vel_max;
        } else {
          vel_robot_I = 0;
          vel_robot_D = 0;
        }
        break;
      case 10: //Medir giro
        if (!estrg_10.check()) {
          vel_robot_I = vel_max;
          vel_robot_D = -vel_max;
        } else {
          vel_robot_I = 0;
          vel_robot_D = 0;
        }
        break;

      case 11: //Medir frenada

        adelante_I = 1023-analogRead (SENSOR_LINEA_IZQUIERDO);
        adelante_D = 1023-analogRead(SENSOR_LINEA_DERECHO);

        vel_robot_I = vel_max;
        vel_robot_D = vel_max;
        if (adelante_I >= 900 || adelante_D >= 900 || bandera11 == true) {
          bandera11 = true;
          if (!estrg_11.check()) {
            vel_robot_I = -vel_max;
            vel_robot_D = -vel_max;
          } else {
            vel_robot_I = 0;
            vel_robot_D = 0;
          }
        }
        break;
      case 12:  //Line follower
        adelante_I = 1023-analogRead(SENSOR_LINEA_IZQUIERDO);
        adelante_D = 1023-analogRead(SENSOR_LINEA_DERECHO);
        enemigo_D = analogRead(SENSOR_DERECHO);
        enemigo_I = analogRead(SENSOR_IZQUIERDO);

        if (enemigo_I > 200 || enemigo_D > 200) {
          estrategia = 16;
        }

        if (adelante_I < 900 && adelante_D < 900) {
          vel_robot_D = vel_base;
          vel_robot_I = vel_base;
        } else if (adelante_I >= 900 && adelante_D < 900) {
          vel_robot_D = -vel_base;
          vel_robot_I = vel_base;

        } else if (adelante_I < 900 && adelante_D >= 900) {
          vel_robot_D = vel_base;
          vel_robot_I = -vel_base;

        } else if (adelante_I >= 900 && adelante_D >= 900) {
          estrategia = 15;
        }

        break;
      case 13:  //Crazy

        break;
      case 14:  //Turrabot

        break;
      case 15:  //Don´t touch the floor
        adelante_I = 1023-analogRead (SENSOR_LINEA_IZQUIERDO);
        adelante_D = 1023-analogRead(SENSOR_LINEA_DERECHO);
        enemigo_D = analogRead(SENSOR_DERECHO);
        enemigo_I = analogRead(SENSOR_IZQUIERDO);
        //        Serial.print(adelante_I);
        //        Serial.print("\t");
        //        Serial.println(adelante_D);

        if (enemigo_I > 200 || enemigo_D > 200) {
          estrategia = 16;
        }

        if ((adelante_I >= 850 && adelante_I < 1010) || (adelante_D >= 850 && adelante_D < 1010) || bandera15 == true) {
          bandera15 = true;

          Timer1.pwm(MOTOR_DERECHO, 512);
          Timer1.pwm(MOTOR_IZQUIERDO, 512);
          delay(300);

          Timer1.pwm(MOTOR_DERECHO, 512);
          Timer1.pwm(MOTOR_IZQUIERDO, 1023);
          delay(500);

          Timer1.pwm(MOTOR_DERECHO, 768);
          Timer1.pwm(MOTOR_IZQUIERDO, 768);
          bandera15 = false;
          estrategia = 16;
        } else {
          vel_robot_D = vel_base;
          vel_robot_I = vel_base;
          estrategia = 16;
        }

        break;
      case 16:  //ataque (la idea es que aqui este la mayoria del tiempo. intentara no perder al rival e ir a por el)

        adelante_I = 1023-analogRead(SENSOR_LINEA_IZQUIERDO);
        adelante_D = 1023-analogRead(SENSOR_LINEA_DERECHO);
        enemigo_D = analogRead(SENSOR_DERECHO);
        enemigo_I = analogRead(SENSOR_IZQUIERDO);

        if ((adelante_I >= 850 && adelante_I < 1010) || (adelante_D >= 850 && adelante_D < 1010)) {
          estrategia = 15;
          return;
        }

        if (enemigo_I > 250 && enemigo_D > 250) {
          vel_robot_D = vel_max;
          vel_robot_I = vel_max;
          Timer1.pwm(MOTOR_DERECHO, vel_robot_D);
          Timer1.pwm(MOTOR_IZQUIERDO, vel_robot_I);
          delay(100);
        } else if (enemigo_I > 250) {
          vel_robot_D = 800;
          vel_robot_I = 767;
        } else if (enemigo_D > 250) {
          vel_robot_D = 767;
          vel_robot_I = 800;
        } else if (enemigo_I <= 200 && enemigo_D <= 200) {
          estrategia = 15;
        }
        break;
    }

    //    Serial.print(vel_robot_I);
    //    Serial.print("\t");
    //    Serial.println(vel_robot_D);
    //Serial.print(enemigo_I);
    //Serial.print("\t");
    //Serial.println(enemigo_D);
    Timer1.pwm(MOTOR_DERECHO, vel_robot_D);
    Timer1.pwm(MOTOR_IZQUIERDO, vel_robot_I);
  }
}


void comprobar_pines() {
  Serial.print("SRD:" );
  Serial.print(analogRead(SENSOR_DERECHO));
  Serial.print("\tSRI:" );
  Serial.print(analogRead(SENSOR_IZQUIERDO));
  Serial.print("\tSLD:" );
  Serial.print(analogRead(SENSOR_LINEA_DERECHO));
  Serial.print("\tSLI:" );
  Serial.print(analogRead(SENSOR_LINEA_IZQUIERDO));
  Serial.print("\tSW1:" );
  Serial.print(digitalRead(SW1));
  Serial.print("\tSW2:" );
  Serial.print(digitalRead(SW2));
  Serial.print("\tBTN:" );
  Serial.print(digitalRead(BTN));
  Serial.println();


  //Timer1.pwm(MOTOR_DERECHO, 1023);
  //Timer1.pwm(MOTOR_IZQUIERDO, 1023);
}

void resetATDelay() {
  estrg_1.reset();
  estrg_2.reset();
  estrg_3.reset();
  estrg_3_1.reset();
  estrg_4.reset();
  estrg_4_1.reset();
  estrg_7.reset();
  estrg_7_1.reset();
  estrg_7_2.reset();
  estrg_9.reset();
  estrg_10.reset();
  estrg_11.reset();
}
