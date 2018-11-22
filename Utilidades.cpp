#include "Utilidades.hpp"
#include <EEPROM.h>
#include <Arduino.h>


void configura_PWM()
{
  pinMode(SERVO, OUTPUT);
  // Modo 'Phase correct PWM'
  // OCR2A marca o topo do timer
  // A freqência é aproximadamente CLK/1024/OCR2A/2
  TCCR2A = _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS22) | _BV(CS21) | _BV(CS20);
  OCR2A = 155; // Aproximadamente 50Hz
}

void duty_cycle(byte duty)
{
  OCR2B = duty;
}

int compara_senhas(const char *senha1, const char *senha2, int tamanho)
{
  return (memcmp(senha1, senha2, tamanho) == 0);
}

// Obtém senha da EEPROM ou usa senha padrão '123456'
void prepara_senha(byte *senha, int tamanho)
{
  if (EEPROM.read(0) == NUMERO_MAGICO) // Se houver uma senha personalizada
  {
    le_eeprom(senha, tamanho, 1);
  }
  else
  {
    for (int i = 0; i < tamanho; ++i)
      senha[i] = i + 1 + '0';
  }
}

void le_eeprom(byte *dados, int tamanho, int inicio)
{
  for (int i = 0; i < tamanho; ++i)
    dados[i] = EEPROM.read(i + inicio);
}

void grava_senha(byte *senha, int tamanho)
{
  noInterrupts();
  EEPROM.update(0, NUMERO_MAGICO);
  for (int i = 0; i < tamanho; ++i)
    EEPROM.update(i + 1, senha[i]);
  interrupts();
}
