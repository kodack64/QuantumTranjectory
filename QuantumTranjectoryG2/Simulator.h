
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

	// id����ʎq�����擾����}�N��
	inline int getIdToPG(int i){return (i%indAE)/indPG;}
	inline int getIdToAE(int i){return (i%indPF)/indAE;}
	inline int getIdToPF(int i){return (i%indAF)/indPF;}
	inline int getIdToAF(int i){return (i%vecSize)/indAF;}

	// ���[�v�ϐ�
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

	// �p���X�̕��f�U���Ɨ���
	complex<double> pulse;
	complex<double> pulseRot;

	// ����step�ŗʎq������ꂽ���ǂ���
	bool flagLossAtom;
	bool flagLossControl;
	bool flagLossProbe;

	//����step�ŗʎq��������m��
	double probLossProbe;
	double probLossControl;
	double probLossAtom;

	//�ʎq�������Ȃ������ꍇ���|�X�g�Z���N�g���邩
	bool postselAtom;
	bool postselControl;
	bool postselProbe;

	//�ő�Ƃ��Đ؂藎�Ƃ��ʎq�̐�
	int maxPG;
	int maxAE;
	int maxPF;
	int maxAF;
	int totAtom;

	//�e���X�𖳎����邩
	bool useLossProbe;
	bool useLossAtom;
	bool useLossControl;

	//	time
	double dt;
	long maxstep;

	//���͂��p���X�`��ɂ��邩�ǂ���
	bool usePulse;
	//�p���X���x
	double pulsePump;
	//���̗͂���
	double pulseDetune;
	//�p���X�̐���
	double pulseWidth;
	//�p���X��ł��؂�{��
	double pulseCut;

	//���ݍ�p�̋���
	double coherenceProbe;
	double coherenceControl;

	//���X�̑��x
	double lossProbe;
	double lossControl;
	double lossAtom;

	//�����I�Ƀ��X������^�C�~���O
	int forceLossProbeTime;
	int forceLossProbeTimeSub;

	// �I������
	double eps;

	// ���O�֘A
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

	// id��U�邽�߂̐�
	int indPG;
	int indAE;
	int indPF;
	int indAF;
	int vecSize;
	int maxEne;

	// �����P��
	complex<double> img;

	// �����W�F�l���[�^
	Random *r;

	//��Ԋ֐��ƍ���
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