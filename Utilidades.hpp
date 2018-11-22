#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <Arduino.h>

#define SERVO         9
#define NUMERO_MAGICO 0xA3 // Indica se há dados na EEPROM

// Configura o pino 9 para aproximadamente 50Hz
void configura_PWM();
void duty_cycle(byte duty); // 0 - 155
// Retorna um valor diferente de 0 se as senhas forem iguais
int compara_senhas(const char *senha1, const char *senha2, int tamanho);
// Obtém senha da EEPROM ou usa senha padrão '123456'
void prepara_senha(byte *senha, int tamanho);
// Lê dados da EEPROM a partir de "inicio"
void le_eeprom(byte *dados, int tamanho, int inicio);
void grava_senha(byte *senha, int tamanho);

#endif // UTILIDADES_H
