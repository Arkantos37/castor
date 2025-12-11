
volatile long encoder_pos = 0;   // contador de posición (volatile por uso en interrupciones)
long setpoint = 0;               // objetivo de posición

// CONFIGURACIÓN 	
void setup() {

  //  CONFIGURACIÓN DE GPIO
  // Pines del encoder 
  DDRD &= ~(1 << PD2);   // PD2 como entrada
  DDRD &= ~(1 << PD3);   // PD3 como entrada

  // Activar pull-up internos
  PORTD |= (1 << PD2);
  PORTD |= (1 << PD3);

  // Pines de dirección del motor (ejemplo: PB0 y PB1)
  DDRB |= (1 << PB0);    // Dirección 1
  DDRB |= (1 << PB1);    // Dirección 2

  // Pin PWM OC1A → PB1 (pero en este ejemplo usamos OC1A = PB1 o OC1B = PB2)
  DDRB |= (1 << PB2);    // PB2 = OC1B como salida de PWM


  //                  CONFIGURACIÓN DE INTERRUPCIONES 
  // INT0 (PD2) configurado a flanco de cambio (ISC00 = 1, ISC01 = 0)
  EICRA |= (1 << ISC00);
  EICRA &= ~(1 << ISC01);

  // INT1 (PD3) configurado igual (lee canal B)
  EICRA |= (1 << ISC10);
  EICRA &= ~(1 << ISC11);

  // Habilitar interrupciones INT0 y INT1
  EIMSK |= (1 << INT0);
  EIMSK |= (1 << INT1);


  //                CONFIGURACIÓN DEL PWM (Timer1) 
  // Modo Fast PWM 10 bits: WGM10 = 1, WGM11 = 0, WGM12 = 1, WGM13 = 0
  TCCR1A |= (1 << WGM10);
  TCCR1A &= ~(1 << WGM11);
  TCCR1B |= (1 << WGM12);
  TCCR1B &= ~(1 << WGM13);

  //                 Modo non-inverting en OC1B
  TCCR1A |= (1 << COM1B1);
  TCCR1A &= ~(1 << COM1B0);

  // Prescaler = 64 → CS11 = 1, CS10 = 1
  TCCR1B |= (1 << CS11) | (1 << CS10);

  // Duty cycle inicial
  OCR1B = 0;

  
  Serial.begin(9600);
}



//                      INTERRUPCIONES DEL ENCODER 

// ISR canal A (INT0)
ISR(INT0_vect) {
  uint8_t A = (PIND >> PD2) & 1;
  uint8_t B = (PIND >> PD3) & 1;

  if (A == B)
    encoder_pos++;      // Giro horario
  else
    encoder_pos--;      // Giro antihorario
}


ISR(INT1_vect) {
  uint8_t A = (PIND >> PD2) & 1;
  uint8_t B = (PIND >> PD3) & 1;

  if (A != B)
    encoder_pos++; 
  else
    encoder_pos--;
}



//  FUNCIÓN PARA CONTROLAR MOTOR 
void moverMotor(long error) {

  // Determinar dirección
  if (error > 0) {
    PORTB |=  (1 << PB0);
    PORTB &= ~(1 << PB1);
  }
  else if (error < 0) {
    PORTB |=  (1 << PB1);
    PORTB &= ~(1 << PB0);
  }
  else {
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
  }

  // Calcular magnitud (PWM)
  uint16_t pwm = abs(error) * 5;
  if (pwm > 1023) pwm = 1023;

  OCR1B = pwm;
}



// BUCLE PRINCIPAL 
void loop() {

  if (Serial.available()) {
    setpoint = Serial.parseInt();
  }

  long pos = encoder_pos;
  long error = setpoint - pos;

  moverMotor(error);

  // Mostrar datos para pruebas
  Serial.print("Posición = ");
  Serial.print(pos);
  Serial.print("  | Setpoint = ");
  Serial.print(setpoint);
  Serial.print("  | Error = ");
  Serial.println(error);

  delay(50);
}
