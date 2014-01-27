
#pragma once

#include <complex>
#include <vector>
#include <string>
using namespace std;

class ParameterSet;
class Random;

class Simulator{
private:

	int _unit;
	int _id;

	virtual void init(ParameterSet*);
	virtual void loadParameter(ParameterSet*);
	virtual void initParameter();
	virtual void initLogging();

	virtual void run();
	virtual void calcPulseSize();
	virtual void calcLiuville();
	virtual void calcLindblad();
	virtual void calcProbabiliyOfLoss();
	virtual void calcProjection();
	virtual void logging();
	virtual void checkCondition();

	virtual void close();
	virtual void releaseParameter();
	virtual void loggingSave();

	// idから量子数を取得するマクロ
	inline int getIdToPG(int i){return (i%indAE)/indPG;}
	inline int getIdToAE(int i){return (i%indPF)/indAE;}
	inline int getIdToPF(int i){return (i%indAF)/indPF;}
	inline int getIdToAF(int i){return (i%vecSize)/indAF;}

	// ループ変数
	int pg;
	int ae;
	int pf;
	int af;
	double trace;
	double energy;
	double edgePG;
	double edgeAE;
	double edgePF;
	double edgeAF;
	double edgeAll;
	int step;
	int i;
	double sum;
	bool endFlag;
	int seed;

	// パルスの複素振幅と離調
	complex<double> pulse;
	complex<double> pulseRot;

	// そのstepで量子が失われたかどうか
	bool flagLossAtom;
	bool flagLossControl;
	bool flagLossProbe;

	//そのstepで量子が失われる確率
	double probLossProbe;
	double probLossControl;
	double probLossAtom;

	//量子が失われなかった場合をポストセレクトするか
	bool postselAtom;
	bool postselControl;
	bool postselProbe;

	//最大として切り落とす量子の数
	int maxPG;
	int maxAE;
	int maxPF;
	int maxAF;
	int totAtom;

	//各ロスを無視するか
	bool useLossProbe;
	bool useLossAtom;
	bool useLossControl;

	//	time
	double dt;
	long maxstep;

	//入力をパルス形状にするかどうか
	bool usePulse;
	//パルス強度
	double pulsePump;
	//入力の離調
	double pulseDetune;
	//パルスの線幅
	double pulseWidth;
	//パルスを打ち切る倍率
	double pulseCut;

	//相互作用の強さ
	double coherenceProbe;
	double coherenceControl;

	//ロスの速度
	double lossProbe;
	double lossControl;
	double lossAtom;

	//強制的にロスさせるタイミング
	int forceLossProbeTime;
	int forceLossProbeTimeSub;

	// 終了条件
	double eps;

	// ログ関連
	int loggingUnit;
	bool loggingTime;
	bool loggingProb;
	bool loggingJump;
	string loggingLossTimeName;
	string loggingLossProbabilityName;
	vector<double> lossTimeLogAtom;
	vector<double> lossTimeLogControl;
	vector<double> lossTimeLogProbe;
	vector<double> lossProbabilityLogAtom;
	vector<double> lossProbabilityLogControl;
	vector<double> lossProbabilityLogProbe;
	vector<double> g2ValueLogProbe;
	vector<double> g2ValueLogControl;
	vector<double> g2ValueLogAtom;
	int totProbeLoss;
	int totControlLoss;
	int totAtomLoss;
	double maxEdge;
	double g2Probe;
	double g2Control;
	double g2Atom;
	int loggingOffset;

	// idを振るための数
	int indPG;
	int indAE;
	int indPF;
	int indAF;
	int vecSize;
	int maxEne;

	// 虚数単位
	complex<double> img;

	// 乱数ジェネレータ
	Random *r;

	//状態関数と差分
	complex<double>* state;
	complex<double>* dif;
	double* ene;

public:
	virtual void execute(int unit,int id,ParameterSet* param);
	virtual int getTotalProbeLossCount(){return totProbeLoss;}
	virtual int getTotalControlLossCount(){return totControlLoss;}
	virtual int getTotalAtomLossCount(){return totAtomLoss;}
	virtual double getMaxEdge(){return maxEdge;}
};