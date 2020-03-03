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


// Boton manual
#define INICIO 13

// Elementos para el struct
 
#define SIZEROUTES 3
#define SIZENAME 10
#define STEPSIZE 10
#define TIMESIZE 10


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
bool modoAutomatico = true;


//Modo manual
typedef struct {
    char nombre[SIZENAME];
    char instrucciones[STEPSIZE];
    int timer[TIMESIZE];
} info;
 
info R1 = {"", {'X','X','X','X','X','X','X','X','X','X'},{0,0,0,0,0,0,0,0,0,0}};
info R2 = {"", {'X','X','X','X','X','X','X','X','X','X'},{0,0,0,0,0,0,0,0,0,0}};
info R3 = {"", {'X','X','X','X','X','X','X','X','X','X'},{0,0,0,0,0,0,0,0,0,0}};


 
info lista[SIZEROUTES] ={
    R1,
    R2,
    R3
};




String nombre="";
String timer="";

int actual=0;
int pos=0;
int ID=0;

bool entraSwitch = false;

/*
  ------------------------------Fin de variables----------------------------------
*/

/*
 * ----------------------------Buscar ruta
 */


int buscarRuta(String nombre){
  char *s;
  char param[10];
  nombre.toCharArray(param,10);
  for(int i=0;i<SIZEROUTES;i++){
       s = strstr(lista[i].nombre,param); 
     if (s != NULL){
      return i;
     }
    
    }
  return 0;
}

/*
 * ----------------------------Inicializar la EEPROM
 */
void inicializarArreglos(){
   int direccion = 0; 
  
   /*
    * En caso de ser esta la primera vez que se corre en otro Arduino, descomentar la siguiente línea:
    * EEPROM.put(direccion, lista);
    */
        EEPROM.get(direccion, R1);
        direccion += sizeof(info);
        EEPROM.get(direccion, R2);
        direccion += sizeof(info);
        EEPROM.get(direccion, R3);
        direccion += sizeof(info);
    
   EEPROM.put(direccion, lista);
}

/*
 * ----------------------------Movimiento dado un arreglo especifico
*/

void recorrerEleccion(int resultado){
  Serial.println("---------Entro a recorrerEleccion---------");
  for(int i=0;i<STEPSIZE;i++){
    if(lista[resultado].instrucciones[i]!='X'){
      char state= lista[resultado].instrucciones[i]; 
      int timer=lista[resultado].timer[i] * 1000;
      Serial.print("Direccion: ");
      Serial.print(state);
      Serial.print(" - Tiempo: ");
      Serial.print(timer);
      Serial.print("\n");
      if(state == 'N'){
        adelante(timer);
      }else if(state =='S'){
        atras(timer);
      }else if(state =='E'){
        derecha(timer);
      }else if(state =='O'){
        izquierda(timer);
      }    
    } 
  }
}

/*
 * ----------------------------Switch que determina acciones definidas por la APP
 */
void otrasAcciones(char entrada){
 /* 
  * Lectura de solicitud por bluetooth, donde:
  *   R es envio de ruta(el nombre termina en '/', cada fila termina en ';' y sus elementos separados por ',' )
  *   C es solicitud de recorrido de ruta
  *   ----->agregar otra mandera para enviar con Serial.print() los titulos de las rutas
 */
  
switch(actual){
  case 0:
    if(entrada=='R'){
      actual=1;
    }
    else if(entrada=='C'){
      actual=5;
    }
    else if(entrada=='S'){
      entraSwitch=false;
      char *s;
      char param[10]="";
      String envio="";
      for(int i=0;i<3;i++){
        s = strstr(lista[i].nombre,param); 
        if(s != NULL){
          envio+=lista[i].nombre;
          if(i!=2){
            envio+=",";
          }
        }    
      }
      for(int i = 0;i<100;i++){
        Serial.println(envio);
        delay(300);
      }
      envio="";
    }
  break;
  
  case 1:  
    if(entrada=='/'){
      actual=2;
    }
    else{
      nombre+=entrada;
    } 
    break;
  case 2:  
    nombre.toCharArray(lista[ID].nombre,10);
    actual=3;
    break;
  case 3:
    if(entrada=='F'){
      Serial.print("FIN");
      EEPROM.put(0, lista);
      actual=0;
      ID++;
      nombre="";
      pos=0;
      entraSwitch=false;
      if(ID==3){
        ID=0;
      }
    }
    else{
      if(entrada==','){
        actual=4;
      }else{
        lista[ID].instrucciones[pos] = entrada;
      }
      /*lista[ID].instrucciones[pos]=entrada; 
      lista[ID].timer[pos]=timer.toInt();
          
      pos++;*/
    } 
    break;
  case 4:
    if(entrada==';'){
      lista[ID].timer[pos] = timer.toInt();
      pos++;
      actual=3;
      timer="";
                
    }
    else{
      timer+=entrada;  
    }
    break;
  case 5:
       
    if(entrada=='F'){
      int resultado=nombre.toInt();
      Serial.println("Resultado: " + resultado);
      recorrerEleccion(resultado);
      actual=0;
      nombre=""; 
      entraSwitch = false;
    }
    else{
      nombre+=entrada;
    }
       
    break;    
     
}
  
}





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



void barrer(int cDelay){



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


void derecha(int cDelay){
  // Encendido
  digitalWrite(MT1P1, HIGH);
  digitalWrite(MT1P2, LOW);
  digitalWrite(MT2P1, LOW);
  digitalWrite(MT2P2, HIGH);

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
  digitalWrite(MT1P2, HIGH);
  digitalWrite(MT2P1, HIGH);
  digitalWrite(MT2P2, LOW);

  delay(cDelay);

  // Apagado
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
  Serial.print("\n");

  // Si detecta color Rojo
  if(R>32&&R<60){
    Serial.println("Rojo");
    return 1;
  }
  // Si detecta color Azul
  else if(R>70&&R<90){
    Serial.println("Azul");
    return 2;
  }
  // Si detecta color Negro
  else if(R>95){
    return 3;
  }
  // Si no detecta ni uno
  else{
    // Serial.println("Otro...");
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

  //int distanciaRecorrida = 1000000;
  long int distanciaRecorrida = 1000000;
  
  digitalWrite(TRIGGERUS, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGERUS, LOW);
  int tiempo = pulseIn(ECHO, HIGH);
  
  distanciaRecorrida = tiempo/59;
  Serial.println("Distancia: ");
  Serial.print(distanciaRecorrida);
  Serial.println();

  // Si detecta un obstaculo cerca
  if(distanciaRecorrida < 10&&distanciaRecorrida>0){
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
  // Serial.println(colorDetectado);

  // Rojo
  if(colorDetectado == 1){
    // Serial.print("Rojo \n");
    barrer(2000);
  }
  // Azul
  else if(colorDetectado == 2){
    Serial.print("Azul \n");
    modoAutomatico = false;
  }
  // Negro
  else if(colorDetectado == 3){
    // Serial.print("Negro \n");
    retroceder(1000);
  }
  // Ninguno
  else{

    bool obstaculo = false;

    obstaculo = detectarObstaculo();

    // Serial.print(obstaculo + "\n");
    
    if(obstaculo){
      // Serial.print("Detecto obstaculo \n");
      atras(1000);
    }else{
      // Serial.print("No detecto obstaculo \n");
      adelante(1000);
    }
    
  }
}

void loopManual(){
<<<<<<< Updated upstream
  int resultado;
  char state;
 // Serial.print("Entro a loop manual \n");
=======
>>>>>>> Stashed changes
  /*
    Solo debe de detectar la accion que le envia el telefono y realizar el movimiento correspondiente
  */
  if(Serial.available()>0){
    resultado= Serial.read();
    state=resultado;
    Serial.print("Resutlado: ");
    Serial.println(state);
if(!entraSwitch){
  if(state == '0'){
      adelante(2000);
    
  }else if(state == '1'){
    
      atras(2000);
    
  }else if(state == '2'){
     derecha(995);
  }else if(state == '3'){
      izquierda(1000);
  }else if(state == '4'){
      barrer(2000);
   
  }else if(state == '5'){
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
    
   for (int i = 0 ; i < SIZEROUTES ; ++i) {
         for(int k=0;k<STEPSIZE;k++){
         lista[i].nombre[k]=' ';
         lista[i].instrucciones[k]='X';
         lista[i].timer[k]=0;
        }
    }
    
  }else if(state == '6'){
    for(int i = 0;i<5;i++){
      // Guardar ruta en memoria
    }
  }else{
    Serial.println("Entro a otras acciones");
    entraSwitch=true;
    otrasAcciones(state);

  }

<<<<<<< Updated upstream
}
 else{
    Serial.println("Entro a otras acciones");
    Serial.print("Actual: ");
    Serial.println(actual);
    otrasAcciones(state);
=======
    char entrada;
    entrada=state;
    otrasAcciones(entrada);
>>>>>>> Stashed changes
  }
 }
}

void setup() {

  Serial.begin(9600);

  // Setup del Ultrasonico
  pinMode(TRIGGERUS, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIGGERUS, LOW);

  // Setup del sensor de color
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(S4,OUTPUT);
  pinMode(SOut,INPUT);

  digitalWrite(S1,HIGH);
  digitalWrite(S2,HIGH);

  // Setup inicio
  pinMode(INICIO, INPUT);
  digitalWrite(INICIO, LOW);


  // Setup para EEPROM
  inicializarArreglos();

}


void loop() {

  if(digitalRead(INICIO)==HIGH){
    modoAutomatico = !modoAutomatico;
    delay(1000);
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
