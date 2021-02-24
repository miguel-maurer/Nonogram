#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "util.h"

#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define TITULO_JANELA "Nonogram"
#define TITULO_MENU "Menu Nonogram"

#define LINHAS 8
#define COLUNAS 8
#define ALTURA_CELULA 50
#define LARGURA_CELULA 50
#define LARGURA_TABULEIRO LARGURA_CELULA * COLUNAS
#define ALTURA_TABULEIRO ALTURA_CELULA * LINHAS
#define MARGIN_HORIZONTAL (int) ((LARGURA_JANELA / 2) - (LARGURA_TABULEIRO / 2))
#define MARGIN_VERTICAL (int) ((ALTURA_JANELA / 2) - (ALTURA_TABULEIRO / 2))
#define LIMT 50

void inicia_tabuleiro(Celula tabuleiro[][COLUNAS]);
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS], int lat[][LINHAS], int sup[][COLUNAS]);
void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS], char gamemode[], int *v, bool *ptr, int coord[]);
void escreve_numeros(Tela *t, int numeros[], int qtd, Ponto inicio, bool horizontal);
void cicla_estado_celula(Celula *c, bool botao_primario);

void menu_do_jogo(Tela *t, Celula tabuleiro[][COLUNAS]);
void solucao_aleatoria(Tela *t, Celula solucao[][COLUNAS]);
void numeros_laterais(Celula solucao[][COLUNAS], int lat[][COLUNAS]);
void numeros_superior(Celula solucao[][COLUNAS], int sup[][COLUNAS]);
void cria_botao(Tela *t, Celula solucao[][COLUNAS], Celula tabuleiro[][COLUNAS], char text[], Ponto p, Tamanho tm, int *v, bool *ptr, int coord[]);
void modo_classico(Tela *t, Celula tabuleiro[][COLUNAS], bool *ptr);
void modo_normal(Tela *t, Celula tabuleiro[][COLUNAS], bool *ptr, int *v);
void venceu_classico(Tela *t, Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS], bool *ptr);
void venceu_normal(Tela *t, Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS], int *v, bool *ptr);
void vidas(Tela *t, int v[]);
void escreve(Tela *t, int *numeros, int qtd, Ponto inicio, bool horizontal);
void contador(Tela *t, Celula solucao[][COLUNAS], Celula tabuleiro[][COLUNAS], int coord[]);
void desfazer(Celula tabuleiro[][COLUNAS], int coord[]);
void solucionado(Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS]);

int main(int argc, char **argv)
{
  Tela t;
  Celula tabuleiro_nonogram[LINHAS][COLUNAS];

  menu_do_jogo(&t, tabuleiro_nonogram);

  finaliza_tela(&t);

  return 0;
}

void menu_do_jogo(Tela *t, Celula tabuleiro[][COLUNAS])
{
  int tecla_pressionada;
  bool mododejogo, *ptr=&mododejogo;
  int vida, *v = &vida;

  inicia_tela(t, LARGURA_JANELA, ALTURA_JANELA, TITULO_MENU);

  while((tecla_pressionada = codigo_tecla(t)) != ALLEGRO_EVENT_DISPLAY_CLOSE){
    limpa_tela(t);

    Ponto p = {325, 250};
    al_draw_text(t->fonte, t->ac_cor, p.x, p.y, ALLEGRO_ALIGN_LEFT, "NONOGRAM");

    Ponto p1 = {200, 430};
    Tamanho tm = {150, 50};
    cria_botao(t, 0, tabuleiro, "Clássico", p1, tm, v, ptr, 0);

    Ponto p2 = {400, 430};
    Tamanho tm2 = {150, 50};
    cria_botao(t, 0, tabuleiro, "Normal", p2, tm2, v, ptr, 0);

    mostra_tela(t);
    espera(30);
  }
}

void solucao_aleatoria(Tela *t, Celula solucao[][COLUNAS])
{
  int i, j, k = 0;
  srand(time(NULL));
  while(k<LIMT){
    i = rand() % LINHAS;
    j = rand() % COLUNAS;
    solucao[i][j].estado = COLORIDO;
    k++;
  }
}

void numeros_superior(Celula solucao[][COLUNAS], int sup[][COLUNAS])
{
  int cont;
  for(int i=0; i<LINHAS; i++){
    cont=0;
    for(int j=0; j<COLUNAS; j++){
      if(solucao[i][j].estado == COLORIDO){
        cont++;
        if(solucao[i][j+1].estado == COLORIDO) sup[i][j] = 0;
        else sup[i][j] = cont;
        if(j == COLUNAS-1) sup[i][j] = cont;
      }else{
        cont=0;
        sup[i][j] = 0;
      }
    }
  }
}

void numeros_laterais(Celula solucao[][COLUNAS], int lat[][COLUNAS])
{
  int cont=0;
  for(int j=0; j<COLUNAS; j++){
    cont=0;
    for(int i=0; i<LINHAS; i++){
      if(solucao[i][j].estado == COLORIDO){
        cont++;
        if(solucao[i+1][j].estado == COLORIDO) lat[i][j] = 0;
        else lat[i][j] = cont;
        if(i == LINHAS-1) lat[i][j] = cont;
      }else{
        cont=0;
        lat[i][j] = cont;
      }
    }
  }
}

void modo_classico(Tela *t, Celula tabuleiro[][COLUNAS], bool *ptr)
{
  int tecla_press, coord[2], *v=0;
  int lat[LINHAS][COLUNAS], sup[LINHAS][COLUNAS];

  Celula solucao[LINHAS][COLUNAS];

  inicia_tela(t, LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);
  inicia_tabuleiro(tabuleiro);
  inicia_tabuleiro(solucao);

  solucao_aleatoria(t, solucao);

  numeros_laterais(solucao, lat);
  numeros_superior(solucao, sup);

  while((tecla_press = codigo_tecla(t)) != ALLEGRO_EVENT_DISPLAY_CLOSE){
    *ptr = 0;

    limpa_tela(t);

    desenha_tabuleiro(t, tabuleiro, lat, sup);
    contador(t, solucao, tabuleiro, coord);

    Ponto p = {200, 625};
    Tamanho tm = {150, 50};
    cria_botao(t, solucao, tabuleiro, "Limpar Tabuleiro", p, tm, v, ptr, coord);

    Ponto p1 = {450, 625};
    cria_botao(t, solucao, tabuleiro, "Novo Jogo", p1, tm, v, ptr, coord);

    Ponto p2 = {650, 630};
    Tamanho tm2 = {100, 40};
    cria_botao(t, solucao, tabuleiro, "Desfazer", p2, tm2, v, ptr, coord);

    Ponto p3 = {50, 630};
    Tamanho tm3 = {100, 40};
    cria_botao(t, solucao, tabuleiro, "Solucionar", p3, tm3, v, ptr, coord);

    Ponto p4 = {50, 50};
    Tamanho tm4 = {70, 40};
    cria_botao(t, solucao, tabuleiro, "Menu", p4, tm4, v, ptr, coord);

    verifica_clique(t, tabuleiro, solucao, "classico", v, ptr, coord);

    mostra_tela(t);

    espera(30);
  }
  finaliza_tela(t);
}
void modo_normal(Tela *t, Celula tabuleiro[][COLUNAS], bool *ptr, int *v)
{
  int tecla_press, coord[2];
  int lat[LINHAS][COLUNAS], sup[LINHAS][COLUNAS];
  Celula solucao[LINHAS][COLUNAS];

  *v = 3;

  inicia_tela(t, LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);
  inicia_tabuleiro(tabuleiro);
  inicia_tabuleiro(solucao);

  solucao_aleatoria(t, solucao);

  numeros_laterais(solucao, lat);
  numeros_superior(solucao, sup);

  while((tecla_press = codigo_tecla(t)) != ALLEGRO_EVENT_DISPLAY_CLOSE){
    *ptr = 1;

    limpa_tela(t);

    desenha_tabuleiro(t, tabuleiro, lat, sup);

    Ponto p = {200, 625};
    Tamanho tm = {150, 50};
    cria_botao(t, solucao, tabuleiro, "Limpar Tabuleiro", p, tm, v, ptr, coord);

    Ponto p1 = {450, 625};
    cria_botao(t, solucao, tabuleiro, "Novo Jogo", p1, tm, v, ptr, coord);

    Ponto p3 = {50, 630};
    Tamanho tm3 = {100, 40};
    cria_botao(t, solucao, tabuleiro, "Solucionar", p3, tm3, v, ptr, coord);

    Ponto p4 = {50, 50};
    Tamanho tm4 = {70, 40};
    cria_botao(t, solucao, tabuleiro, "Menu", p4, tm4, v, ptr, coord);

    verifica_clique(t, tabuleiro, solucao, "normal", v, ptr, coord);
    vidas(t, v);

    mostra_tela(t);

    espera(30);
  }
  finaliza_tela(t);
}

void cria_botao(Tela *t, Celula solucao[][COLUNAS], Celula tabuleiro[][COLUNAS], char text[], Ponto p, Tamanho tm, int *v, bool *ptr, int coord[])
{
  float x = p.x, y = p.y, altura = tm.alt, largura = tm.larg;
  Celula a = { { x, y }, {altura, largura}, VAZIO };
  float endx = a.pos.x + a.tam.alt;
  float endy = a.pos.y + a.tam.larg;

  al_draw_rounded_rectangle(a.pos.x, a.pos.y, endx, endy, 5, 5, t->ac_cor, 2);

  al_draw_text(t->fonte, t->ac_cor, a.pos.x+5, a.pos.y+15, ALLEGRO_ALIGN_LEFT, text);

  Ponto mouse = posicao_mouse(t);
  int mouse1 = botao_clicado(t);

  if(mouse1){
    int x_click_botao = (mouse.x);
    int y_click_botao = (mouse.y);

    if((x_click_botao >= x && y_click_botao >= y) && (x_click_botao <= endx && y_click_botao <= endy)){

      if(strcmp(text, "Limpar Tabuleiro")==0){
        for(int i=0; i<LINHAS; i++)
          for(int j=0; j<COLUNAS; j++)
            tabuleiro[i][j].estado = VAZIO;
        if(*ptr)
          *v = 3;
     }else if(strcmp(text, "Novo Jogo")==0){
        if(*ptr){
          finaliza_tela(t);
          modo_normal(t, tabuleiro, ptr, v);
        }else{
          finaliza_tela(t);
          modo_classico(t, tabuleiro, ptr);
        }
      }else if(strcmp(text, "Clássico")==0){
        finaliza_tela(t);
        modo_classico(t, tabuleiro, ptr);
      }else if(strcmp(text, "Normal")==0){
        finaliza_tela(t);
        modo_normal(t, tabuleiro, ptr, v);
      }else if(strcmp(text, "Desfazer")==0){
        if((coord[0] >= 0 && coord[0] < LINHAS) && (coord[1] >= 0 && coord[1] < COLUNAS))
          desfazer(tabuleiro, coord);
      }else if(strcmp(text, "Solucionar")==0){
        solucionado(tabuleiro, solucao);
      }else if(strcmp(text, "Menu")==0){
        finaliza_tela(t);
        menu_do_jogo(t, tabuleiro);
      }
    }
  }
}

void solucionado(Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS])
{
  for(int i=0; i<LINHAS; i++)
    for(int j=0; j<COLUNAS; j++)
      tabuleiro[i][j].estado = solucao[i][j].estado;
}

void venceu_classico(Tela *t, Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS], bool *ptr)
{
  int cont=0, cont1=0, total_solucao=0;
  for(int k=0; k<LINHAS; k++)
    for(int l=0; l<COLUNAS; l++){
      if(solucao[k][l].estado == COLORIDO)
        total_solucao++;
      if(solucao[k][l].estado == tabuleiro[k][l].estado && tabuleiro[k][l].estado == COLORIDO)
        cont++;
      else if(tabuleiro[k][l].estado == VAZIO || tabuleiro[k][l].estado == ALERTA)
        cont1++;
    }
  if(cont == total_solucao && cont1 == LINHAS*COLUNAS-total_solucao){
    char tcaixa[] = "Parabens!!!";
    char titulo[] = "Voce ganhou o jogo!";
    char texto[] = "Deseja jogar novamente?";
    if(al_show_native_message_box(t->display,tcaixa,titulo,texto,NULL,ALLEGRO_MESSAGEBOX_YES_NO)==1){
      finaliza_tela(t);
      modo_classico(t, tabuleiro, ptr);
    }
    mostra_tela(t);
  }
}

void venceu_normal(Tela *t, Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS], int *v, bool *ptr)
{
  int cont=0, cont1=0, total_solucao=0;
  for(int i=0; i<LINHAS; i++)
    for(int j=0; j<COLUNAS; j++){
      if(solucao[i][j].estado == COLORIDO)
        total_solucao++;
      if(tabuleiro[i][j].estado == COLORIDO && tabuleiro[i][j].estado != solucao[i][j].estado){
        *v=*v-1;
        tabuleiro[i][j].estado = ALERTA;
      }else if(solucao[i][j].estado == COLORIDO && tabuleiro[i][j].estado == ALERTA){
        *v=*v-1;
        tabuleiro[i][j].estado = COLORIDO;
      }else if(tabuleiro[i][j].estado == COLORIDO && tabuleiro[i][j].estado == solucao[i][j].estado)
        cont++;
      else if(tabuleiro[i][j].estado == VAZIO || tabuleiro[i][j].estado == ALERTA)
        cont1++;
    }
  if(*v < 1){
    char titulo[] = "Acabaram suas vidas!";
    char tcaixa[] = "Voce perdeu o jogo :(";
    char texto[] = "Deseja jogar novamente?";
    if(al_show_native_message_box(t->display,tcaixa,titulo,texto,NULL,ALLEGRO_MESSAGEBOX_YES_NO)==1){
      finaliza_tela(t);
      modo_normal(t, tabuleiro, ptr, v);
    }
    mostra_tela(t);
  }else if(cont == total_solucao && cont1 == LINHAS*COLUNAS-total_solucao){
    char tcaixa[] = "Parabens!!!";
    char titulo[] = "Voce ganhou o jogo!";
    char texto[] = "Deseja jogar novamente?";
    if(al_show_native_message_box(t->display,tcaixa,titulo,texto,NULL,ALLEGRO_MESSAGEBOX_YES_NO)==1){
      finaliza_tela(t);
      modo_normal(t, tabuleiro, ptr, v);
    }
    mostra_tela(t);
  }
}

void vidas(Tela *t, int *v)
{
  Ponto centro = {350, 50};
  switch(*v){
        case 3:
          for(int i=0; i<150; i+=50)
            al_draw_filled_circle(centro.x+i, centro.y, 10, al_map_rgb(255, 77, 77));
          break;
        case 2:
          for(int i=0; i<150; i+=50){
            al_draw_filled_circle(centro.x+i, centro.y, 10, al_map_rgb(255, 77, 77));
            if(i == 100)
              al_draw_filled_circle(centro.x+i, centro.y, 10, al_map_rgb(140, 140, 140));
          }
          break;
        case 1:
          for(int i=0; i<150; i+=50){
            al_draw_filled_circle(centro.x+i, centro.y, 10, al_map_rgb(255, 77, 77));
            if(i == 100 || i == 50)
              al_draw_filled_circle(centro.x+i, centro.y, 10, al_map_rgb(140, 140, 140));
          }
          break;
        case 0:
          for(int i=0; i<150; i+=50)
            al_draw_filled_circle(centro.x+i, centro.y, 10, al_map_rgb(140, 140, 140));
  }
}

void contador(Tela *t, Celula solucao[][COLUNAS], Celula tabuleiro[][COLUNAS], int coord[])
{
  int total=0, pintados=0;
  for(int i=0; i<LINHAS; i++)
    for(int j=0; j<COLUNAS; j++){
      if(solucao[i][j].estado == COLORIDO)
        total++;
      if(tabuleiro[i][j].estado == COLORIDO)
        pintados++;
    }
  int x = 400, y = 50;
  al_draw_textf(t->fonte, al_map_rgb(0, 0, 0), x, y, ALLEGRO_ALIGN_CENTRE, "%d/%d", pintados, total);
  if(pintados>total){
    char tcaixa[] = {"Aviso!"};
    char titulo[] = {"Você não encontrou a solução."};
    char msg[] = {"Você excedeu a quantidade correta de células pintadas."};
    al_show_native_message_box(t->display,tcaixa,titulo,msg,NULL,ALLEGRO_MESSAGEBOX_WARN);
    desfazer(tabuleiro, coord);
  }
}

void desfazer(Celula tabuleiro[][COLUNAS], int coord[])
{
  int x = coord[0];
  int y = coord[1];
  if(tabuleiro[x][y].estado == COLORIDO)
    tabuleiro[x][y].estado = VAZIO;
  else if(tabuleiro[x][y].estado == ALERTA)
    tabuleiro[x][y].estado = VAZIO;

}

void inicia_tabuleiro(Celula tabuleiro[][COLUNAS]) {
  for(int i = 0; i < LINHAS; i++) {
    for(int j = 0; j < COLUNAS; j++) {

      int x = MARGIN_HORIZONTAL + (LARGURA_CELULA * i);
      int y = MARGIN_VERTICAL + (ALTURA_CELULA * j);

      Celula c = { { x, y }, { ALTURA_CELULA, LARGURA_CELULA }, VAZIO };
      tabuleiro[i][j] = c;
    }
  }
}
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS], int lat[][COLUNAS], int sup[][COLUNAS]) {
  Cor preto = {0, 0, 0};
  Cor vermelho = {1, 0, 0};
  define_cor(t, preto);
  for(int i = 0; i < LINHAS; i++){
    for(int j = 0; j < COLUNAS; j++){

      desenha_retangulo(tabuleiro[i][j], t, tabuleiro[i][j].estado == COLORIDO);

      if(tabuleiro[i][j].estado == ALERTA) {
        define_cor(t, vermelho);
        desenha_x_dentro_ret(tabuleiro[i][j], t);
        define_cor(t, preto);
      }
    }
  }

  int aux, offset = 20;
  for(int i = LINHAS-1; i >= 0; i--) {
    int x = MARGIN_HORIZONTAL - 20;
    int y = MARGIN_VERTICAL + (i * ALTURA_CELULA) + 15;

    for(int l=COLUNAS-1; l>=0; l--){
      if(lat[l][i] != 0){
        aux = lat[l][i];
        if(l<COLUNAS-1)
          al_draw_textf(t->fonte, t->ac_cor, x, y, ALLEGRO_ALIGN_CENTRE, "%d, ", aux);
        else
          al_draw_textf(t->fonte, t->ac_cor, x, y, ALLEGRO_ALIGN_CENTRE, "%d ", aux);
        x-=offset;
      }
    }
  }

  int aux1, offset1 = 20;
  for(int i = LINHAS-1; i >= 0; i--){
    int x = MARGIN_HORIZONTAL + (i * LARGURA_CELULA) + 20;
    int y = MARGIN_VERTICAL - 25;

    for(int l = COLUNAS-1; l >= 0; l--){
      if(sup[i][l] != 0){
        aux1 = sup[i][l];
        al_draw_textf(t->fonte, t->ac_cor, x, y, ALLEGRO_ALIGN_CENTRE, "%d", aux1);
        y-=offset1;
      }
    }
  }
}

void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS], Celula solucao[][COLUNAS], char gamemode[], int *v, bool *ptr, int coord[]) {
  Ponto mouse = posicao_mouse(t);
  int botao = botao_clicado(t);
  if(botao) {
    int x_clique_tabuleiro = (mouse.x - MARGIN_HORIZONTAL);
    int y_clique_tabuleiro = (mouse.y - MARGIN_VERTICAL);

    if(x_clique_tabuleiro >= 0 && y_clique_tabuleiro >= 0) {

      int i = (int) (x_clique_tabuleiro / ALTURA_CELULA);
      int j = (int) (y_clique_tabuleiro / LARGURA_CELULA);

      if(i >= 0 && i < LINHAS && j >=0 && j < COLUNAS) {

        cicla_estado_celula(&tabuleiro[i][j], botao == 1);

        coord[0] = i;
        coord[1] = j;

        if(strcmp(gamemode, "classico")==0)
          venceu_classico(t, tabuleiro, solucao, ptr);
        else{
          venceu_normal(t, tabuleiro, solucao, v, ptr);
        }
      }
    }
    t->_botao = false;
  }
}

void cicla_estado_celula(Celula *c, bool botao_primario) {
  switch(c->estado) {
        case VAZIO: c->estado = botao_primario ? COLORIDO : ALERTA;
          break;
        case COLORIDO: c->estado = VAZIO;
          break;
        case ALERTA: c->estado = VAZIO;
          break;
  }
}
