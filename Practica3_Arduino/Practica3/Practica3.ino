/*
  ------------------------------Librerias a usar---------------------------------
*/

#include <EEPROM.h>

/*
  ------------------------------Fin de librerias---------------------------------
*/

/*
  ------------------------------Pines usados-------------------------------------
*/

// Ultrasonico
#define TRIGGERUS 2
#define ECHO 3

// Sensor de color
#define S1 4
#define S2 5
#define S3 6
#define S4 7

#define SOut 8

// Motores del carro
// Motor 1
#define MT1P1 9
#define MT1P2 10

// Motor 2
#define MT2P1 11
#define MT2P2 12

// Motor Barrido
#define MTBP1 13
#define MTBP2 14

// Boton manual
#define INICIO 15

/*
  ------------------------------Fin de pines usados-------------------------------
*/

/*
  ------------------------------Variables a utilizar------------------------------
*/

/*  
 *   Variable que controla lo que envia la aplicacion Android.
 *   Tendra los siguientes valores dependiendo de que boton
 *   presionen en la aplicacion:
 *      0 = ARRIBA
 *      1 = ABAJO
 *      2 = DERECHA
 *      3 = IZQUIERDA
 *      4 = BARRER
 *      5 = LIMPIAR MEMORIA
 *      6 = GUARDAR MEMORIA
 */
int state = -1;
bool modoAutomatico = true;

/*
  ------------------------------Fin de variables----------------------------------
*/

/*
  --------------------Movimiento--------------------
*/
void adelante(int cDelay){
  // Encendido
  digitalWrite(MT1P1, HIGH);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, HIGH);
  digitalWrite(MT2P2, LOW);

  delay(cDelay);

  // Apagado
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, LOW);
}

void atras(int cDelay){
  // Encendido
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, HIGH);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, HIGH);

  delay(cDelay);

  // Apagado
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, LOW);
}

void derecha(int cDelay){
  // Encendido
  digitalWrite(MT1P1, HIGH);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, LOW);

  delay(cDelay);

  // Apagado
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, LOW);
}

void izquierda(int cDelay){
  // Encendido
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, HIGH);
  digitalWrite(MT2P2, LOW);

  delay(cDelay);

  // Apagado
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, LOW);
}

void barrer(int cDelay){
  // Encendido
  digitalWrite(MTBP1, HIGH);
  digitalWrite(MTBP2, LOW);

  delay(cDelay);

  // Apagado
  digitalWrite(MTBP1, LOW);
  digitalWrite(MTBP2, LOW);
}

void retroceder(int cDelay){
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, HIGH);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, HIGH);

  delay(cDelay);
  
  digitalWrite(MT1P1, LOW);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, LOW);
}

/*
  --------------------Fin Movimiento--------------------
*/

/*
  --------------------Color detectado--------------------
*/

int detectarColor(){
  int R = 0, G = 0, B = 0;
  
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  R = pulseIn(SOut, digitalRead(SOut) == HIGH ? LOW : HIGH);
  digitalWrite(S3, HIGH);
  B = pulseIn(SOut, digitalRead(SOut) == HIGH ? LOW : HIGH);
  digitalWrite(S2, HIGH);
  G = pulseIn(SOut, digitalRead(SOut) == HIGH ? LOW : HIGH);

  Serial.print("    ");
  Serial.print(R, DEC);
  Serial.print("    ");
  Serial.print(G, DEC);
  Serial.print("    ");
  Serial.print(B, DEC);

  // Si detecta color Rojo
  if(R < B && G > B && R < 35){
    return 1;
  }
  // Si detecta color Azul
  else if(B < R && B < G && G < R){
    return 2;
  }
  // Si detecta color Negro
  /*else if(){
    return 3;
  }*/
  // Si no detecta ni uno
  else{
    return 0;
  }
  
}

/*
  --------------------Fin Color detectado--------------------
*/

/*
  --------------------Obstaculo detectado--------------------
*/

bool detectarObstaculo(){

  int distanciaRecorrida = 10000;
  
  digitalWrite(TRIGGERUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGERUS, LOW);
  int tiempo = pulseIn(ECHO, HIGH);
  
  distanciaRecorrida = tiempo/59;

  // Si detecta un obstaculo cerca
  if(distanciaRecorrida < 7){
    return true;
  }
  // Si no detecta obstaculo cerca
  else{
    return false;
  }
}

/*
  --------------------Fin Obstaculo detectado--------------------
*/

void loopAutomatico(){
  
  int colorDetectado = 0;  

  colorDetectado = detectarColor();
  // Rojo
  if(colorDetectado == 1){
    barrer(500);
  }
  // Azul
  else if(colorDetectado == 2){
    modoAutomatico = false;
  }
  // Negro
  else if(colorDetectado == 3){
    retroceder(500);
  }
  // Ninguno
  else{

    bool obstaculo = false;

    obstaculo = detectarObstaculo();
    if(obstaculo){
      // Mover derecha o izquierda
    }else{
      adelante(500);
    }
    
  }
}

void loopManual(){
  if(Serial.available()>0){
    state = Serial.read();
  }
  
  if(state == 0){
    for(int i = 0;i<5;i++){
      adelante(500);
    }
  }
  else if(state == 1){
    for(int i = 0;i<5;i++){
      atras(500);
    }
  }
  else if(state == 2){
    for(int i = 0;i<5;i++){
      derecha(500);
    }
  }
  else if(state == 3){
    for(int i = 0;i<5;i++){
      izquierda(500);
    }
  }
  else if(state == 4){
    for(int i = 0;i<5;i++){
      barrer(500);
    }
  }
  else if(state == 5){
    for(int i = 0;i<5;i++){
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
    }
  }
  else if(state == 6){
    for(int i = 0;i<5;i++){
      // Guardar ruta en memoria
    }
  }
  else{
    Serial.println("Saber ni que pedo paso, state = " + state);
  }
}

void setup() {

  Serial.begin(9600);

}

void loop() {

  if(digitalRead(INICIO == HIGH)){
    modoAutomatico = !modoAutomatico;
  }

  if(modoAutomatico){
    
    loopAutomatico();
  
  }else{
    
    loopManual();
  
  }

}

/*






                                                                                        Flujo de la aplicacion
                                                      
                                                                              -------------MODO AUTOMATICO-------------
                                                      El robot iniciara en el modo automatico. Este debe de estar detectando objetos todo el rato.
                                                      Debe de poder evadir por la izquierda y por la derecha. Este deberá de poder moverse solo
                                                      dentro de los limites de la pista, considerando limite por pared o por gradas.
                                                      En modo automatico aparte de moverse todo el rato para adelante y barrer debe de detectar 
                                                      los siguientes colores y realizar la accion correspondiente:
                                                          Rojo: Barrer
                                                          Azul: Modo Manual
                                                          Negro: Retroceder y girarse
                                                      Para cambiar al modo manual tambien se necesita de un boton en el robot.
                                                      
                                                                              -------------MODO MANUAL-------------
                                                      Este modo solo debe de estar leyendo las salidas de la aplicacion asi como mandar a la aplicacion
                                                      las rutas que se han guardado en el EEPROM. Tambien se le deberá de poder enviar rutas a elegir
                                                      Tambien existira la opción limpiar rutas que borrara la memoria EEPROM del arduino.






*/
