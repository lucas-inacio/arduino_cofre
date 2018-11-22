#include "TecladoMatricial.h"
#include <stdlib.h>
#include <Arduino.h>


static char *_caracteres = NULL;
static int *_portas_linhas = NULL;
static int *_portas_colunas = NULL;
static int _linhas = 0;
static int _colunas = 0;

void configura_teclado(
  const char *matriz, int nlinhas, int ncolunas,
  const int *portas_linhas, const int *portas_colunas)
{
  _caracteres = matriz;
  _portas_linhas = portas_linhas;
  _portas_colunas = portas_colunas;
  _linhas = nlinhas;
  _colunas = ncolunas;

  for (int i = 0; i < _linhas; ++i)
  {
    pinMode(_portas_linhas[i], OUTPUT);
    digitalWrite(_portas_linhas[i], HIGH);
  }

  for (int i = 0; i < _colunas; ++i)
    pinMode(_portas_colunas[i], INPUT_PULLUP);
}

int checa_botao_por(int botao, long milli, int nivel)
{
  long marca = millis();
  long conta_tempo = 0;
  do
  {
    long agora = millis();
    conta_tempo += agora - marca;
    marca = agora;
  } while ((conta_tempo < milli) && (digitalRead(botao) == nivel));

  if (conta_tempo >= milli)
    return 1;
  else
    return 0;
}

int espera_botao_por(int botao, long milli, int nivel)
{
  long marca = millis();
  long conta_tempo = 0;
  do
  {
    long agora = millis();
    conta_tempo += agora - marca;
    marca = agora;
  } while ((conta_tempo < milli) && (digitalRead(botao) != nivel));

  if (conta_tempo < milli)
    return 1;
  else
    return 0;
}

char le_botao()
{
  for (int i = 0; i < _linhas; ++i)
  {
    digitalWrite(_portas_linhas[i], LOW);
    for (int j = 0; j < _colunas; ++j)
    {
      if (checa_botao_por(_portas_colunas[j], 150, LOW))
      {
        // Evita falha de leitura dos outros botões caso
        // um botão fique pressionado por muito tempo
        if (espera_botao_por(_portas_colunas[j], 100, HIGH))
          return _caracteres[i * _colunas + j];
      }
    }
    digitalWrite(_portas_linhas[i], HIGH);
  }
  
  return 0;
}
