#include "TecladoMatricial.h"
#include "LCD.h"
#include "Utilidades.hpp"
#include <EEPROM.h>
#include <string.h>


// ESTADO DA APLICAÇÃO
#define TRANCADO          16 // Pulso de 2ms aproximadamente
#define ABERTO            7  // Pulso de 1ms aproximadamente

#define STATUS_VIOLADO     1
#define STATUS_FECHADO     2
#define STATUS_ABERTO      3
#define STATUS_TROCA_SENHA 4
#define STATUS_PENALIDADE  5

// LIMITES
#define LINHAS            4
#define COLUNAS           3
#define LIMITE_TENTATIVAS 3
#define TEMPO_PENALIDADE  60
#define TAMANHO_SENHA     6

// PINOS
#define FIM_DE_CURSO      21
#define SERVO             9  // OC2B PWM

const int PORTAS_LINHAS[] = { 22, 24, 26, 28 };
const int PORTAS_COLUNAS[] = { 30, 32, 34 };

const char CARACTERES[LINHAS][COLUNAS] = {
    { '1', '2', '3' },
    { '4', '5', '6' },
    { '7', '8', '9' },
    { '*', '0', '#' }
  };

// Utilizadas na função loop_fechado
byte senha_atual[TAMANHO_SENHA] = { 0 };

// Utilizadas por todo o programa
struct LCD lcd;
byte status_atual;

// Utilizadas na função loop_penalidade
long ultimo_instante; // Atualizada em loop_fechado e loop_violado loop_penalidade

void rotina_isr()
{
  // Se o cofre for aberto de forma válida não deve
  // entrar em alerta
  if (status_atual == STATUS_ABERTO)
    return;
  status_atual = STATUS_VIOLADO;
}

// Trava o cofre
int fecha_cofre()
{
  duty_cycle(TRANCADO);
  delay(1000);
  return STATUS_FECHADO;
}

// Abre o cofre
int abre_cofre()
{
  duty_cycle(ABERTO);
  delay(1000);
  return STATUS_ABERTO;
}

// Garante que o cofre se encontra num estado conhecido
// (fechado e com a senha definida - padrão ou da EEPROM)
byte inicia_cofre(byte *senha)
{
  pinMode(FIM_DE_CURSO, INPUT_PULLUP);
  pinMode(SERVO, OUTPUT);
  prepara_senha(senha, TAMANHO_SENHA);
  return (digitalRead(FIM_DE_CURSO)) ? STATUS_ABERTO : STATUS_FECHADO;
}

// A variável "inicio" indica se a função foi chamada
// pela primeira vez dentro do loop (se houve transição de estado)
void loop_fechado(int inicio)
{
  static int tentativas = 0;
  static byte numeros_digitados[TAMANHO_SENHA] = { 0 };
  static int conta_digitados = 0;
  if (inicio)
  {
    prompt(&lcd, "Senha");
    conta_digitados = 0;
    tentativas = 0;
    prepara_senha(senha_atual, TAMANHO_SENHA);
  }
  char c = le_botao();
  if (c != 0 && c != '#')
  {
    if (c == '*')
    {
      status_atual = STATUS_TROCA_SENHA;
      return;
    }
    WriteLCD(&lcd, '*');
    numeros_digitados[conta_digitados] = c;
    ++conta_digitados;
    if (conta_digitados >= TAMANHO_SENHA)
    {
      conta_digitados = 0;
      // Verifica senha
      if (compara_senhas(numeros_digitados, senha_atual, TAMANHO_SENHA))
      {
        // Essa parte é vital para evitar que o programa entre
        // em estado de alerta no momento errado
        noInterrupts();
        status_atual = STATUS_ABERTO;
        interrupts();
        abre_cofre();
      }
      else
      {
        prompt(&lcd, "Erro!");
        delay(1000);
        ++tentativas;
        if (tentativas >= LIMITE_TENTATIVAS)
        {
          ultimo_instante = millis();
          status_atual = STATUS_PENALIDADE;
        }
        else
        {
          prompt(&lcd, "Senha");
        }
      }
    }
  }
}

void loop_penalidade(int inicio)
{
  static int segundos = TEMPO_PENALIDADE;
  long delta_segundos = (millis() - ultimo_instante);
  if ((delta_segundos >= 1000))
  {
    segundos -= delta_segundos / 1000;
    ultimo_instante = millis();
    if (segundos > 0)
    {
      ClearLCD(&lcd);
      WriteLCD(&lcd, (segundos / 10) + '0');
      WriteLCD(&lcd, (segundos % 10) + '0');
    }
  }
  if (segundos <= 0)
  {
    status_atual = STATUS_FECHADO;
    segundos = TEMPO_PENALIDADE;
  }
}

void loop_aberto(int inicio)
{
  if (inicio)
    prompt(&lcd, "Aberto");
  if (digitalRead(FIM_DE_CURSO) == LOW)
  {
    fecha_cofre();
    status_atual = STATUS_FECHADO;
  }
}

void loop_violado(int inicio)
{
  if (inicio)
    prompt(&lcd, "Violado!");
  if (digitalRead(FIM_DE_CURSO) == LOW) // Voltou a fechar
  {
    // Volta ao estado anterior
    fecha_cofre();
    //status_atual = ultimo_status;
    status_atual = STATUS_FECHADO;
  } 
}

void loop_troca_senha(int inicio)
{
  static char nova_senha[TAMANHO_SENHA] = { 0 };
  static char senha[TAMANHO_SENHA] = { 0 };
  static byte conta_senha = 0;
  // 0 - Nova senha; 1 - Repete; 2 - Senha atual
  static byte passo = 0;
  if (inicio)
  {
    prompt(&lcd, "Senha nova");
    conta_senha = 0;
    passo = 0;
  }
  char c = le_botao();
  if (c != 0 && c != '*')
  {
    if (c == '#') // Retorna
    {
      status_atual = STATUS_FECHADO;
      return;
    }
    WriteLCD(&lcd, '*');
    if (passo == 0)
    {
      nova_senha[conta_senha] = c;
      ++conta_senha;
      if (conta_senha >= TAMANHO_SENHA)
      {
        prompt(&lcd, "Repita");
        conta_senha = 0;
        passo = 1;
      }
    }
    else if (passo == 1)
    {
      if (c != nova_senha[conta_senha])
      {
        prompt(&lcd, "Erro!");
        delay(1000);
        status_atual = STATUS_FECHADO;
      }

      ++conta_senha;
      if (conta_senha == TAMANHO_SENHA)
      {
        conta_senha = 0;
        prompt(&lcd, "Senha atual");
        passo = 2;
      }
    }
    else
    {
      senha[conta_senha] = c;
      ++conta_senha;
      if (conta_senha >= TAMANHO_SENHA)
      {
        if (compara_senhas(senha, senha_atual, TAMANHO_SENHA))
        {
          grava_senha(nova_senha, TAMANHO_SENHA);
        }
        else
        {
          prompt(&lcd, "Erro!");
          delay(1000);
        }
        status_atual = STATUS_FECHADO;
      }
    }
  }
}

void setup() {
  InitLCD(&lcd, 8, 10, 7, 6, 5, 4);
  configura_teclado((char *)CARACTERES, LINHAS,
                    COLUNAS, PORTAS_LINHAS,
                    PORTAS_COLUNAS);
  configura_PWM();
  duty_cycle(TRANCADO);
  attachInterrupt(
    digitalPinToInterrupt(FIM_DE_CURSO), rotina_isr, RISING);
  
  status_atual = inicia_cofre(senha_atual);
}

void loop() {
  static byte _status = status_atual;
  static int houve_transicao = 1;
  switch (status_atual)
  {
  case STATUS_FECHADO:
    loop_fechado(houve_transicao);
    break;
  case STATUS_ABERTO:
    loop_aberto(houve_transicao);
    break;
  case STATUS_PENALIDADE:
    loop_penalidade(houve_transicao);
    break;
  case STATUS_TROCA_SENHA:
    loop_troca_senha(houve_transicao);
    break;
  case STATUS_VIOLADO:
    loop_violado(houve_transicao);
    break;
  }
  houve_transicao = _status != status_atual;
  _status = status_atual;
}
