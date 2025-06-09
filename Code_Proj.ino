//Importar bibliotecas para cada um dos componentes-chave do robô
#include <AFMotor.h> //Biblioteca com funções para utilizar nos motores
#include <IRremote.h> //Biblioteca com funções para utilizar com o recetor e o comando IV
#include <NewPing.h> //Biblioteca com funções para utilizar com os sonares

//Definir o detetor de obstáculos do robô
NewPing Sonar(13, 13, 200);

//Pinos dos fotodíodos
int SensorR = A1;
int SensorL = A0;

//Variáveis para guardar a informação recebida pelos fotodíodos
int dadosR;
int dadosL;

int IR_Receiver = 2; //Pino recetor do sensor IV
IRrecv irrecv(IR_Receiver); //Criar uma variável para "ler" os códigos IV emitidos pelo comando...
decode_results results; //... e uma para armazenar o valor descodificado
long unsigned int mode; //Armazenador temporário do código lido pelo sensor IV

//Definir os motores do robô
AF_DCMotor motorL(3);
AF_DCMotor motorR(4);

//variáveis que armazenam o valor do código dos botões correspondentes às diferentes ações do robô
const long int seguir_fonte = 4194695940; //Botão CH ^
const long int detetar_obs = 2790914820; //Botão CH v

void setup() {
  //DEBUG: preparar o monitor série para mostrar a informação lida
  Serial.begin(9600);

  //Definir as velocidades dos motores
  motorL.setSpeed(170);
  motorR.setSpeed(125);

  IrReceiver.begin(IR_Receiver, ENABLE_LED_FEEDBACK); //Ativar o sensor de infravermelhos

  mode = seguir_fonte; //Preparar o robô para seguir a fonte de luz, por defeito
}

void loop()
{
  delay(50); //Um pequeno intervalo entre cada iteração do programa principal

  float dist = Sonar.ping_cm(); //Distância medida pelo sonar

  //Valores lidos pelos fotodíodos
  dadosR = analogRead(SensorR);
  dadosL = analogRead(SensorL);

  //DEBUG: Mostrar os valores lidos pelos sensores no monitor série
  /*Serial.print("Sensor Esquerdo: ");
  Serial.print(dadosL);
  Serial.print(", Sensor Direito: ");
  Serial.print(dadosR);
  Serial.print(", Distância medida: ");
  Serial.println(dist);*/

  //Receção e leitura de dados infravermelhos
  if (IrReceiver.decode()) //Se for recebida alguma informação, descodifica-se, ...
  {
    long int deCode = IrReceiver.decodedIRData.decodedRawData; //... e armazena-se a informação descodificada numa variável long int
    Serial.println(IrReceiver.decodedIRData.decodedRawData); //DEBUG: Verificar se estão a ser recebidos os códigos corretos
    IrReceiver.resume(); //Continuar a captura de sinais IV...

    //... ignorando o código de repetição do comando...
    if(deCode != 0)
    {
      mode = deCode; //... e armazenando o código pretendido na variável definida anteriormente
    }
  }

  //Diferentes ações requerem diferentes botões, logo...
  //... caso se pretenda que o robô detete e evite obstáculos...
  if(mode == detetar_obs)
  {
    //Ele anda em frente...
    motorL.run(FORWARD);
    motorR.run(FORWARD);

    if(dist < 10) //... até se detetar algo a 10 cm ou menos do sonar...
    {
      Serial.println("OBSTÁCULO DETETADO!");
      Serial.println("VIRAR!");

      //... e ele desvia-se
      motorL.run(RELEASE);
      motorR.run(FORWARD);
    }
  }

  //Caso se queira que ele siga uma fonte de luz...
  else if(mode == seguir_fonte)
  {
    if((dadosL > 650 && dadosR > 650) || (dadosL < 450 && dadosR < 450)) //Caso a fonte de luz se encontre muito próxima ou distante dos sensores...
    {
      //... o robô imobiliza-se
      motorL.run(RELEASE);
      motorR.run(RELEASE);
    }
    //Caso a fonte de luz se encontre mais próxima de um dos sensores, o robô vira-se para essa direção
    else if(dadosR - dadosL >= 150)
    {
      motorR.run(FORWARD);
      motorL.run(RELEASE);
    }
    else if(dadosL - dadosR >= 150)
    {
      motorL.run(FORWARD);
      motorR.run(RELEASE);
    }
    //Se nenhuma das condições anteriores se aplicar...
    else
    {
      //...o robô move-se em frente
      motorL.run(FORWARD);
      motorR.run(FORWARD);
    }
  }
  else //Caso se pressione algum outro botão do comando, o robô imobiliza-se
  {
    motorL.run(RELEASE);
    motorR.run(RELEASE);
  }
}