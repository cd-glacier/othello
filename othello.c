#include<stdio.h>
#include<string.h>
#include <stdbool.h>
#define HIGHT 8
#define WIDTH 8

// ボードの状態を表すenum
typedef enum Cell {
  Blank = 0,
  Black = 1,
  White =-1,
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
void selectByAI(int[2], bool[WIDTH][HIGHT], Cell[HIGHT][WIDTH]);
int eval(Cell[HIGHT][WIDTH]);
int maxlevel(int, Cell[HIGHT][WIDTH], bool[HIGHT][WIDTH]);

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
    bool enableCells[WIDTH][HIGHT];
    // enableCellsの初期化
    for (int x=0; x<WIDTH; x++) {
      for (int y=0; y<HIGHT; y++) {
        enableCells[x][y] = false; 
      }
    }
    getEnableCells(isFirst, board, enableCells);

    int selectedCell[2] = {-1, -1};
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
      printf("passed \n");
    } else {
      if(isAI){
        printf("AI phase\n");
        selectByAI(selectedCell, enableCells, board); 
      } else {
        printf("human phase\n");
        inputCell(selectedCell);
        //printf("select: %d, %d\n", selectedCell[0], selectedCell[1]);
        if (!canPut(selectedCell, enableCells)) { 
          // 置けない場所を選択していたらisAIフラグを変えずにもう一回ループしてもらう
          printf("そこは置けません\n");
          selectedCell[0] = -1;
          selectedCell[1] = -1;
          // 二回flagを変えて元のflagに戻している
          isAI = !isAI;
          isFirst = !isFirst;
        } 
      }
    }

    reverse(isFirst, selectedCell, board);
    displayBoard(board);
    isAI = !isAI;
    isFirst = !isFirst;
  }

  return 0;
}

void getEnableCells(bool isFirst, Cell board[HIGHT][WIDTH], bool enableCells[WIDTH][HIGHT]) {
  // 置くことのできるcellのindexを返す
  for (int x=0; x<WIDTH; x++) {
    for (int y=0; y<HIGHT; y++) {
      int selectedCell[2] = {x, y};
      for(int i=0; i<8; i++) {
        if (board[x][y] == Blank 
            && canPutLine(isFirst, selectedCell, i, board)) {
          printf("(%d, %c) ", y+1, x + 'a');
          enableCells[x][y] = true;
        }
      } 
    }
  }
}

bool canPutLine(bool isFirst, int selectedCell[2], int directionIndex, Cell board[HIGHT][WIDTH]) {
  int x = selectedCell[0];
  int y = selectedCell[1];
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
  
  Cell myColor = getStoneColor(isFirst);
  if (tmp[0] <= -1 || tmp[1] <= -1 || tmp[0] > WIDTH || tmp[1] > HIGHT) {
    // 端まで行った 
    return false;
  } else if (board[tmp[0]][tmp[1]] == Blank) {
    // 横がBlank
    return false;
  }  
  // printf("%d, %d\n", tmp[0], tmp[1]);

  if (board[tmp[0]][tmp[1]] != myColor && !flag) {
    // 置けているとき
    return canPutLineIter(isFirst, tmp, directionIndex, board, true);
  } else if (board[tmp[0]][tmp[1]] == myColor && !flag) {
    // 横が同じ色だったとき 
    return false;
  }  else if (board[tmp[0]][tmp[1]] == myColor && flag) {
    // 挟んだとき
    return true;
  }
  return canPutLineIter(isFirst, tmp, directionIndex, board, true);
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
      if (board[x][y] == Blank)
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

  if (board[x][y] == myColor || board[x][y] == Blank) {
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
}

Cell getStoneColor(bool isFirst) {
  if(isFirst) {
    return Black;
  }
  return White;
}

//AI by Ohata
void selectByAI(int selectedCell[2], bool enableCells[WIDTH][HIGHT], Cell board[HIGHT][WIDTH]) {
	int selectedNum = maxlevel(1,board,enableCells);
	selectedCell[0] = selectedNum/10;
	selectedCell[1] = selectedNum%10;
	printf("評価値：%d\n", eval(board));
}

//minimax法 by Ohata
int maxlevel(int limit, Cell board[HIGHT][WIDTH], bool enableCells[HIGHT][WIDTH]){
	int max_eval;
	int selected[2] = {2, 4};
	bool isFirst;
	if(limit == 1){ //自分の手を考える
		max_eval = 1000;
		for(int y=0;y<8;y++){
			for(int x=0;x<8;x++){
				if(enableCells[x][y]){
					Cell copyBoard[HIGHT][WIDTH];
					int thinkSelectedCell[2] = {x,y};
					for(int a=0;a<8;a++){
						for(int b=0;b<8;b++){
							copyBoard[a][b] = board[a][b];
						}
					}
					isFirst = false;
					reverse(isFirst,thinkSelectedCell, copyBoard); //次の手を打つ
					bool copyEnableCells[8][8];
					for(int x=0;x<8;x++){
						for(int y=0;y<8;y++){
							copyEnableCells[x][y] = false;
						}
					}
					printf("\n%d,%d",x,y);
					getEnableCells(!isFirst, copyBoard, copyEnableCells);
					if(maxlevel(limit-1, copyBoard, copyEnableCells)<max_eval){
						max_eval = eval(copyBoard);
						selected[0] = x;
						selected[1] = y;
					}
				}
			}
		}
		return selected[0]*10+selected[1];
	} else if(limit = 0){ //相手の手を考える
		max_eval = -1000;
		for(int y=0;y<8;y++){
			for(int x=0;x<8;x++){
				if(enableCells[x][y]){
					Cell copyBoard[HIGHT][WIDTH];
					int thinkSelectedCell[2] = {x,y};
					for(int a=0;a<8;a++){
						for(int b=0;b<8;b++){
							copyBoard[a][b] = board[a][b];
						}
					}
					isFirst = true;
					reverse(isFirst,thinkSelectedCell, copyBoard); //次の手を打つ
					if(eval(copyBoard)>max_eval){
						max_eval = eval(copyBoard);
						selected[0] = x;
						selected[1] = y;
					}
				}
			}
		}
		return max_eval;
	}
}
/*	
	int thinkSelectedCell[] = new int[2]; //可能な手を一時保存する配列
	//可能な手を全て生成
	for(int x=0; x<8; x++){
		for(int y=0; y<8;y++){
			if(enableCell[x][y]){
				int score, score_max;
				//手を打つ
				thinkSelectedCell[0]=x;
				thinkSelectedCell[1]=y;
				thinkReverse();
				score = minlevel(limit-1, alpha, beta, board, enableCells); //次の相手の手
				//手を戻す
				if(score => beta){
				//beta値を上回ったら探索終了
					return score;
				}
				if(score > score_max){
				//良い手が見つかったら
					score_max = score;
					alpha = max(alpha, score_max); //α値を更新
				}
			}
		}
	}
	return score_max;
*/


/*
//相手の手を調べる
int minlevel(int limit, int alpha, int beta, Cell board[HIGHT][WIDTH], boolean enableCells[HIGHT][WIDTH]){
	if(limit == 0){
		return eval(board);
	}
	//可能な手を全て生成
	for(int x=0; x<8; x++){
		for(int y=0; y<8; y++){
			if(enableCells[x][y]){
				int score, score_min;
				//手を打つ
				score = maxlevel(limit-1, alpha, beta, board, enavleCells); //次の自分の手
				//手を戻す
				if(score <= alpha){
					alpha値を上回ったら探索中止
					return score;
				}
				if(score < score_min){
				//より悪い手(相手にとって良い手)が見つかった
					score_min = score;
					beta = min(beta, score_min); //β値を更新 
				}
			}
		}
	}
	return score_min;
}
*/

//石の位置による評価値 by Ohata
int val_table[8][8] = {
	{120, -20,  20,   5,   5,  20,  -20, 120},
	{-20, -40,  -5,  -5,  -5,  -5,  -40, -20},
	{ 20,  -5,  15,   3,   3,  15,   -5,  20},
	{  5,  -5,   3,   3,   3,   3,   -5,   5},
	{  5,  -5,   3,   3,   3,   3,   -5,   5},
	{ 20,  -5,  15,   3,   3,  15,   -5,  20},
	{-20, -40,  -5,  -5,  -5,  -5,  -40, -20},
	{120, -20,  20,   5,   5,  20,  -20, 120}
};

//評価値を返す関数 by Ohata
int eval(Cell board[HIGHT][WIDTH]){
	int i,j,val=0;
	for(i=0; i<8; i++){
		for(j=0; j<8; j++){
			val += val_table[i][j] * board[i][j]; 
		}
	}
	return val;
}


