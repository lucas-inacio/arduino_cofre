#ifndef TECLADO_H
#define TECLADO_H

#ifdef __cplusplus
extern "C" {
#endif

// Inicializa o teclado matricial mapeando as posições em "portas_*"
// aos caracteres fornecidos por "matriz"
void configura_teclado(
  const char *matriz, int nlinhas, int ncolunas,
  const int *portas_linhas, const int *portas_colunas);

// Checa se a entrada "botao" permanece no estado "nivel" 
// por pelo menos milli milisegundos
int checa_botao_por(int botao, long milli, int nivel);

// Retorna 1 se, dentro do tempo "milli" especificado, o "botao"
// esiver no estado "nivel" (retorna assim que digitalRead(botao) == nivel)
// Retorna 0 se o tempo se esgotar
int espera_botao_por(int botao, long milli, int nivel);

// Lê o teclado matricial e retorna o caractere equivalente
// previamente mapeado pela função configura_teclado
char le_botao();

#ifdef __cplusplus
}
#endif

#endif // TECLADO_H
