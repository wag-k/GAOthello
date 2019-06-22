#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <list>
#include "param.h"
#include "Othello.h"

using namespace std;

/* intの配列をGAの遺伝子型として別名を付ける。 */
/*   プログラム中ではgtype_tを使う。*/
typedef int* gtype_t;
/*
class memory {
  public:
	memory();
	void* my_malloc(int _size);
	
};

  メモリ確保のための関数。確保できなかった場合はエラー・終了。

void* memory::my_malloc(int size){
  void* ptr = malloc(size);
  if(!ptr){
    fprintf(stderr,"failed to maloc.\n");
    exit(1);
  }
  return ptr;
}
 */

//個別のクラス 
//void* my_malloc(int size)を継承
class INDI {
	
  public:
	gtype_t gene_ptr;
	gtype_t gtype;  /*遺伝子型 int配列の先頭のポインタ*/
	double ptype; //表現型
	int fitness; //適合度
	int rank; //線形リストの中での順位(ソート後)
	int parent1; //交差での親１のインデックス
	int parent2; //交差での親２のインデックス
	int cross_point; //交差したポイント
	
	////function////
	INDI(void);
	INDI(int code_length, int code_max);
	// コピーコンストラクタは、メンバー変数のコピーなので必要ないが一応作っておく
	// [const]を忘れると、エラーになる
	
    INDI(const INDI &source){
		fitness  = source.fitness;
		ptype = source.ptype;
    }
	
	void* my_malloc(int _size);
	void set_ptype(double _ptype){ ptype=_ptype; }
	void set_fitness(double _fitness){ fitness = _fitness;}
	void add_fitness(double _fitness){ fitness += _fitness;}
	
	// 比較演算子(findで必要)
    bool operator==(const INDI &a)const{
        return fitness == a.fitness;
    }
    // 比較演算子(sortで必要)
    bool operator<(const INDI &a )const{
        return fitness < a.fitness;
    }
	static int up( INDI& a, INDI& b){
    return (a.fitness < b.fitness);
	}

	/*gtype(int配列)を作り最初のアドレスを返す*/
	gtype_t mk_gtype(int code_length);
	/*gtypeのメモリを解放する*/
	void free_gtype(gtype_t gtype);
	/*ランダムにgtypeを作る*/
	gtype_t mk_random_gtype(int code_length, int code_max);
	void encode_gtype(double value, gtype_t gtype, int code_length, double min, double max);
	double decode_gtype(void);
	double decode_gtype(gtype_t gtype, int code_length, double min, double max);
	/* gtypeを表示する */
	void print_gtype(gtype_t gtype,int length);
};

//集団のクラス
class POPU {
  public:
	INDI genes;/* 個体の連結リスト先頭のポインタ */
	vector<INDI*> indi; // 線形リストの中での次の個体
	vector<INDI> rindi;
	vector<double> pselect; //適合度の配列
	int mutate_count; //突然変異回数の合計
	int max_fitness, min_fitness, avg_fitness; //適合度の最大、最小、平均値
	int population_size; //集団の個体数
	int code_length; //遺伝子長
	int code_max; //各遺伝子座の最大値、ビットストリングの場合は１
	int tournament_size;	
	
	////function for operation////
	//constructer
	POPU(int _pop, int _length, int _code_max);
	~POPU(){
		indi.clear();
		rindi.clear();
		pselect.clear();
	}
	void* my_malloc(int _size);
	/* 個体を作る。メモリ領域確保、初期化 */
	void mk_gene(INDI* _indi, int code_length, int code_max);
	/* gtype のコピー*/
	void copy_gtype(gtype_t new_gtype, gtype_t old_gtype, int length);
	int cross_gtype(void);
	int cross_gtype(gtype_t gtype1, gtype_t gtype2, int length);
	int mutate_gtype(void);
	int mutate_gtype(gtype_t gtype, int length, int code_max, double pm);

	/* 線形リスト用の隣接した要素の入れ替え、引数は先頭のindividual_tのアドレス */
	void switch_gene(INDI *individual);
	/* 個体をコピーする */
	void copy_gene(INDI *new_gene, INDI *old_gene, int code_length);
	/* 交叉、突然変異で子供を作る 突然変異回数を返す */
	int mk_children_genes(INDI *child1, INDI *child2, INDI *parent1, INDI *parent2, int code_length,int code_max,double pm);
	/*GA集団の作成、初期化を行う*/
	void mk_init_ga_population(int _population_size, int _code_length, int _code_max);
	/* 指定した文字chを指定した長さlengthだけ繰り返す関数 */
	/* print_population(・)の中で使われる。 */
	void print_sequence(char ch, int length);
	/* 集団を表示する。 */
	/* 左から,世代数,親のインデックス,交叉点,gtype,ptype,fitnessを表示する。 */
	/* また、最後に突然変異の回数を表示する。 */
	void print_population(void);
	/* 適合度を出力
	最大,平均,最小
	CSV形式にする
	*/
	void print_fitness(void);
	void calc_pselect(void);
	/* 適合度の比較関数 */
	/* individualA の適合度が低ければ0以外を返し、等しいか大きければ0を返す */
	int less_than(INDI individualA, INDI individualB);
	/* 適合度計算*/
	void calc_fitness(double value_min, double value_max);
	INDI* select_parent(void);
	/* 
	適合度順に並んだ線形リストから
	最大値、最小値、平均値を記録、順番付け
	*/
	void normalize_population(void);
	void save_generation(void);
	
	INDI* select_parent_roulette(void);
	INDI select_parent_tournament(void);
	INDI* select_parent_tournament(int _tournament_size);
	
};


/* 新しい世代の生成
   new_populationのメモリ領域はすでに確保してあるとする
   必ずソート済みのpopulationを渡すこと
*/
void generate_population(POPU *new_population, POPU *old_population, double gap, double elite_rate, double mutate_prob, double crossover_prob);


#endif
