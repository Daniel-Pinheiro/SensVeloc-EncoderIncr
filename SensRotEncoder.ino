#define segsTotal 30
#define pinoSinal 2     //Frequência. máx.(Atmega328p):     9kHz
#define pulsopVolta 4   //Medição máx.: freq x 60/PpV = 135000RPM

unsigned long tempoAnterior=0, tempoAtual=0, tempoTotal=0;
char mudou=0;

long X[4]={ 1000000, 1000000, 1000000, 1000000 };
long Y[4]={ 1000000, 1000000, 1000000, 1000000 };
const long a[4]={ 512, -1026, 741, -185 };  //LPF Buttworth
const long b[4]={ 5, 16, 16, 5 };    // fc = fs*8/50

void setup() {
  Serial.begin(9600);
  pinMode(pinoSinal, INPUT);
  pinMode(8, OUTPUT);

  while (!Serial);
  attachInterrupt(digitalPinToInterrupt(pinoSinal), borda, RISING);
}

// Função para o Interrupt
void borda() {
#if  defined(__AVR_ATmega328P__)
  PORTB |= 1;
#endif
  tempoAnterior = tempoAtual;
  tempoAtual = micros();
  long deltaT = tempoAtual-tempoAnterior;

  if( deltaT>100 && deltaT<2000000 )
  {
    X[3] = X[2];
    X[2] = X[1];
    X[1] = X[0];
    X[0] = deltaT;

    Y[3] = Y[2];
    Y[2] = Y[1];
    Y[1] = Y[0];
    Y[0] =          - a[1]*Y[1] - a[2]*Y[2] - a[3]*Y[3]
      +   b[0]*X[0] + b[1]*X[1] + b[2]*X[2] + b[3]*X[3];
    Y[0] = (Y[0]/a[0]);

    tempoTotal += deltaT;
    mudou=1;
  }
#if  defined(__AVR_ATmega328P__)
  PORTB &= ~1;
#endif
}

void loop()
{
  if(mudou)
  {
    float RPM = (60.0*1000000/pulsopVolta)/float(Y[0]);
    Serial.println(RPM);
    //mudou=0;
    /*if(tempoTotal >= segsTotal*1000000)
    {
      detachInterrupt(0);
      mudou=0;
    }*/
  }
  delay(100);
}
