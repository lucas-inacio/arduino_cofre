#include "LCD.h"
#include <stdlib.h>
#include <Arduino.h>


// Envia o nibble alto seguido no nibble baixo para o display
void WriteByteLCD(const struct LCD *lcd, char value)
{
  WriteNibbleLCD(lcd, (value >> 4) & 0x0F);
  WriteNibbleLCD(lcd, value & 0x0F);
}

// Envia um nibble para o display
void WriteNibbleLCD(const struct LCD *lcd, char value)
{
  digitalWrite(lcd->DB7, (value >> 3) & 0x01);
  digitalWrite(lcd->DB6, (value >> 2) & 0x01);
  digitalWrite(lcd->DB5, (value >> 1) & 0x01);
  digitalWrite(lcd->DB4, value & 0x01);
  ClockLCD(lcd);
}

void ClockLCD(const struct LCD *lcd)
{
  delay(2);
  digitalWrite(lcd->EN, HIGH);
  delay(2);
  digitalWrite(lcd->EN, LOW);
  delay(2);
}

// Seleciona o registrador de dados
void SelectDataLCD(const struct LCD *lcd)
{
  digitalWrite(lcd->RS, HIGH);
}

// Seleciona o registrador de instruções
void SelectInstructionLCD(const struct LCD *lcd)
{
  digitalWrite(lcd->RS, LOW);
}

// Configura o display LCD
void InitLCD(struct LCD *lcd, int RS, int EN, int DB7, int DB6, int DB5, int DB4)
{
  lcd->RS = RS;
  lcd->EN = EN;
  lcd->DB7 = DB7;
  lcd->DB6 = DB6;
  lcd->DB5 = DB5;
  lcd->DB4 = DB4;

  pinMode(lcd->RS, OUTPUT);
  pinMode(lcd->EN, OUTPUT);
  pinMode(lcd->DB7, OUTPUT);
  pinMode(lcd->DB6, OUTPUT);
  pinMode(lcd->DB5, OUTPUT);
  pinMode(lcd->DB4, OUTPUT);

  delay(50);

  // Configura display para trabalhar com 4 vias de dados
  SelectInstructionLCD(lcd);
  WriteNibbleLCD(lcd, 0x02); // Ativa o display para o modo de 4 bits
  WriteByteLCD(lcd, 0x28);   // quatro bits; duas linhas
  WriteByteLCD(lcd, 0x0C);   // Liga display
  WriteByteLCD(lcd, 0x06);   // Deslocamento automático do cursor da direita para a esquerda. Scroll ligado.
  WriteByteLCD(lcd, 0x01);   // Limpa o display.
  WriteByteLCD(lcd, 0x80);   // Coloca o cursor no início da primeira linha
}

// Assume o uso de um display de 2 linhas e 16 colunas
void SetCursorLCD(const struct LCD *lcd, char line, char col)
{
  char comando = (line > 0) ? 0xC0 : 0x80;
  comando |= col;
  SelectInstructionLCD(lcd);
  WriteByteLCD(lcd, comando);
}

// Desloca o display. RL = 1 para direita ou 0 para esquerda
void ShiftDisplayLCD(const struct LCD *lcd, int RL)
{
  char comando = (RL > 0) ? 0x1C : 0x18;
  SelectInstructionLCD(lcd);
  WriteByteLCD(lcd, comando);
}

void ClearLCD(const struct LCD *lcd)
{
  SelectInstructionLCD(lcd);
  WriteByteLCD(lcd, 0x01);
}

// Imprime uma string no display (deve terminar com caractere nulo)
void PrintLCD(const struct LCD *lcd, const char *str)
{
  SelectDataLCD(lcd);
  char c;
  while ((c = *str++))
    WriteByteLCD(lcd, c);
}

void WriteLCD(const struct LCD *lcd, char c)
{
  SelectDataLCD(lcd);
  WriteByteLCD(lcd, c);
}

void prompt(struct LCD *lcd, const char *frase)
{
  ClearLCD(lcd);
  PrintLCD(lcd, frase);
  SetCursorLCD(lcd, 1, 0);
}
