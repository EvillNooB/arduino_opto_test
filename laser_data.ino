#define s_i 11 
#define s_o 10

#define d_i 13
#define d_o 12

#define connection_timeout 500
#define delaytime 16
void setup() {

pinMode(s_i, INPUT);
pinMode(d_i, INPUT);

pinMode(s_o, OUTPUT);
pinMode(d_o, OUTPUT);

Serial.setTimeout(115);
Serial.begin(115200);
}

bool receiving_message = false;
String buffer1;
String encoded_character;
unsigned long millis1;

unsigned long startTime;
unsigned long endTime;
short bytecounter = 0;
void loop() {

  
// receiving the message
if (digitalRead(s_i))
  {
     // begin listening
    digitalWrite(s_o,1);

    // wait for the sender to acknowledge this
    millis1 = millis();
    while (!digitalRead(s_i)) {
      if (millis()>=millis1+connection_timeout) 
           {
            Serial.println("Таймаут подключения W3");
            softReset();
            return false;
           }
    }
    
    receiving_message = true;
    startTime = millis();
    Serial.print(" - - - ");
    Serial.print("Полученное сообщение");
    Serial.println(" - - - ");
  }



while (receiving_message) 
  {
  byte character = 0b00000000;
  for (short b=0; b<=7;b++) 
   {      

    // ожидание сигнала готовности 1 бита для считывания
           millis1 = millis();
           while (!digitalRead(s_i)) 
           {
              if (millis()>=millis1+connection_timeout) 
              {
              Serial.println("Таймаут подключения W1");
              softReset();
              return false;
              }
          }
          
        //doesn't work without this, probably timing issue with digitalWrite and Read
          delay(delaytime);
        // ---------------------------------------------------------

        
          // счиытваем готовый бит
          bitWrite(character,b,digitalRead(d_i));
          // сообщаем что считали
          digitalWrite(d_o,1);

          // ждём пока отправитель получит подтверждение
          millis1 = millis();
          while (digitalRead(s_i)) 
           {
              if (millis()>=millis1+connection_timeout) 
              {
              Serial.println("Таймаут подключения W2");
              softReset();
              return false;
              }
          }

          // показываем что готовы считать следующий бит
          digitalWrite(d_o,0);
    
   }
   bytecounter++;
  Serial.print((char)character);
  if (character == 0b00000000) {
    Serial.println((char)character);
    receiving_message = false;
    digitalWrite(s_o,0);
    endTime = millis();
     Serial.println(" - - - - - - - - - - - - - - - - -");
        Serial.print("затрачено времени ");
  float spent =(float) (endTime*1ul - startTime*1ul)/1000;
Serial.print(spent);
  Serial.println(" с.");
  Serial.print("Скорость передачи данных - ");
  Serial.print((float) bytecounter/spent);
  Serial.println(" байт/c");
  bytecounter = 0;
  }
  } 





  if (Serial.available()) {
    buffer1 = Serial.readString();
    
    // check for carriage return, null symbol and new line/ remove if found
    for (int i=0; i<=buffer1.length(); i++) {
      if ((buffer1.charAt(i) == 13) || (buffer1.charAt(i) == 10) || (buffer1.charAt(i) == 0)) {
        buffer1.remove(i);
    }
  }




    Serial.print(" - - - ");
    Serial.print("Отправка сообщения");
    Serial.println(" - - - ");
  if (sendMessage(buffer1)) {
    Serial.println("");
     Serial.println(" - - - - - - - - - - - - - - - - -");
      endTime = millis();
  Serial.print("затрачено времени ");
  float spent =(float) (endTime*1ul - startTime*1ul)/1000;
Serial.print(spent);
  Serial.println(" с.");
  Serial.print("Скорость передачи данных - ");
  Serial.print((float) bytecounter/spent);
  Serial.println(" байт/c");
   bytecounter = 0;
  } else {
    Serial.println("Ошибка");
  }

   }
 
}


bool sendMessage(String message) {
  startTime = millis();
  // try to initialize the connection
  digitalWrite(s_o,1);
  millis1 = millis();
  while (!digitalRead(s_i)) {
    if (millis()>=millis1+connection_timeout) {
      Serial.println("Таймаут подключения W4");
      softReset();
      return false;
    }
  }
  digitalWrite(s_o,0);


   // Sending the message
       for (int i=0; i<=message.length(); i++) 
       {
        byte character = buffer1[i];
        for (short b=0; b<=7;b++) 
        {
         // отправляем 1 бит
         digitalWrite(d_o,bitRead(character,b));

         //doesn't work without this, probably timing issue with digitalWrite and Read
          delay(delaytime);
        // ---------------------------------------------------------
        
          // показываем что бит отправлен и готов для считывания
         digitalWrite(s_o,1);

         // ожидание подтверждения получения 1 бита
         millis1 = millis();
           while (!digitalRead(d_i)) 
           {
              if (millis()>=millis1+connection_timeout) 
              {
              Serial.println("Таймаут подключения W5");
              softReset();
              return false;
              }
          }

          // когда подтверждение получено - выключаем всё, показывая что сейчас будет передача следующего бита
          digitalWrite(s_o,0);
          
          // ждём чтобы принимающая сторона поступила так же
           while (digitalRead(d_i)) 
           {
              if (millis()>=millis1+connection_timeout) 
              {
              Serial.println("Не выключен дата пин после приёма");

              softReset();
              return false;
              }
          }
        }
       bytecounter++;
       Serial.print(buffer1[i]);
       }  


       digitalWrite(d_o,0);
       millis1 = millis();
       while (digitalRead(s_i))
        {
           if (millis()>=millis1+connection_timeout) 
           {
            Serial.println("Receiver missed 'end of transmission' signal");
            softReset();
            return false;
           }
        }
        return true;

  
  

}
  

void softReset() {
digitalWrite(s_o,0);
digitalWrite(d_o,0);
receiving_message = false;
Serial.println("soft-reset");
delay(1000);
}
