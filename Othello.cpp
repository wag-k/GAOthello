/* Othello.cpp */

#include "Othello.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include "GA.h"
#include "param.h"

using namespace std;

Othello::Othello() {
	simuFlag=false;
	virtualFlag = false; 
	for(int i=0; i<nCell; i++) {
		for(int j=0; j<nCell; j++) {
			board[i][j] = Non;
		}
	}
	board[(nCell/2)-1][(nCell/2)-1] = White;
	board[(nCell/2)][(nCell/2)] = White;
	board[(nCell/2)-1][(nCell/2)] = Black;
	board[(nCell/2)][(nCell/2)-1] = Black;
	
	pos_list.resize(nCell*3,vector<int>(25,0));
	v=0;
	h=0;
	nTern =0;
	gene.resize(LENGTH);
	tc = Black;
	gnum1 =0;
	gnum2 =0;
}
Othello::Othello(bool _virtualFlag) {
	virtualFlag = _virtualFlag; 
	if (virtualFlag == true) {
		//cout << "Simulation Mode" <<endl;
		struct_virtual();
		tc = White;
	}
	else{
		for(int i=0; i<nCell; i++) {
			for(int j=0; j<nCell; j++) {
				board[i][j] = Non;
			}
		}
		board[(nCell/2)-1][(nCell/2)-1] = White;
		board[(nCell/2)][(nCell/2)] = White;
		board[(nCell/2)-1][(nCell/2)] = Black;
		board[(nCell/2)][(nCell/2)-1] = Black;
		tc = Black;
		nTern=0;
	}
	pos_list.resize(nCell*3,vector<int>(25,0));
	v=0;
	h=0;
	gnum1 =0;
	gnum2 =0;
	gene.resize(LENGTH);
}

Othello::~Othello() {
	
	if (virtualFlag == false)cout << "quit" << endl;
	pos_list.clear();
}

void	Othello::play() {
	
	string	str;
	int	maxTern = nCell*nCell - 4;
	nTern = 0;
	
	while(1) {
		
		show();
		
		if(nTern>=maxTern)	break;
		
		if(tc==Black)	{cout << "Black tern : " ;}
		else if(tc==White) {cout << "White tern : " ;}
		cout << "input point\n>> " ;
		cin >> str;
		
		if((str.at(0)=='q')&&(str.length()==1))	return;
		
		if((str.at(0)=='p')&&(str.length()==1)) {
			ternChange();
			continue;
		}
		
		if(str.length()!=2) {
			cout << "please try again" << endl;
			continue;
		}
		int	v, h;
		char	c;
		if(str.at(0)<str.at(1)) {
			v = c2i(str.at(0));
			h = c2i(str.at(1));
		}
		else {
			v = c2i(str.at(1));
			h = c2i(str.at(0));
		}
		if(!(v>=0 && v<nCell)) {
			cout << "out of board" << endl;
			continue;
		}
		if(!(h>=0 && h<nCell)) {
			cout << "out of board" << endl;
			continue;
		}
		
		if((!putCheck(v, h, tc)) || (!reverseCheck(v, h, tc))) {
			cout << "you can't put this point" << endl;
			continue;
		}
		put(v, h, tc);
		reverse(v, h, tc);
		
		ternChange();
		nTern++;
	}
	
	int	nb=0, nw=0;
	for(int i=0; i<nCell; i++) {
		for(int j=0; j<nCell; j++) {
			if(board[i][j]==Black)	nb++;
			else if(board[i][j]==White) nw++;
		}
	}
	cout << "Black : " << nb << endl;
	cout << "White : " << nw << endl;
	if(nb>nw) {
		cout << "winner : Black" << endl;
	} else if(nb<nw) {
		cout << "winner : White" << endl;
	} else if(nb==nw) {
		cout << "draw" << endl;
	}
	
}

void Othello::playCom(gtype_t _com1, gtype_t _com2) {
	string	str;
	int	maxTern = nCell*nCell - 4;
	int fail_counter=0;
	
	
	for(int i=0; i<LENGTH;i++){
		com1[i]=&_com1[i];
		//cout << *com1[i] ;
	}
	//cout << endl;
	for(int i=0; i<LENGTH;i++){
		com2[i]=&_com2[i];
		//cout << *com2[i] ;
	}
	//cout << endl;
	while(1) {
		
		if (virtualFlag == false)show();

		
		if(nTern>=maxTern)	break;
		
		init_pos_list();
		Okeru = false;
		search_pos2(tc);
		if(Okeru==false){
			ternChange();
			continue;
		}
		determine_pos( tc, false);

		char	c;
		if(!(v>=0 && v<nCell)) {
			if (virtualFlag == false)cout << "out of board" << endl;
			continue;
		}
		if(!(h>=0 && h<nCell)) {
			if (virtualFlag == false)cout << "out of board" << endl;
			continue;
		}
		
		if((!putCheck(v, h, tc)) || (!reverseCheck(v, h, tc))) {
			if (virtualFlag == false)cout << "you can't put this point" << endl;
			fail_counter +=1;
			if(fail_counter==2) break;
			ternChange();
			continue;
		}
		put(v, h, tc);
		reverse(v, h, tc);
		ternChange();

		fail_counter=0;
		nTern++;
	}
	
	nb=0, nw=0;
	for(int i=0; i<nCell; i++) {
		for(int j=0; j<nCell; j++) {
			if(board[i][j]==Black)	nb++;
			else if(board[i][j]==White) nw++;
		}
	}
	if (virtualFlag == false)cout << "Black : " << nb << endl;
	if (virtualFlag == false)cout << "White : " << nw << endl;
	if(nb>nw) {
		winFlag=Black;
	} else if(nb<nw) {
		winFlag=White;
	} else if(nb==nw) {
		winFlag=Non;
	}
	
}

void	Othello::playHuman(gtype_t _com1) {
	
	string	str;
	int	maxTern = nCell*nCell - 4;
	nTern = 0;
	int fail_counter=0;
	for(int i=0; i<LENGTH;i++){
		com1[i]=0;
		com2[i]=&_com1[i];
		cout << *com2[i] ;
	}
	cout << endl;
	
	while(1) {
		
		show();
		
		if(nTern>=maxTern)	break;
		char	c;
		v=0;h=0;
		
		//HUMAN
		if(tc==Black)	{
			cout << "Black tern : " ;
			cout << "input point\n>> " ;
			cin >> str;
			
			if((str.at(0)=='q')&&(str.length()==1))	return;
			
			if((str.at(0)=='p')&&(str.length()==1)) {
				ternChange();
				continue;	
			}
			if(str.length()!=2) {
				cout << "please try again" << endl;
				continue;
			}
			

			if(str.at(0)<str.at(1)) {
				v = c2i(str.at(0));
				h = c2i(str.at(1));
			}
			else {
				v = c2i(str.at(1));
				h = c2i(str.at(0));
			}
		}
		//COM
		init_pos_list();
		Okeru = false;
		search_pos(tc);
		if(Okeru==false){
			ternChange();
			continue;
		}
		determine_pos(tc, true);		
		
				
		if(!(v>=0 && v<nCell)) {
			cout << "out of board" << endl;
			fail_counter +=1;
			if(fail_counter==2) break;
			
			
			continue;
		}
		if(!(h>=0 && h<nCell)) {
			cout << "out of board" << endl;
			fail_counter +=1;
			if(fail_counter==2) break;
			
			continue;
		}
	
		if((!putCheck(v, h, tc)) || (!reverseCheck(v, h, tc))) {
			cout << "you can't put this point" << endl;
			if(fail_counter==2) break;
			if(tc==White){
				ternChange();
				fail_counter +=1;
			}
			continue;
		}
		put(v, h, tc);
		reverse(v, h, tc);
		
		ternChange();
		fail_counter=0;
		nTern++;
	}
	
	int	nb=0, nw=0;
	for(int i=0; i<nCell; i++) {
		for(int j=0; j<nCell; j++) {
			if(board[i][j]==Black)	nb++;
			else if(board[i][j]==White) nw++;
		}
	}
	cout << "Black : " << nb << endl;
	cout << "White : " << nw << endl;
	if(nb>nw) {
		cout << "winner : Black" << endl;
	} else if(nb<nw) {
		cout << "winner : White" << endl;
	} else if(nb==nw) {
		cout << "draw" << endl;
	}
	
}

void	Othello::playHuman2() {
	
	string	str;
	int	maxTern = nCell*nCell - 4;
	nTern = 0;
	int fail_counter=0;
	
	//if(simuFlag==false)simulation();	
	simuFlag=true;
	while(1) {
		
		show();
		
		if(nTern>=maxTern)	break;
		char	c;
		
		//HUMAN
		if(tc==Black)	{
			cout << "Black tern : " ;
			cout << "input point\n>> " ;
			cin >> str;
			
			if((str.at(0)=='q')&&(str.length()==1))	return;
			
			if((str.at(0)=='p')&&(str.length()==1)) {
				ternChange();
				continue;	
			}
			if(str.length()!=2) {
				cout << "please try again" << endl;
				continue;
			}
			

			if(str.at(0)<str.at(1)) {
				v = c2i(str.at(0));
				h = c2i(str.at(1));
			}
			else {
				v = c2i(str.at(1));
				h = c2i(str.at(0));
			}
		}
		//COM
		if(tc==White) {
			cout << "White tern : " <<endl;
			com_routine(tc);
			//init_pos_list();
			//init_Okeru();	
			//search_pos2(tc);
			if(Okeru==false){
				ternChange();
				continue;
			}
			determine_pos(tc, true);	
		}
		
		
				
		if(!(v>=0 && v<nCell)) {
			cout << "out of board" << endl;
			fail_counter +=1;
			if(fail_counter==2) break;
			
			
			continue;
		}
		if(!(h>=0 && h<nCell)) {
			cout << "out of board" << endl;
			fail_counter +=1;
			if(fail_counter==2) break;
			
			continue;
		}
	
		if((!putCheck(v, h, tc)) || (!reverseCheck(v, h, tc))) {
			cout << "you can't put this point" << endl;
			if(fail_counter==2) break;
			if(tc==White){
				ternChange();
				fail_counter +=1;
			}
			continue;
		}
		put(v, h, tc);
		reverse(v, h, tc);
		
		ternChange();
		fail_counter=0;
		nTern++;
	}
	
	int	nb=0, nw=0;
	for(int i=0; i<nCell; i++) {
		for(int j=0; j<nCell; j++) {
			if(board[i][j]==Black)	nb++;
			else if(board[i][j]==White) nw++;
		}
	}
	cout << "Black : " << nb << endl;
	cout << "White : " << nw << endl;
	if(nb>nw) {
		cout << "winner : Black" << endl;
	} else if(nb<nw) {
		cout << "winner : White" << endl;
	} else if(nb==nw) {
		cout << "draw" << endl;
	}
	
}


int	Othello::c2i(char c) {
	
	if(c>='a' && c<='z') {
		return int(c-'a');
	}
	else if(c>='A' && c<='Z') {
		return int(c-'A');
	}
	else if(c>='1' && c<='9') {
		return int(c-'1');
	}
	else {
		return -1;
	}
	
}

void	Othello::show() {
	
	cout << "  A B C D E F G H" << endl;
	for(int v=0; v<nCell; v++) {
		cout << v+1 << " " ;
		for(int h=0; h<nCell; h++) {
			switch(board[v][h]) {
			  case Non:
				cout << "--" ;
				break;
			  case Black:
				cout << "B " ;
				break;
			  case White:
				cout << "W " ;
				break;
			}
		}
		cout << endl;
	}
	
}

void	Othello::ternChange() {
	
	if(tc==Black) {
		tc = White;
	}
	else if(tc==White) {
		tc = Black;
	}
	
	return;
}

bool	Othello::putCheck(int v, int h, State sc) {
	
	if(board[v][h]==Non) {
		return true;
	}
	else {
		return false;
	}
	
}

void	Othello::put(int v, int h, State sc) {
	
	board[v][h] = sc;
	
}

bool	Othello::reverseCheck(int v, int h, State sc) {
	
	int num;
	
	num = reverseBase(v, h, sc, false);
	
	if(num) {
		return true;
	} else {
		return false;
	}
}

void	Othello::reverse(int v, int h, State sc) {
	
	reverseBase(v, h, sc, true);
	
}


void Othello::com_routine(State _tc){
	init_pos_list();
	init_Okeru();
	if (nTern == 1) simulation();		
	if (nTern > 5) simulation();		
	search_pos2(_tc);
}


int	Othello::reverseBase(int v, int h, State sc, bool rFlag) {
	
	State oc;
	if(sc==Black)	oc=White;
	if(sc==White)	oc=Black;
	int	cnt=0, tcnt=0;
	int nn, nv, nh;
	
	//over
	if(v>1) {
		cnt=0;
		nn=v-1;
		while(1) {
			if(board[nn][h]==oc) {
				cnt++;
				if(nn==0)	break;
			}
			else if(board[nn][h]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=v-1; i>nn; i--) {
						board[i][h] = sc;
					}
				}
				break;
			}
			else if(board[nn][h]==Non) {
				break;
			}
			nn--;
		}
	}
	
	//under
	if(v<nCell-2) {
		cnt=0;
		nn=v+1;
		while(1) {
			if(board[nn][h]==oc) {
				cnt++;
				if(nn==(nCell-1))	break;
			}
			else if(board[nn][h]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=v+1; i<nn; i++) {
						board[i][h] = sc;
					}
				}
				break;
			}
			else if(board[nn][h]==Non) {
				break;
			}
			nn++;
		}
	}
	
	//left
	if(h>1) {
		cnt=0;
		nn=h-1;
		while(1) {
			if(board[v][nn]==oc) {
				cnt++;
				if(nn==0)	break;
			}
			else if(board[v][nn]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=h-1; i>nn; i--) {
						board[v][i] = sc;
					}
				}
				break;
			}
			else if(board[v][nn]==Non) {
				break;
			}
			nn--;
		}
	}
	
	//right
	if(h<nCell-2) {
		cnt=0;
		nn=h+1;
		while(1) {
			if(board[v][nn]==oc) {
				cnt++;
				if(nn==(nCell-1))	break;
			}
			else if(board[v][nn]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=h+1; i<nn; i++) {
						board[v][i] = sc;
					}
				}
				break;
			}
			else if(board[v][nn]==Non) {
				break;
			}
			nn++;
		}
	}
	
	//over left
	if(v>1 && h>1) {
		cnt = 0;
		nv = v-1;
		nh = h-1;
		
		while(1) {
			if(board[nv][nh]==oc) {
				cnt++;
				if(nv==0 || nh==0)	break;
			}
			else if(board[nv][nh]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=1; i<=cnt; i++) {
						board[v-i][h-i] = sc;
					}
				}
				break;
			}
			else if(board[nv][nh]==Non) {
				break;
			}
			nv--;
			nh--;
		}
	}
	
	//over right
	if(v>1 && h<nCell-2) {
		cnt = 0;
		nv = v-1;
		nh = h+1;
		
		while(1) {
			if(board[nv][nh]==oc) {
				cnt++;
				if(nv==0 || nh==nCell-1)	break;
			}
			else if(board[nv][nh]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=1; i<=cnt; i++) {
						board[v-i][h+i] = sc;
					}
				}
				break;
			}
			else if(board[nv][nh]==Non) {
				break;
			}
			nv--;
			nh++;
		}
	}
	
	//under left
	if(v<nCell-2 && h>1) {
		cnt = 0;
		nv = v+1;
		nh = h-1;
		
		while(1) {
			if(board[nv][nh]==oc) {
				cnt++;
				if(nv==nCell-1 || nh==0)	break;
			}
			else if(board[nv][nh]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=1; i<=cnt; i++) {
						board[v+i][h-i] = sc;
					}
				}
				break;
			}
			else if(board[nv][nh]==Non) {
				break;
			}
			nv++;
			nh--;
		}
	}
	
	//under left
	if(v<nCell-2 && h<nCell-2) {
		cnt = 0;
		nv = v+1;
		nh = h+1;
		
		while(1) {
			if(board[nv][nh]==oc) {
				cnt++;
				if(nv==nCell-1 || nh==nCell-1)	break;
			}
			else if(board[nv][nh]==sc) {
				tcnt += cnt;
				if(rFlag) {
					for(int i=1; i<=cnt; i++) {
						board[v+i][h+i] = sc;
					}
				}
				break;
			}
			else if(board[nv][nh]==Non) {
				break;
			}
			nv++;
			nh++;
		}
	}
	
	
	return tcnt;
	
}



void Othello::init_pos_list(void){
	for(int i=0; i<nCell*3; i++){		
		pos_list[i].resize(25);
		for(int j=0; j<24; j++){
			pos_list[i][j] = 0;
		}
	}
}

void Othello::search_pos(int _ptype,State _tc){
	int tmp=0;
	for(int i=0; i<nCell; i++){
		for(int j=0; j<nCell; j++){
			if(putCheck(i,j,_tc)){
				tmp = reverseBase(i,j,_tc,false);
				pos_list[tmp][0]=tmp;
				pos_list[tmp][1]=i;
				pos_list[tmp][2]=j;
				Okeru = true;
			}	
		}
	}
}


void Othello::search_pos(State _tc){
	VF cout << "Searching"<< endl;
	int tmp=0;
	for(int i=0; i<nCell/2; i++){
		for(int j=0; j<nCell/2; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				pos_list[tmp][0]=tmp;
				pos_list[tmp][1]=i;
				pos_list[tmp][2]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru1 = true;
				}
			}	
		}
		for(int j=nCell/2; j<nCell; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				pos_list[tmp][3]=tmp;
				pos_list[tmp][4]=i;
				pos_list[tmp][5]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru2 = true;
				}
			}
		}
	}
	for(int i=nCell/2; i<nCell; i++){
		for(int j=0; j<nCell/2; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				pos_list[tmp][6]=tmp;
				pos_list[tmp][7]=i;
				pos_list[tmp][8]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru3 = true;
				}	
			}
		}
		for(int j=nCell/2; j<nCell; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				pos_list[tmp][9]=tmp;
				pos_list[tmp][10]=i;
				pos_list[tmp][11]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru4 = true;
				}
				//VF cout <<tmp<<"-"<< i <<"," <<j << endl;
			}
		}
	}
	for(int i =0; i<nCell*3-1; i++){
		for(int j=0; j<12; j+=3){
			VF cout<< pos_list[i][j]<< "-"<< char(pos_list[i][j+2]+'A')<< pos_list[i][j+1]+1<<"|";
		}
		VF cout<<endl;
	}
	
	VF cout << "Complete"<< endl;
}
void Othello::search_pos2(State _tc){
	VF cout << "Searching"<< endl;
	int tmp=0;
	for(int i=0; i<nCell/2; i++){
		for(int j=0; j<nCell/2; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				if(tmp!=pos_list[tmp][3]){
					pos_list[tmp][0]=tmp;
					pos_list[tmp][1]=i;
					pos_list[tmp][2]=j;
				}
				pos_list[tmp][3]=tmp;
				pos_list[tmp][4]=i;
				pos_list[tmp][5]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru1 = true;
				}
			}	
		}
		for(int j=nCell/2; j<nCell; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				if(tmp!=pos_list[tmp][9]){
					pos_list[tmp][6]=tmp;
					pos_list[tmp][7]=i;
					pos_list[tmp][8]=j;
				}
				pos_list[tmp][9]=tmp;
				pos_list[tmp][10]=i;
				pos_list[tmp][11]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru2 = true;
				}
			}
		}
	}
	for(int i=nCell/2; i<nCell; i++){
		for(int j=0; j<nCell/2; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				if(tmp!=pos_list[tmp][15]){
					pos_list[tmp][12]=tmp;
					pos_list[tmp][13]=i;
					pos_list[tmp][14]=j;
				}
				pos_list[tmp][15]=tmp;
				pos_list[tmp][16]=i;
				pos_list[tmp][17]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru3 = true;
				}	
			}
		}
		for(int j=nCell/2; j<nCell; j++){
			if(putCheck(i,j,_tc)){
				tmp=0;
				tmp = reverseBase(i,j,_tc,false);
				if(tmp!=pos_list[tmp][21]){
					pos_list[tmp][18]=tmp;
					pos_list[tmp][19]=i;
					pos_list[tmp][20]=j;
				}
				pos_list[tmp][21]=tmp;
				pos_list[tmp][22]=i;
				pos_list[tmp][23]=j;
				Okeru = true;
				if(tmp> 0){
					Okeru4 = true;
				}
				//VF cout <<tmp<<"-"<< i <<"," <<j << endl;
			}
		}
	}
	for(int i =0; i<nCell*3-1; i++){
		for(int j=0; j<24; j+=3){
			VF cout<< pos_list[i][j]<< "-"<< char(pos_list[i][j+2]+'A')<< pos_list[i][j+1]+1<<"|";
		}
		VF cout<<endl;
	}
	
	VF cout << "Complete"<< endl;
}


void Othello::simulation(void){
	save_board();
	cout << "Simulating" << endl;
	srand(time(NULL));
	
	POPU *parent_group = new POPU(POP, LENGTH, CODE_MAX) ;
	POPU *child_group = new POPU(POP, LENGTH, CODE_MAX) ; 	
	int i ;
	
	
	for(i=0;i < GENERATION;i++){
		
		// 集団の適合度を計算し、線形リストを作る
		parent_group->calc_fitness(MIN,MAX);
		// 最大値・最小値、
		parent_group->normalize_population();
		// 現在の世代の表示
		if(PRINT_GROUP ==1){
			parent_group->print_population();
		}
		if(PRINT_FITNESS == 1){
			if(PRINT_FITNESS==1) printf("#generation,max_fitness, avg_fitness, min_fitness, best_individual_ptype, best_individual_gtype\n");
			printf("%3d, ",i);  
			
			parent_group->print_fitness();  
		}
		
		if(i==GENERATION-1){
			for(int i=0; i<LENGTH;i++){
				com1[i]=&parent_group->indi[0]->gtype[i];
				com2[i]=&parent_group->indi[0]->gtype[i];
				cout << *com2[i] ;
			}
			cout << endl;
		}
		
		// 現在の世代parent_groupから次世代child_groupを作る。
		generate_population(child_group,parent_group,GAP,ELITE_RATE,P_MUTATE,P_CROSS);
		

		
		// 世代を入れ替える。
		parent_group = child_group;
		parent_group->save_generation();
	}

	
	//delete[] parent_group;
	//delete[] child_group;
	cout << "Complete" << endl;
}

void Othello::determine_pos(State _tc, bool human){
	VF cout << "Deciding strategy-" ;
	int maxVal = 0;
	int strategy=0;
	int minVal = nCell*3;
	////Decide strategy////
	////White////
	if ((*com1[gnum1]==0 || *com1[gnum1]==1 || *com1[gnum1]==2 || *com1[gnum1]==3 )&& human == false){
		if(Okeru1 == true) strategy = (*com1[gnum1]) %16;
		else if(Okeru2 == true) strategy = (*com1[gnum1] +4) %16;
		else if(Okeru3 == true) strategy = (*com1[gnum1] +8) %16;
		else if(Okeru4 == true) strategy = (*com1[gnum1] +12) %16;
		else  strategy=0;	
	}
	
	if ((*com1[gnum1]==4 || *com1[gnum1]==5 || *com1[gnum1]==6 || *com1[gnum1]==7 )&& human == false){
		if(Okeru2 == true) strategy = (*com1[gnum1]) %16;
		else if(Okeru3 == true) strategy = (*com1[gnum1] +4) %16;
		else if(Okeru4 == true) strategy = (*com1[gnum1] +8) %16;
		else if(Okeru1 == true) strategy = (*com1[gnum1] +12) %16;
		else  strategy=4;
	}
		if ((*com1[gnum1]==8 || *com1[gnum1]==9 || *com1[gnum1]==10 || *com1[gnum1]==11 ) && human == false){
		if(Okeru3 == true) strategy = (*com1[gnum1]) %16;
		else if(Okeru4 == true) strategy = (*com1[gnum1] +4) %16;
		else if(Okeru1 == true) strategy = (*com1[gnum1] +8) %16;
		else if(Okeru2 == true) strategy = (*com1[gnum1] +12) %16;
		else  strategy=8;
	}
	
	if ((*com1[gnum1]==12 || *com1[gnum1]==13 || *com1[gnum1]==14 || *com1[gnum1]==15 )&& human == false){
		if(Okeru4 == true) strategy = (*com1[gnum1]) %16;
		else if(Okeru1 == true) strategy = (*com1[gnum1] +4) %16;
		else if(Okeru2 == true) strategy = (*com1[gnum1] +8) %16;
		else if(Okeru3 == true) strategy = (*com1[gnum1] +12) %16;
		else  strategy=12;
	}
	
	
	////Black////
	
	if (*com2[gnum2]==0 || *com2[gnum2]==1 || *com2[gnum2]==2 || *com2[gnum2]==3 ){
		if(Okeru1 == true) strategy = (*com2[gnum2]) %16;
		else if(Okeru2 == true) strategy = (*com2[gnum2] +4) %16;
		else if(Okeru3 == true) strategy = (*com2[gnum2] +8) %16;
		else if(Okeru4 == true) strategy = (*com2[gnum2] +12) %16;
		else  strategy=0;
	}
	if (*com2[gnum2]==4 || *com2[gnum2]==5 || *com2[gnum2]==6 || *com2[gnum2]==7 ){
		if(Okeru2 == true) strategy = (*com2[gnum2]) %16;
		else if(Okeru3 == true) strategy = (*com2[gnum2] +4) %16;
		else if(Okeru4 == true) strategy = (*com2[gnum2] +8) %16;
		else if(Okeru1 == true) strategy = (*com2[gnum2] +12) %16;
		else  strategy=4;
	}
	if (*com2[gnum2]==8 || *com2[gnum2]==9 || *com2[gnum2]==10 || *com2[gnum2]==11 ){
		if(Okeru3 == true) strategy = (*com2[gnum2]) %16;
		else if(Okeru4 == true) strategy = (*com2[gnum2] +4) %16;
		else if(Okeru1 == true) strategy = (*com2[gnum2] +8) %16;
		else if(Okeru2 == true) strategy = (*com2[gnum2] +12) %16;
		else  strategy=8;
	}
	if (*com2[gnum2]==12 || *com2[gnum2]==13 || *com2[gnum2]==14 || *com2[gnum2]==15 ){
		if(Okeru4 == true) strategy = (*com2[gnum2]) %16;
		else if(Okeru1 == true) strategy = (*com2[gnum2] +4) %16;
		else if(Okeru2 == true) strategy = (*com2[gnum2] +8) %16;
		else if(Okeru3 == true) strategy = (*com2[gnum2] +12) %16;
		else  strategy=12;
	}

	VF cout << "complete-"<< "I chose Strategy-"<< strategy << endl;
	
	
	if(tc==White)VF cout << "White tern : "<<endl ;
	if(tc==Black && human == false)	VF cout << "Black tern : " <<endl;
	////Decide position////
	if (strategy==0){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][0] > maxVal ) {
				maxVal=pos_list[i][0];
				v= pos_list[i][1];
				h= pos_list[i][2];
			}
		}
	}
	else if (strategy==1){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][0] < minVal && pos_list[i][0] != 0) {
				minVal=pos_list[i][0];		
				v= pos_list[i][1];
				h= pos_list[i][2];
			}		
		}
	}
	else if (strategy==2){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][3] > maxVal ) {
				maxVal=pos_list[i][3];
				v= pos_list[i][4];
				h= pos_list[i][5];
			}
		}
	}
	else if (strategy==3){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][3] < minVal && pos_list[i][3] != 0) {
				minVal=pos_list[i][3];		
				v= pos_list[i][4];
				h= pos_list[i][5];
			}		
		}
	}
	else if (strategy==4){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][6] > maxVal ) {
				maxVal=pos_list[i][6];
				v= pos_list[i][7];
				h= pos_list[i][8];
			}
		}
	}
	else if (strategy==5){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][6] < minVal && pos_list[i][6] != 0) {
				minVal=pos_list[i][6];		
				v= pos_list[i][7];
				h= pos_list[i][8];
			}		
		}
	}
	else if (strategy==6){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][9] > maxVal ) {
				maxVal=pos_list[i][9];
				v= pos_list[i][10];
				h= pos_list[i][11];
			}
		}
	}
	else if (strategy==7){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][9] < minVal && pos_list[i][9] != 0) {
				minVal=pos_list[i][9];		
				v= pos_list[i][10];
				h= pos_list[i][11];
			}		
		}
	}	
	else if (strategy==8){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][12] > maxVal ) {
				maxVal=pos_list[i][12];
				v= pos_list[i][13];
				h= pos_list[i][14];
			}
		}
	}
	else if (strategy==9){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][12] < minVal && pos_list[i][12] != 0) {
				minVal=pos_list[i][12];		
				v= pos_list[i][13];
				h= pos_list[i][14];
			}		
		}
	}
	else if (strategy==10){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][15] > maxVal ) {
				maxVal=pos_list[i][15];
				v= pos_list[i][16];
				h= pos_list[i][17];
			}
		}
	}
	else if (strategy==11){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][15] < minVal && pos_list[i][15] != 0) {
				minVal=pos_list[i][15];		
				v= pos_list[i][16];
				h= pos_list[i][17];
			}		
		}
	}
	else if (strategy==12){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][18] > maxVal ) {
				maxVal=pos_list[i][18];
				v= pos_list[i][19];
				h= pos_list[i][20];
			}
		}
	}
	else if (strategy==13){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][18] < minVal && pos_list[i][18] != 0) {
				minVal=pos_list[i][18];		
				v= pos_list[i][19];
				h= pos_list[i][20];
			}		
		}
	}
	else if (strategy==14){
		for(int i = 0; i < nCell*3; i++) {
			if( pos_list[i][21] > maxVal ) {
				maxVal=pos_list[i][21];
				v= pos_list[i][22];
				h= pos_list[i][23];
			}
		}
	}
	else if (strategy==15){
		for(int i = nCell*3-1; i > 0; i--) {
			if( pos_list[i][21] < minVal && pos_list[i][21] != 0) {
				minVal=pos_list[i][21];		
				v= pos_list[i][22];
				h= pos_list[i][23];
			}		
		}
	}
	if(tc==Black && human == false)gnum1 += 1;
	if(tc==White)gnum2 += 1;
	VF cout << char(h+'A') << "," <<v+1<<endl;
	
}

void Othello::save_board(void){
	// ファイル出力ストリームの初期化
	ofstream ofs("virtual_board.txt");
	char k;
	// ファイルに1文字ずつ変換して書き込み
	for(int i=0; i<nCell; i++) {
		for(int j=0; j<nCell; j++) {
			switch(board[i][j]){
			  case Non: 
				k='N'; 
				break;
			  case Black:
				k='B';
				break;
			  case White:
				k='W';
				break;
			  default:
				k=0;

			}
			ofs << k ;
		}
		ofs << endl;
	}
	ofs << nTern <<endl;
	ofs << gnum1 <<endl;
	ofs << gnum2 <<endl;
}

void Othello::struct_virtual(void){
	//cout << "Structing board" <<endl;
	// ファイル出力ストリームの初期化
	ifstream ifs("virtual_board.txt");
	if(ifs.fail()) {
		cerr << "File do not exist.\n";
		exit(0);
	}
	string line;
	int i = 0;
	char k;
	// ファイルに1文字ずつ変換して読み込み
	while (getline(ifs, line)) {
		for(int j=0; j<nCell && i<nCell; j++) {
			k = line.at(j);
			switch(k){
			  case 'N': 
				board[i][j] = Non; 
				break;
			  case 'B':
				board[i][j] = Black; 
				break;
			  case 'W':
				board[i][j] = White; 
				break;
			}
		}
		if (i==nCell) nTern = atoi(line.c_str());
		if (i==nCell+1) gnum1 = atoi(line.c_str());
		if (i==nCell+2) gnum2 = atoi(line.c_str());
		i++;
	}
	//cout << "Complete"<<endl;
}


void Othello::init_Okeru(void){
	Okeru=false;
	Okeru1=false;
	Okeru2=false;
	Okeru3=false;
	Okeru4=false;
}


