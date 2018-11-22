#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus
extern "C" {
#endif

struct LCD
{
  int RS, EN;
  int DB7, DB6, DB5, DB4;  
};

// Envia o nibble alto seguido no nibble baixo para o display
void WriteByteLCD(const struct LCD *lcd, char value);

// Envia um nibble para o display
void WriteNibbleLCD(const struct LCD *lcd, char value);

void ClockLCD(const struct LCD *lcd);

// Seleciona o registrador de dados
void SelectDataLCD(const struct LCD *lcd);

// Seleciona o registrador de instruções
void SelectInstructionLCD(const struct LCD *lcd);

// Configura o display LCD
void InitLCD(struct LCD *lcd, int RS, int EN, int DB7, int DB6, int DB5, int DB4);

// Assume o uso de um display de 2 linhas e 16 colunas
void SetCursorLCD(const struct LCD *lcd, char line, char col);

// Desloca o display. RL = 1 para direita ou 0 para esquerda
void ShiftDisplayLCD(const struct LCD *lcd, int RL);

void ClearLCD(const struct LCD *lcd);

// Imprime uma string no display (deve terminar com caractere nulo)
void PrintLCD(const struct LCD *lcd, const char *str);

void WriteLCD(const struct LCD *lcd, char c);

void prompt(struct LCD *lcd, const char *frase);

#ifdef __cplusplus
}
#endif

#endif // LCD_H
