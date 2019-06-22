#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "param.h"
#include "GA.h"
#include "Othello.h"

using namespace std;

/* main関数 */
/* GAの実行 */
int main(){
	Othello *othello;
	othello = new Othello ;
	othello->playHuman2();
	delete othello;
  return 0;
}
