#ifndef _OTHELLO_H_
#define _OTHELLO_H_

/* Othello.h */

#include <string>
#include "GA.h"
#include "param.h"

#define	nCell	8
#define VF if(virtualFlag == false)
using namespace std;

typedef int* gtype_t;

class Othello {
	
  public:
	
	Othello();
	Othello(bool _virtualFlag);

	~Othello();
	
	////variable////
	int	v, h;
	int nTern;
	int gnum1, gnum2;
			
	bool	virtualFlag; 
	bool	simuFlag;
	enum	State	{Non, Black, White};
	State	winFlag; 
	State	board[nCell][nCell];	
	State	tc;
	
	vector<int> gene;

	int	nb, nw;
	int fail_counter;
	gtype_t com1[LENGTH];
	gtype_t com2[LENGTH];
	////function////
	void	play();
	void	playCom(gtype_t com1, gtype_t com2);
	void	playHuman(gtype_t _com1); 
	void	playHuman2(void);	
	int	c2i(char c);
	
	void	show();
	
	void	ternChange();
	bool	putCheck(int v, int h, State sc);
	void	put(int v, int h, State sc);
	bool	reverseCheck(int v, int h, State sc);
	void	reverse(int v, int h, State sc);

	void com_routine(State _tc);
	void init_pos_list(void);
	void search_pos(int _ptype, State _tc);
	void search_pos(State _tc);
	void search_pos2(State _tc);
	void simulation(void);
	void determine_pos( State _tc, bool human);
	void save_board(void);
	void struct_virtual(void);
	void init_Okeru(void);

	
  private:
	////variable////
	bool Okeru,Okeru1,Okeru2,Okeru3,Okeru4;
	vector<vector<int> > pos_list;
	////function////			
	int	reverseBase(int v, int h, State sc, bool rFlag);
};


#endif