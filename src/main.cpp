//Control beta lazo de temperatura con rtd pt100
//Autor: Sergio Pablo Peñalve
//catriel - Rio Negro


#include <LiquidCrystal.h>
#include <PID_v1.h>
#include <Arduino.h>


//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

LiquidCrystal lcd(6, 7, 2, 3, 4, 5); //usamos la primer filas de pines

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint,5,0.2,0.5, DIRECT);
 
//#define _uno
#define _mega_2560
 
//metodo de calibracion 

//Usar un baño termostatico seco o de aceite a falta del mismo usar termometro,pirometro o termocupla
//agua congelada y agua hervida

//La RTD PT100 varia la resistencia de manera lineal a la temperatura por lo que tomando dos puntos en baja temp y alta temp
//podemos generar una funcion lineal con estos puntos y transformar el valor ADC en grados C°


//otra forma es calculando la resistencia el voltaje y por regla de tres con respecto al valor ADC obtener la T°

 
float Ay1= 17.2;  //1 RTD °C puntos de calibracion frio
float Ax1=306; //adc

float By1= 17.2;  //2 RTD °C puntos de calibracion frio
float Bx1=302; //adc

float Ay2=60; //1 RTD °C puntos de calibracion caliente
float Ax2=623; //adc
 
float By2=60; ///1 RTD °C puntos de calibracion caliente
float Bx2=618; //adc

int t1,t2,i,cant;
float acum1,acum2,m1,m2,c1,c2,temp1,temp2;
int setP=15; //temperatura estandar para caudal u otros


//funcion pendiente con respecto a dos puntos se acuerdan de las matematicas 
// y te preguntabas para que fuck servia esto jejeje

float pendiente(float y1,float x1,float y2, float x2)
{
 float m=0;
 m=(y2-y1)/(x2-x1);
 return m;
}

//la constante la obtengo despejando 
float constante(float m,float y1,float x1)
{
  float c=0;   
  c=y1-(m*x1);
  return c;
}

//esta es la funcion lineal
float obt_temp(float m,float c,float x1)
{
  float y1=(m*x1)+c;
  return y1;
}



//esta es una funcion inversa le damos la temp y nos devuelve el ADC para el PID
float obt_adc(float m,float c,float y1)
{
  float x1=(y1 - c)/m;
  return x1;
}

void menu1()
{
  
  

  

}





void setup()
{
  cant=10000;
  Serial.begin(9600);
#ifdef _uno    
  analogReference(INTERNAL); //arduino uno
 #endif
       
#ifdef _mega_2560    
  analogReference(INTERNAL1V1); //arduino mega 2560
#endif
  
myPID.SetMode(AUTOMATIC);
lcd.begin(16, 2);
lcd.setCursor(0, 0);  
}

void loop()
{ 
  for (i=0;i<10000;i++)
  {
  t1 = analogRead(1);
  t2 = analogRead(2);
  
  acum1=acum1+(float)t1;
  acum2=acum2+(float)t2;
  }
  
  acum1=acum1/cant;
  acum2=acum2/cant;
  
 
  //calculo de temperatura por funcion lineal
  // y=m * b + c
  // donde m es la pendiente
  // y - b son los puntos
  //c es la constante
   
  m1=pendiente(Ay1,Ax1,Ay2,Ax2);
  m2=pendiente(By1,Bx1,By2,Bx2);
  
  c1=constante(m1,Ay1,Ax1);
  c2=constante(m2,By1,Bx1);
  
  
  temp1=obt_temp(m1,c1,acum1);
  temp2=obt_temp(m2,c1,acum2);
  lcd.clear();
  lcd.print("PV:"); 
  lcd.print(temp1);
  
  
  Setpoint=obt_adc(m1,c1,setP);
  lcd.setCursor(0, 1); 
  lcd.print("SP:");
  lcd.print(obt_temp(m1,c1,Setpoint));
  
 // chequeo de falla de alguna RTD
  Input=acum1;
  if (acum1<= Ax1)
  {
    Input = acum2;
   
  }
  if(acum2<=Ax1)
  {
    Input = acum1;
  
  }
     
  if (Input < Ax1 or Input < Bx1)
  {
   myPID.SetMode(MANUAL);
   lcd.clear();
   lcd.print("Falla en RTD"); 
  }/*else 
  {
  Input =acum1;
  }*/
  
  //Input = acum1;
  myPID.Compute();

//prioridad en control
#ifdef _uno
  analogWrite(9,Output);
  analogWrite(10,Output);
  analogWrite(11,Output);
#endif

#ifdef _mega_2560
  analogWrite(11,Output);
  analogWrite(12,Output);
  analogWrite(13,Output);
#endif
if (Output>0)
  {
   lcd.print(" OUT:ON ");
  }
   else
  {
   lcd.print(" OUT:OFF");   
  }
  menu1();
  Serial.print("Input de temp --> ");
  Serial.println(Input);
  
    
  Serial.print("Setpoint de temp --> ");
  Serial.println(Setpoint);
  
  Serial.print("Salida Output");
  Serial.println(Output);

}


  
