#include<stdio.h>
#include<string.h>
#include <stdbool.h>
#define HIGHT 8
#define WIDTH 8

// ボードの状態を表すenum
typedef enum Cell {
  Blank,
  Black,
  White,
} Cell;

void getEnableCells(bool, Cell[HIGHT][WIDTH], int[64][2]);
bool existEnableCells(int[64][2]);
bool fillBoard(Cell[HIGHT][WIDTH]);
void finishGame(Cell[HIGHT][WIDTH]);
bool isOneColor(Cell[HIGHT][WIDTH]);
void inputCell(int[2]);
bool canPut(int[2], int[64][2]);
void reverse(bool, int[2], Cell[HIGHT][WIDTH]);
void displayBoard(Cell[HIGHT][WIDTH]);

int main() {
  // ボードの初期状態
  Cell board[HIGHT][WIDTH];
  for (int y=0; y < HIGHT; y++) {
    for(int x=0; x < WIDTH ; x++) {
      board[x][y] = Blank;
    }
  }
  board[3][3] = White;
  board[4][4] = White;
  board[3][4] = Black;
  board[4][3] = Black;

  // 初期状態の表示
  displayBoard(board);

  // TODO: 先行後攻の選択の処理
  bool isFirst = true;
  bool isAI = true;
  if(isFirst) {
    isAI = false;
  }

  while(true) {
    int enableCells[64][2];
    // enableCellsの初期化
    for (int i = 0; i < 64; i++) {
      for(int j = 0; j < 2; j++) {
        enableCells[i][j] = -1; 
      }
    }
    getEnableCells(isFirst, board, enableCells);

    if(!existEnableCells(enableCells)) {
      if(fillBoard(board)){
        finishGame(board);
        return 0;
      }
      if (isOneColor(board)) {
        finishGame(board);
        return 0;
      }
      // passの処理
      isAI = !isAI;
      isFirst = !isFirst;
    }

    int selectedCell[2] = {-1, -1};
    if(isAI){
      printf("AI phase\n");
      //sugoiAlgorithm();
      isAI = !isAI;
      isFirst = !isFirst;
    } else {
      inputCell(selectedCell);
      printf("select: %d, %d\n", selectedCell[0], selectedCell[1]);
      if (canPut(selectedCell, enableCells)) { // 置けない場所を選択していたらisAIフラグを変えずにもう一回ループしてもらう
        isAI = !isAI;
        isFirst = !isFirst;
      } 
    }

    reverse(isFirst, selectedCell, board);
    displayBoard(board);
  }

  return 0;
}

void getEnableCells(bool isFirst, Cell board[HIGHT][WIDTH], int enableCells[64][2]) {
  // TODO: 置くことのできるcellのindexを返す
  // e.g. (1, a), (2, d) のマスが置けるなら[[2, 0], [3, 3]]
}

bool existEnableCells(int enableCells[64][2]) {
  // TODO: 置ける場所が存在したらtrue
  return true;
}

bool fillBoard(Cell board[HIGHT][WIDTH]) {
  // TODO: ボードが埋まっていたらtrue
  for (int y=0; y < HIGHT; y++) {
    for(int x=0; x < WIDTH ; x++) {
      if (board[x][y] != Blank)
        return true;
    }
  }
  return false;
}

void finishGame(Cell board[HIGHT][WIDTH]) {
  // TODO: ゲームの勝敗判定を行う
/*  int countblack = 0, countwhite = 0;
  for (int y=0; y < HIGHT; y++) {
    for(int x=0; x < WIDTH ; x++) {
      switch (board[x][y]) {
	case Black:
	  countblack++;
	  break;
	case White:
	  countwhite++;
	  break;
	default:
	  break;
      }
    }
  }
	
  printf("o:%d\n",countwhite);
  printf("x:%d\n",countblack);
  if(countwhite > countblack) {
    printf("winner:o");
  } else if (countwhite < countblack) {
    printf("winner:x");
  } else {
    printf("draw");
  }*/
}

bool isOneColor(Cell board[HIGHT][WIDTH]) {
  // TODO: cellが一色だけのときtrue
  enum Cell color = Blank;
  for (int y=0; y < HIGHT; y++) {
    for(int x=0; x < WIDTH ; x++) {
      if(board[x][y]!=Blank) {
        if(color == Blank) {
          color = board[x][y];
    	} else if(board[x][y]!=color) {
          return false;
        }
      }
    if(y == HIGHT-1 && x == WIDTH-1)
      return true;
    }
  }
}

void inputCell(int selectedCell[2]){
  // TODO: ユーザから標準入力でcellのindexを受け取る
  // e.g. (3, b)を選択したなら[4, 3]
  
  // ,つなぎで2つの数値を入力
  // 入力後分割とキャスト
  
  char input[20];
  printf("置く場所を入力：");
  fgets(input,20,stdin);
  
  char *in;
  
  in = strtok(input, ",");
  selectedCell[0] = atoi(in) - 1;
  
  in = strtok(NULL, ",");
  selectedCell[1] = atoi(in);
}

bool canPut(int selectedCell[2], int enableCells[64][2]) {
  // TODO: enableCellsにselectedCellが含まれていたらtrue
  for(int i=0;i<64;i++) {
    if(selectedCell[0]==enableCells[i][0] && selectedCell[1]==enableCells[i][1]) {
      return true;
      break;
    } else if(i == 64-1) {
      return false;
    }
  }
}

void reverse(bool isFirst, int selectedCell[2], Cell board[HIGHT][WIDTH]) {
  // TODO: selectedCellの場所に置き、boardを更新する
  //未完成
  int x = selectedCell[0], y = selectedCell[1];
  //board[x][y] = ;
  enum Cell cellcolor = board[x][y];
  int nextcell[8][2] = { {-1,-1},{-1,0},{-1,1},
			 {0,-1},        {0,1},
			 {1,-1},{1,0},{1,1}};
  for(int a = 0; a < 8; a++) {
    int x2 = x+nextcell[a][0] , y2 = y+nextcell[a][1];
    //if(座標[x2][y2]が盤面の中かの判定){
      if(board[x2][y2] != Blank && board[x2][y2] != cellcolor) {
        for(int i = 0; i < 6; i++) {
          x2 += nextcell[a][0];
  	  y2 += nextcell[a][1];
	  //if(座標[x2][y2]が盤面の中かの判定){
  	  if(board[x2][y2] == cellcolor) {
  	    for(int j = 0; j < i+1 ; j++) {
              x2 -= nextcell[a][0];
              y2 -= nextcell[a][0];
              board[x2][y2] = cellcolor;
  	    }
  	  }
       }
    }
  }
}

void displayBoard(Cell board[HIGHT][WIDTH]) {
  printf("   ");
  for(int i = 0; i < WIDTH; i++){
    printf("%c ", 'a' + i);
  }
  printf("\n");


  for (int y=0; y < HIGHT; y++) {
    printf(" %d ", y+1);
    for(int x=0; x < WIDTH ; x++) {
      switch (board[x][y]) {
        case Black:
          printf("× ");
          break;
        case White:
          printf("◯ ");
          break;
        default:
          printf("- ");
          break;
      }
    }
    printf("\n");
  }
  printf("--------------------------\n");
}
