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

void getEnableCells(bool, Cell[HIGHT][WIDTH], bool[HIGHT][WIDTH]);
bool existEnableCells(bool[WIDTH][HIGHT]);
bool canPutLine(bool, int[2], int, Cell[HIGHT][WIDTH]);
void add(int[2], int[2]);
bool canPutLineIter(bool, int[2], int, Cell[HIGHT][WIDTH], bool);
bool fillBoard(Cell[HIGHT][WIDTH]);
void finishGame(Cell[HIGHT][WIDTH]);
bool isOneColor(Cell[HIGHT][WIDTH]);
void inputCell(int[2]);
bool canPut(int[2], bool[WIDTH][HIGHT]);
void reverse(bool, int[2], Cell[HIGHT][WIDTH]);
void reverseIter(bool, int[2], int, Cell[WIDTH][HIGHT]);
void displayBoard(Cell[HIGHT][WIDTH]);
Cell getStoneColor(bool);

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
  printf("isFirst: %d\n",isFirst);
  printf("isAI: %d\n",isAI);

  while(true) {
    bool enableCells[WIDTH][HIGHT];
    // enableCellsの初期化
    for (int x=0; x<WIDTH; x++) {
      for (int y=0; y<HIGHT; y++) {
        enableCells[x][y] = false; 
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
      //sucanPutLineIteriAlcanPutLineIterrithm();
    } else {
      printf("human phase\n");
      inputCell(selectedCell);
      //printf("select: %d, %d\n", selectedCell[0], selectedCell[1]);
      if (!canPut(selectedCell, enableCells)) { // 置けない場所を選択していたらisAIフラグを変えずにもう一回ループしてもらう
        isAI = !isAI;
        isFirst = !isFirst;
      } else {
        printf("そこは置けません\n");
        selectedCell[0] = -1;
        selectedCell[1] = -1;
      }
    }

    reverse(isFirst, selectedCell, board);
    isAI = !isAI;
    isFirst = !isFirst;
    displayBoard(board);
  }

  return 0;
}

void getEnableCells(bool isFirst, Cell board[HIGHT][WIDTH], bool enableCells[WIDTH][HIGHT]) {
  // 置くことのできるcellのindexを返す
  for (int x=0; x<WIDTH; x++) {
    for (int y=0; y<HIGHT; y++) {
      int selectedCell[2] = {x, y};
      for(int i=0; i<8; i++) {
        if (canPutLine(isFirst, selectedCell, i, board)) {
          printf("(%d, %c) ", y+1, (x - 1) + 'a');
          enableCells[x][y] = true;
        }
      } 
    }
  }
}

bool canPutLine(bool isFirst, int selectedCell[2], int directionIndex, Cell board[HIGHT][WIDTH]) {
  return canPutLineIter(isFirst, selectedCell, directionIndex, board, false);
}

/*
 *
 * @param (flag) 名前が思いつかなかった. 隣の石が異なる色のときtrueにして呼び出す
 */
bool canPutLineIter(bool isFirst, int selectedCell[2], int directionIndex, Cell board[HIGHT][WIDTH],
    bool flag) {
  int directions[8][2] = {
    {-1,-1}, {0,-1}, {1,-1},
    {-1, 0}, {1, 0},
    {-1,1}, {0,1}, {1,1}
  };
  
  int tmp[2] = {selectedCell[0], selectedCell[1]};
  add(tmp, directions[directionIndex]);
  
  if (tmp[0] <= -1 || tmp[1] <= -1 || tmp[0] > WIDTH || tmp[1] > HIGHT) {
    // 端まで行った 
    return false;
  } else if (board[tmp[0]][tmp[1]] == Blank) {
    // 横がBlank
    return false;
  }
  // printf("%d, %d\n", tmp[0], tmp[1]);

  if (board[tmp[0]][tmp[1]] != isFirst && !flag) {
    // 置けているとき
    return canPutLineIter(isFirst, tmp, directionIndex, board, true);
  } else if (board[tmp[0]][tmp[1]] == isFirst && !flag) {
    // 横が同じ色だったとき 
    return false;
  }  else if (board[tmp[0]][tmp[1]] == isFirst && flag) {
    // 挟んだとき
    return true;
  }
  return canPutLineIter(isFirst, tmp, directionIndex, board, false);
}

void add(int target[2], int a[2]) {
  // printf("add x: %d + %d\n", target[0], a[0]);
  // printf("add y: %d + %d\n", target[1], a[1]);
  target[0] = target[0] + a[0];
  target[1] = target[1] + a[1];
}

bool existEnableCells(bool enableCells[WIDTH][HIGHT]) {
  // 置ける場所が存在したらtrue
  for (int x=0; x<WIDTH; x++) {
    for (int y=0; y<HIGHT; y++) {
      if (enableCells[x][y]) {
        return true;
      }
    }
  }
  return false;
}

bool fillBoard(Cell board[HIGHT][WIDTH]) {
  // ボードが埋まっていたらtrue
  for (int y=0; y < HIGHT; y++) {
    for(int x=0; x < WIDTH ; x++) {
      if (board[x][y] != Blank)
        return false;
    }
  }
  return true;
}

void finishGame(Cell board[HIGHT][WIDTH]) {
  // ゲームの勝敗判定を行う
  int countblack = 0, countwhite = 0;
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
  }
}

bool isOneColor(Cell board[HIGHT][WIDTH]) {
  // cellが一色だけのときtrue
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

// kitagawa
void inputCell(int selectedCell[2]){
  // ユーザから標準入力でcellのindexを受け取る
  // e.g. (3, b)を選択したなら[4, 3]
  
  // ,つなぎで2つの数値を入力
  // 入力後分割とキャスト
  
  char input[20];
  printf(" 置く場所を入力\n input:");
  fgets(input,20,stdin);
  
  char *in;
  
  in = strtok(input, ",");
  selectedCell[1] = atoi(in) - 1;
  
  in = strtok(NULL, ",");
  selectedCell[0] = *in - 'a';

  //printf("intput %d, %d\n", selectedCell[0], selectedCell[1]);
}

bool canPut(int selectedCell[2], bool enableCells[WIDTH][HIGHT]) {
  // enableCellsにselectedCellが含まれていたらtrue
  return enableCells[selectedCell[0]][selectedCell[1]];
}

void reverse(bool isFirst, int selectedCell[2], Cell board[HIGHT][WIDTH]) {
  // selectedCellの場所に置き、boardを更新する
  int x = selectedCell[0], y = selectedCell[1];
  Cell myColor = getStoneColor(isFirst);

  board[x][y] = myColor;
  for (int i=0; i < 8; i++) {
    if (canPutLine(isFirst, selectedCell, i, board)) {
      reverseIter(isFirst, selectedCell, i, board);
    }
  }
}

void reverseIter(bool isFirst, int scanningCell[2], int directionIndex, Cell board[WIDTH][HIGHT]) {
  int directions[8][2] = {
    {-1,-1}, {0,-1}, {1,-1},
    {-1, 0}, {1, 0},
    {-1,1}, {0,1}, {1,1}
  };
  
  int tmp[2] = {scanningCell[0], scanningCell[1]};
  add(tmp, directions[directionIndex]);
  
  int x = tmp[0];
  int y = tmp[1];

  Cell myColor = getStoneColor(isFirst);

  if (board[x][y] != myColor) {
    return;
  }

  board[x][y] = myColor; 
  reverseIter(isFirst, tmp, directionIndex, board);
}

void displayBoard(Cell board[HIGHT][WIDTH]) {
  printf("--------------------------\n");
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
          printf("● ");
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
}

Cell getStoneColor(bool isFirst) {
  if(isFirst) {
    return Black;
  }
  return White;
}
