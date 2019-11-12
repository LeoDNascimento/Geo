// Inclusao das bibliotecas
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SDISerial.h>
#include <DHT.h>

#define DATALINE_PIN 2
#define INVERTED 1
#define DHTPIN 7 //PINO DIGITAL UTILIZADO PELO DHT22
#define DHTTYPE DHT22 //DEFINE O MODELO DO SENSOR (DHT22 / AM2302)
 

SDISerial sdi_serial_connection(DATALINE_PIN, INVERTED);
DHT dht(DHTPIN, DHTTYPE); //PASSA OS PARÂMETROS PARA A FUNÇÃO

const int PINO_ONEWIRE = 12; // Define pino do sensor
OneWire oneWire(PINO_ONEWIRE); // Cria um objeto OneWire
DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire
DeviceAddress endereco_temp;


const int AirValue =20 ; //you need to replace this value with Value_1
const int WaterValue = 58; //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue)/3;
float soilMoistureValue = 0;
float soilMoistureValuei;
int erro=0;

char* get_measurement(){
  char* service_request = sdi_serial_connection.sdi_query("?M!",1000);
  //you can use the time returned above to wait for the service_request_complete
  char* service_request_complete = sdi_serial_connection.wait_for_response(1000);
  //dont worry about waiting too long it will return once it gets a response
  return sdi_serial_connection.sdi_query("?D0!",1000);
}


void solo()
{
  soilMoistureValue = analogRead(A0); //put Sensor insert into soil
  soilMoistureValue= (1-(soilMoistureValue/1023))*100;
  soilMoistureValue= ((soilMoistureValue-AirValue)/(WaterValue-AirValue))*100;
  soilMoistureValuei=int(soilMoistureValue);
  if(soilMoistureValuei<0)
    erro++;
  Serial.print("Umidade: ");
  Serial.print(soilMoistureValuei);
  Serial.print("% ");
  Serial.print("|| Erro: ");
  Serial.println(erro);
  
  }

void temp(){
    sensor.requestTemperatures(); // Envia comando para realizar a conversão de temperatura
  if (!sensor.getAddress(endereco_temp,0)) { // Encontra o endereco do sensor no barramento
    Serial.println("SENSOR NAO CONECTADO"); // Sensor conectado, imprime mensagem de erro
  } else {
    Serial.print("Temperatura = "); // Imprime a temperatura no monitor serial
    Serial.println(sensor.getTempC(endereco_temp), 1); // Busca temperatura para dispositivo
  }
}
 
void setup() {
  sensor.begin(); ; // Inicia o sensor
  sdi_serial_connection.begin(); // start our SDI connection 
  dht.begin(); //INICIALIZA A FUNÇÃO
  Serial.begin(9600); // start our uart
  Serial.println("OK INITIALIZED"); // startup string echo'd to our uart
  delay(3000); // startup delay to allow sensor to powerup and output its DDI serial string
  
}



int j=0;
void loop() {
  uint8_t wait_for_response_ms = 1000;
  char* response = get_measurement(); // get measurement data
   solo();
   Serial.println("=======================");
   delay(1000);
   
   Serial.println("Medindo Temperatura"); // Imprime a mensagem inicial
    temp();
    Serial.println("===============================");
    delay(1000);

    Serial.println("===============================");
    Serial.print("RECV:");
    Serial.println(response!=NULL&&response[0] != '\0'?response:"No Response!"); //just a debug print statement to the serial port
    delay(1000);
    Serial.println("===============================");
    Serial.print("Umidade do ar: "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(dht.readHumidity()); 
    Serial.print("%"); 
    Serial.print(" / Temperatura do ar: "); 
    Serial.print(dht.readTemperature()); 
    Serial.println("*C"); 
    delay(2000); 
    Serial.println("===============================");
}
