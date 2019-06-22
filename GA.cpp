////GA.cpp////
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>
#include <memory.h>
#include "GA.h"
#include "param.h"
#include "Othello.h"

using namespace std;

////function////

////INDI////
INDI::INDI(void){
	ptype = 0;
	fitness = 0;
	parent1 = 0;
	parent2 = 0;
	cross_point = 0;	
}

INDI::INDI(int code_length, int code_max){
	ptype = 0;
	fitness = 0;
	parent1 = 0;
	parent2 = 0;
	cross_point = 0;
}

void* INDI::my_malloc(int size){
	void* ptr = malloc(size);
	if(!ptr){
		fprintf(stderr,"failed to maloc.\n");
		exit(1);
	}
	return ptr;
}
/* gtype(遺伝子コード)の実数GAのための実装 */

/*gtype(int配列)を作り最初のアドレスを返す*/
gtype_t INDI::mk_gtype(int code_length)
{
	gtype_t gtype = (gtype_t)my_malloc(sizeof(int)*code_length);
	return gtype;
}

/*gtypeのメモリを解放する*/
void INDI::free_gtype(gtype_t gtype)
{
	free(gtype);
	return;
}

/*ランダムにgtypeを作る*/
gtype_t INDI::mk_random_gtype(int code_length, int code_max)
{
	gtype_t ptr = mk_gtype(code_length);
	int i;
	for(i=0;i<code_length;i++){
		ptr[i] = rand()%(code_max+1);
	}
	return ptr;
}

/*与えられた実数に近い、ビットストリング型のgtypeを作る。
  GRAY=1が指定されているときはグレイ表現に変換。
  仕様：最後の桁まで計算した後に残る端数は切り捨て。
 */
void INDI::encode_gtype(double value, gtype_t gtype, int code_length, double min, double max){
	double gap = max-min;
	double remain_value = value - min; //値のうち、遺伝子によって表現されている部分
	double value_of_code; //その桁の遺伝子が表現する値
	int position =1;
	int pre_code=0; //一つ上位の桁の表現(バイナリ),バイナリとグレイの変換に必要	
	int i=0;
	int tmp; //グレイ表現変換用、一時保管変数
	while( i< code_length){
		value_of_code = gap/pow(2,position);
		if (remain_value == value_of_code){
			gtype[i] = 1;
			remain_value -= value_of_code;
		}else{
			gtype[i] = 0;
		}
		//グレイ表現への変換
		//バイナリ表現と、元のバイナリを右に１シフトしたもののXORを取る
		if (GRAY ==1){
			tmp = gtype[i];
			gtype[i] = (pre_code) ^ (gtype[i]);
			pre_code = tmp;
		}
		position++;
		i++;
	}
	return;
}

//与えられたgtypeから実数に変換する関数
//GRAY=1 が指定されているときはグレイ表現に変換
double INDI::decode_gtype(gtype_t gtype, int code_length, double min, double max){
	double gap = max- min;
	double decoded_value = min;
	int position =1;
	int pre_code=0; //一つ上位の桁の表現(バイナリ),バイナリとグレイの変換に必要

	//グレイ変換の解釈
	//変換されたバイナリの一つ上位の桁の表現との排他的論理和をとる
	if (GRAY == 1){
		for (int i=0;i<code_length;i++){
			pre_code = pre_code ^ gtype[i];
			if(pre_code){
				decoded_value += gap /pow(2,position);
				//最上位から順に、最大値と最小値の差の1/2,1/4,1/8,1/16,,,となる
			}
			position++;
		}
	}
	//バイナリ表現のとき
	else {
		for (int i=0;i<code_length;i++){
			if(gtype[i]){
				decoded_value += gap /pow(2,position);
				//最上位から順に、最大値と最小値の差の1/2,1/4,1/8,1/16,,,となる
			}
			position++;
		}
	}
	return decoded_value;
}

/* gtypeを表示する */
void INDI::print_gtype(gtype_t gtype,int length){
	int i=0;
	printf("[");
	while(i < length){
		if(gtype[i]<10){
			printf("%d",gtype[i]);
		}
		else{
			printf("(%d)",gtype[i]);
		}
		i++;
	}
	printf("]");
}



////POPU////
//コンストラクタ
POPU::POPU(int _pop, int _length, int _code_max){
	indi.reserve(POP*sizeof(POPU));
	rindi.reserve(POP*sizeof(POPU));
	pselect.reserve(POP);
	mk_init_ga_population(_pop, _length, _code_max);
}

void* POPU::my_malloc(int size){
	void* ptr = malloc(size);
	if(!ptr){
		fprintf(stderr,"failed to maloc.\n");
		exit(1);
	}
	return ptr;
}

/* 個体を作る。メモリ領域確保、初期化 */
void POPU::mk_gene(INDI* _indi, int code_length, int code_max){
	_indi->gtype = _indi->mk_random_gtype(code_length, code_max);
	return ;
}

/* gtype のコピー*/
void POPU::copy_gtype(gtype_t new_gtype, gtype_t old_gtype, int length){
	int i = 0;
	for(i=0;i<length;i++){
		new_gtype[i]=old_gtype[i];
	}
	return;
}

int POPU::cross_gtype(gtype_t gtype1, gtype_t gtype2, int length){
	int cross_point= rand()%(length-1);
	int i= cross_point+1;
	int tmp;
	while(i<length){
		tmp= gtype1[i];
		gtype1[i]= gtype2[i];
		gtype2[i]= tmp;
		i++;
	}
	return cross_point;
}

int POPU::mutate_gtype(gtype_t gtype, int length, int code_max, double pm){
	//エラー処理
	if(pm > 1.0 || pm <0.0){
		printf("%f mutation probability must be from 0.0 to 1.0 \n", pm);
		exit(-1);
	}
	int mutate_point= 0;
	int i= 0;
	double rm;
	for(i=0; i<length; i++){
		rm= (double)rand()/RAND_MAX;
		if (rm<pm){
			gtype[i]= rand()%(code_max+1);
			mutate_point++ ;
		}
	}
	return mutate_point;
}



/* 線形リスト用の隣接した要素の入れ替え、引数は先頭のindividual_tのアドレス */
void POPU::switch_gene(INDI *individual){
	INDI tmp_ptr1 =  *individual;
	INDI tmp_ptr2 =  *(individual+1);
	*(individual+1) = *individual;
	*individual = tmp_ptr1;
	(*individual) = tmp_ptr2;
	return;
}


/* 個体をコピーする */
void POPU::copy_gene(INDI *new_gene, INDI *old_gene, int code_length){

	copy_gtype(new_gene->gtype,old_gene->gtype,code_length);
	new_gene->ptype = old_gene->ptype;
	new_gene->fitness = old_gene->fitness;
	new_gene->parent1 = old_gene->rank;
	new_gene->parent2 = old_gene->rank;
	new_gene->cross_point = code_length-1;
	return;

}

/* 交叉、突然変異で子供を作る 突然変異回数を返す */
int POPU::mk_children_genes(INDI *child1, INDI *child2, INDI *parent1, INDI *parent2, int code_length,int code_max,double pm){
	int cross_point,mutateCount;
	copy_gene(child1,parent1,code_length);
	copy_gene(child2,parent2,code_length);
	child1->parent1 = parent1->rank;
	child1->parent2 = parent2->rank;
	cross_point = cross_gtype(child1->gtype,child2->gtype,code_length);
	child1->parent1 = parent1->rank;
	child1->parent2 = parent2->rank;
	child1->cross_point = cross_point;
	child2->parent1 = parent2->rank;
	child2->parent2 = parent1->rank;
	child2->cross_point = cross_point;
	mutateCount = mutate_gtype(child1->gtype,code_length,code_max,pm);
	mutateCount += mutate_gtype(child2->gtype,code_length,code_max,pm);
	return mutateCount;
}

/*GA集団の作成、初期化を行う*/
void POPU::mk_init_ga_population(int _population_size, int _code_length, int _code_max){
	//pselect = (double*)my_malloc(sizeof(double)*population_size);
	mutate_count = 0;
	population_size = _population_size;
	code_length = _code_length;
	code_max = _code_max;
	mk_gene(&genes, code_length, code_max);
	//indi[0] = genes;
	mk_gene(&rindi[0],code_length,code_max);
	indi.push_back(&rindi[0] );
	int i=1;
	for(i=1; i<population_size; i++){
		mk_gene(&rindi[i],code_length,code_max);
		indi.push_back(&rindi[i] );
	}
	return;
}

/* 指定した文字chを指定した長さlengthだけ繰り返す関数 */
/* print_population(・)の中で使われる。 */
void POPU::print_sequence(char ch, int length){
	int i = 0;
	for( i = 0; i < length; i++ ){
		printf("%c", ch);
	}
	return;
}

/* 集団を表示する。 */
/* 左から,世代数,親のインデックス,交叉点,gtype,ptype,fitnessを表示する。 */
/* また、最後に突然変異の回数を表示する。 */
void POPU::print_population(void){
	//		vector<INDI> member(indi);
	int i = 0;
	printf("-------------------");
	print_sequence('-', LENGTH+2);
	printf("---------------\n");
	printf("#   parents  xsite  gtype");
	print_sequence(' ', LENGTH-3);
	printf("ptype  fitness\n");

	while(i < POP){		
		printf("%-3d (%3d,%3d) %3d  ", i, indi[i]->parent1, indi[i]->parent2, indi[i]->cross_point);
		indi[i]->print_gtype(indi[i]->gtype, code_length);
		printf(" %+3.3f %d\n",indi[i]->ptype,indi[i]->fitness);
		i++;
	}
	printf("total mutate %d\n", mutate_count);
	
	
	
	return;
}

/* 適合度を出力
   最大,平均,最小
   CSV形式にする
 */
void POPU::print_fitness(void){
	printf("%d, %d, %d, %f, ",max_fitness,avg_fitness,min_fitness, indi[0]->ptype);
	indi[0]->print_gtype(indi[0]->gtype, code_length);
	printf("\n");
	

	return;
}

/* GA集団の個体線形リストgenesの一人一人のfitnessを見て
   配列pselectを作る
   1. pselect[i] = pselect[i-1]+fitness[i]
   2. pselect[i] = pselect[i]/pselect[POPULATION-1]
 */
void POPU::calc_pselect(void){

	//pselect.reserve(POP);
	pselect.resize(POP);
	int i;
	pselect[0] = indi[0]->fitness;
	for (i=1; i<population_size; i++){
		pselect[i]= pselect[i-1] + indi[i]->fitness;
	}

	for (i = 0; i<population_size; i++){
		pselect[i] /= pselect[population_size-1];
	}

	return;
}


/* 適合度の比較関数 */
/* individualA の適合度が低ければ0以外を返し、等しいか大きければ0を返す */
int POPU::less_than(INDI individualA, INDI individualB)
{
	return (individualA.fitness < individualB.fitness);
}

/* 適合度計算
   gtypeからptypeへの変換、fitnessの計算を行う
   適合度を計算した個体から適合度順に線形リストに挿入する。
   param.hのF_Xを関数f(x)として
   param.hのG_Yをg(f(x))として適合度とする。
   例題では
   f(x)=0となるxを求める。
 */
void POPU::calc_fitness(double value_min, double value_max){
	double x,y;
	for (int i=0; i<population_size;i++){
		indi[i]->set_fitness(0);
	}
	
	bool cnt;
	cnt =false;
	for( int i=0; i< population_size-1; i++ ){
		//indi[i]->set_ptype(x);
		for(int j=i+1; j<population_size; j++){
			Othello *othello;
			othello = new Othello(true) ;
			if(cnt ==true)othello->playCom(indi.at(i)->gtype, indi.at(j)->gtype);
			else if(cnt ==false)othello->playCom(indi.at(j)->gtype, indi.at(i)->gtype);
			if (othello->winFlag==Othello::Black){
				if(cnt ==true){
					indi[i]->add_fitness(othello->nb);
					indi[j]->add_fitness (othello->nw);
				}
				else if(cnt ==false){
					indi[j]->add_fitness(othello->nb);
					indi[i]->add_fitness (othello->nw);
				}
			}
			else if(othello->winFlag==Othello::White){
				if(cnt ==true){
					indi[j]->add_fitness(60);
					indi[i]->add_fitness (othello->nb);
				}
				else if(cnt ==false){
					indi[i]->add_fitness(60);
					indi[j]->add_fitness (othello->nb);
				}
			} else{
				//indi[i]->add_fitness (othello->nb);
				indi[j]->add_fitness (othello->nw);
				indi[i]->add_fitness (othello->nw);
			}
			//cout << i << "vs" <<j << endl;
			//cout << "Winner"<<othello->winFlag  << endl;
			//cout << indi[i]->fitness << "-" <<indi[j]->fitness << endl;
			//cout <<"-----------------" << endl;
			if(cnt==true) cnt=false;
			else if(cnt==false) cnt=true;
			//printf("[%d] get %d fitness \n", i, indi[i]->fitness); 
			//printf("[%d] get %d fitness \n", j, indi[j]->fitness); 
			delete othello;
		}
	}
	
	for( int i=0; i< population_size; i++ ){
		for (int j=i-1; j>=0; j--){			
			if ( indi[j]->fitness < indi[j+1]->fitness ){
				swap(indi[j],indi[j+1]);
			}			
		}	
	}
	return;
}



/* ルーレット方式による親選択 */
/* 親の選択
   　　1. ０から１までの乱数を生成する。
   　　2. pselectを見て親を選ぶ
   　　3. 選ばれた親を返す
 */
INDI* POPU::select_parent_roulette(void){
	int j=0;
	double r;
	INDI* parent;
	r= (double)rand()/RAND_MAX;
	parent = indi[0];

	/* pselect[j-1]<r<pselect[j]のとき、j番目が親になる */
	while( r> pselect[j]){
		parent = indi[j];
		j++;
	}
	return parent;
}

//トーナメント方式による親選択
INDI* POPU::select_parent_tournament(int tournament_size){
	int pop= population_size;
	int i,j,r,min = pop;
	//INDI min_selected = NULL;
	INDI* min_selected;
	INDI* ptr;
	for (i=0; i<tournament_size; i++){
		r =rand()%pop;
		if (min > r) min =r;
	}
	ptr = indi[0];
	for(j=0; j<min; j++) ptr = indi[j];
	min_selected = ptr;
	return min_selected;
}


/* 親個体の選択、param.hのSELECTION_METHODによって
   ルーレット選択かトーナメント選択を行う */
INDI* POPU::select_parent(void){
	INDI *parent;
	switch(SELECTION_METHOD){
		case 1:
			parent = select_parent_roulette();
			break;
		case 2:
			parent = select_parent_tournament(TOURNAMENT_SIZE);
			break;
		default:
			fprintf(stderr,"invalid number on SELECTION_METHOD\n");
			exit(1);
	}
	return parent;
}

/* 
   適合度順に並んだ線形リストから
   最大値、最小値、平均値を記録、順番付け
 */
void POPU::normalize_population(void){
	int i;
	vector<INDI*> tmp;
	tmp.reserve(POP);
	tmp.assign(indi.begin(), indi.end());
	max_fitness = indi[0]->fitness; /* 先頭の適合度が最大適合度 */
	double avg = 0.0;
	/* 順番付け*/
	for(i=0;i < population_size;i++){
		avg += tmp[i]->fitness;
		tmp[i]->rank = i;
		min_fitness = tmp[i]->fitness;
	}
	avg = avg/population_size;
	avg_fitness = avg;

	tmp.clear();
	return;
}

void POPU::save_generation(void){
	// ファイル出力ストリームの初期化
	ofstream ofs("save_generation.txt");
	string str;
	
	for(int i=0; i<population_size;i++){
		for(int j=0; j< code_length;j++){
			ofs << indi[i]->gtype[j] ;
		}
		ofs<<endl;
	}
}


/* 新しい世代の生成
   new_populationのメモリ領域はすでに確保してあるとする
   必ずソート済みのpopulationを渡すこと
 */
void generate_population(POPU *new_population, POPU *old_population, double gap, double elite_rate, double mutate_prob, double crossover_prob){
	int num_of_remain = (int)(old_population->population_size*(1-gap)); /*親世代からコピーする数*/
	int num_of_elite = (int)(num_of_remain*elite_rate);  /*コピー枠のうちエリートの数*/
	int generated;
	double rand_double;

	vector<INDI*> old_gene(POP);
	//old_gene.reserve(POP);
	old_gene.assign(old_population->indi.begin(), old_population->indi.end());
	vector<INDI*> new_gene(POP);
	//new_gene.reserve(POP);
	new_gene.assign(new_population->indi.begin(), new_population->indi.end());
	//INDI old_gene = old_population.indi[0];
	//INDI new_gene = new_population.indi[0];

	/* 親選択テーブルを準備 */
	old_population->calc_pselect();


	/* エリート戦略 親世代での上位一定数はそのまま子供になる */
	for(generated = 0 ;generated < num_of_elite ; generated++){
		new_population->copy_gene(new_gene[generated],old_gene[generated],old_population->code_length);
		//old_gene = old_population.indi[generated+1];
		//new_gene = new_population.indi[generated+1];
	}
	/* エリート以外のそのまま子供になる枠 */
	for( ; generated < num_of_remain ; generated++){
		//new_population.copy_gene(new_gene,old_population.select_parent(),old_population.code_length);
		//new_population->copy_gene(new_gene[generated],old_population->select_parent(),old_population->code_length);

		//new_gene = new_population.indi[generated+1];
	}

	new_population->mutate_count = 0;
	/* 交叉・突然変異を適応する枠 */
	/* 残り個体数が奇数の時は、一つだけ突然変異で子供を作る */
	if( (old_population->population_size - generated)%2 == 1){
		//new_population.copy_gene(new_gene,old_population.select_parent(),old_population.code_length);
		//new_population.mutate_count += new_population.mutate_gtype(new_gene.gtype,old_population.code_length,old_population.code_max,mutate_prob);
		new_population->copy_gene(new_gene[generated],old_population->select_parent(),old_population->code_length);

		new_population->mutate_count += new_population->mutate_gtype(new_gene[generated]->gtype,old_population->code_length,old_population->code_max,mutate_prob);
		//new_gene = new_population.indi[generated+1];
		generated++;

	}

	/* 交叉・突然変異をする */
	for(; generated < old_population->population_size; generated += 2){
		rand_double = (double)rand()/RAND_MAX;
		/* 交叉するとき */
		if(rand_double < crossover_prob){
			//new_population.mutate_count += new_population.mk_children_genes(new_gene,new_population.indi[generated+1],old_population.select_parent(),old_population.select_parent(),old_population.code_length,old_population.code_max,mutate_prob);
			new_population->mutate_count += new_population->mk_children_genes(new_gene[generated],new_gene[generated],old_population->select_parent(),old_population->select_parent(),old_population->code_length,old_population->code_max,mutate_prob);
			//new_gene = new_population.indi[generated+2];
		}
		/* 交叉しないとき */
		else{
			//new_population.copy_gene(new_gene,old_population.select_parent(),old_population.code_length);
			//new_population.mutate_count += new_population.mutate_gtype(new_gene.gtype,old_population.code_length,old_population.code_max,mutate_prob);
			new_population->copy_gene(new_gene[generated],old_population->select_parent(),old_population->code_length);
			new_population->mutate_count += new_population->mutate_gtype(new_gene[generated]->gtype,old_population->code_length,old_population->code_max,mutate_prob);
			//new_gene = new_population.indi[generated+1];
			//new_population.copy_gene(new_gene,old_population.select_parent(),old_population.code_length);
			//new_population.mutate_count += new_population.mutate_gtype(new_gene.gtype,old_population.code_length,old_population.code_max,mutate_prob);
			new_population->copy_gene(new_gene[generated],old_population->select_parent(),old_population->code_length);
			new_population->mutate_count += new_population->mutate_gtype(new_gene[generated]->gtype,old_population->code_length,old_population->code_max,mutate_prob);
			//new_gene = new_population.indi[generated+1];
		}

	}
	new_gene.clear();
	old_gene.clear();
	return;
}

