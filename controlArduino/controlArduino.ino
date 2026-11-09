#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Ventilador aire caliente
const int in1Pin = 9;
const int in2Pin = 10;
const int enAPin = 11;

// Ventilador aire frio
const int in3Pin = 4;
const int in4Pin = 5;
const int enBPin = 6;

const int foco = 8;

//unsigned long tiempoprevio = 0;
//int tiempoCambio = 5000;
double temp;

//int fanState = 0;  // 0 - Off, 1 - Medium, 2 - Max
//bool lightState = false; // false - off, true - on

LiquidCrystal_I2C lcd(0x27, 16, 2);
int timeActualizarPantalla = 1000;
unsigned long previousTime = 0;
int intervalDistancia = 1000;
unsigned long previousMillisDistancia = 0;
unsigned long previousMillis2 = 0;

const unsigned long interval2 = 500; //intervalo entre segundos.

float kp;
float ki;
float kd;

double elapsedTime = 0.0;
double error;
double setpoint=38;
double cumError;
double lastError=0;
double rateError;
unsigned long previoPID = 0;
double velocity;



//"Sensor Ultrasonido"
int trig = 12;
int echo = 13;
int timeInMicro;
int distanceInCm;


void setup() {
  Serial.begin(9600);
  dht.begin();
  
  // Initialize the pins as outputs
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enAPin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enBPin, OUTPUT);
  pinMode(foco, OUTPUT);
  pinMode(A0, INPUT);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Start with the fan and light off
  digitalWrite(in2Pin, LOW);
  digitalWrite(in1Pin, LOW);
  analogWrite(enAPin, 0); // Fan AIRE CALIENTE off
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, LOW);
  analogWrite(enBPin, 0); // Fan AIRE FRIO off
  digitalWrite(foco, LOW); // Light off

  //"Sensor Ultrasonido"
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  unsigned long currentTime = millis();
  
//Data
  unsigned long currentMillis2 = millis();

  if (currentMillis2 - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis2;
  }
  
  float currentTime2 = currentMillis2/1000.0;
    
  // Medir altura para fijar el setpoint

  digitalWrite(trig, LOW);
  delayMicroseconds(2);   //solo es para limpiar la senial anterior
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  timeInMicro = pulseIn(echo, HIGH);
  distanceInCm = timeInMicro / 29 / 2;    //equivalencia de velociadad del sonido 340m/s = 29 microsegundos/cm

  if (currentMillis2 - previousMillisDistancia >= intervalDistancia) {
    if (distanceInCm < 39) {
      setpoint = (int) (46.62 - 0.27*distanceInCm);
    }                  
    previousMillisDistancia = currentMillis2;
  }
  
  // Read temperature from DHT22 sensor
  temp = dht.readTemperature();

  // Display temperature on Serial Monitor
  /*Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");
*/
  int k=analogRead(A3);

  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, HIGH);
  digitalWrite(in4Pin, LOW);

  // Update LCD every timeActualizarPantalla interval
  if (currentTime - previousTime >= timeActualizarPantalla) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp); //////////////////////////////////////////////////////////// probando setpoint, borrar siguiente linea
    //lcd.print(setpoint);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("v: ");
    double realVelocity = velocity*1.6/255;
    lcd.print(realVelocity); //////////////////////////////////////////////////////////// probando ultrasonico, borrar siguiente linea
    //lcd.print( distanceInCm);
    lcd.print(" m/s");
    previousTime = currentTime;  // Reset the LCD update timer
  }
  

  kp= -50.0; 
  ki=0.0145; 
  kd=2.25*2.0061; 
  elapsedTime=(double)((currentMillis2-previoPID)/1000.0);
  error=setpoint-temp;
  cumError+=error*elapsedTime;
  rateError=(error-lastError)/elapsedTime;
  velocity=kp*(error+ki*cumError+kd*rateError);

  /*Serial.print("\ElapTime: ");
  Serial.print(elapsedTime);
  Serial.println();

  Serial.print("\CumE: ");
  Serial.print(cumError);
  Serial.println();
  Serial.print("\ratE: ");
  Serial.print(rateError);
  Serial.println();
 
  Serial.print("\Vel: ");
  Serial.print(velocity);
  Serial.println();
*/
  velocity=constrain(velocity,10,255);
  lastError=error;
  previoPID=currentTime;

/*
  Serial.print("\nError: ");
  Serial.print(error);
  Serial.println();
 */
  
  velocity=(int) velocity;
  analogWrite(enAPin, velocity); //255 max
  analogWrite(enBPin, velocity); //255 max  

  double realVelocity = velocity*1.6/255;

  // Plot para Matlab

  Serial.print(currentTime2);
  Serial.print(","); 
  Serial.print(temp);
  Serial.print(","); 
  Serial.print(setpoint);
  Serial.print(","); 
  Serial.println(realVelocity);
  delay(500);
}