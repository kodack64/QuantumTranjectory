
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <iostream>
#include <complex>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

int muldown(int p,int c){
	int rtn=1;
	for(int i=0;i<c;i++){
		rtn*=(p-i);
	}
	return rtn;
}
int comb(int n,int m){
	if(m==0)return 1;
	return muldown(n,m)/muldown(m,m);
}
int energyDimension(int e){
	return comb(e+2,2);
}
int energyDimensionLowerThan(int e){
	int rtn=0;
	for(int i=0;i<e;i++)rtn+=comb(i+2,i);
	return rtn;
}

class StateNumber{
public:
	int excited;
	int control;
	int probe;
	StateNumber(int p,int e,int c){
		probe=p;
		control=c;
		excited=e;
	}
	StateNumber(int id){
		setId(id);
	}
	int getEnergy(){
		return probe+excited+control;
	}
	int getId(){
		int ene=getEnergy();
		return energyDimensionLowerThan(ene)+probe*(ene+1)-probe*(probe-1)/2+excited;
	}
	void setId(int id){
		int ene=0;
		while(id-energyDimensionLowerThan(ene)>=energyDimension(ene))ene++;
		int lid = id - energyDimensionLowerThan(ene);
		probe=0;
		while(lid-((probe+1)*(ene+1)-probe*(probe+1)/2)>=0)probe++;
		excited = lid-(probe*(ene+1)-probe*(probe-1)/2);
		control = ene-probe-excited;
//		cout << probe << " " << excited << " " << control << endl;
	}
};

class CoefCalc{
public:
	double kp;
	double kc;
	double r;
	double gp;
	double gc;
	double eps;
	int totalAtom;
	CoefCalc(double _gp,double _gc,double _kp,double _kc,double _r,double _eps,int totAtom)
	:kp(_kp),
	kc(_kc),
	r(_r),
	gp(_gp),
	gc(_gc),
	totalAtom(totAtom),
	eps(_eps){
	}

	double etap(){
		return 4*gp*gp/r/kp;
	}
	double etac(){
		return 4*gc*gc/r/kc;
	}
	double transpro(){
		return pow((1.0+etac())/(1+totalAtom*etap()+etac()),2);
	}
	double transspn(){
		double gpe = gp*sqrt(1.0*totalAtom);
		return pow((gpe/r)/(1+totalAtom*etap()+etac()),2)*r/kp;
	}
	double transcon(){
		double gpe = gp*sqrt(1.0*totalAtom);
		return pow((gpe*gc/r/kc)/(1+totalAtom*etap()+etac()),2)*kc/kp;
	}
	complex<double> getCoef(int id1,int id2){
		StateNumber sn1(id1);
		StateNumber sn2(id2);
		if(sn1.getEnergy()==sn2.getEnergy()){
			if((sn1.excited==sn2.excited+1 && sn1.probe+1==sn2.probe)
			|| (sn1.excited+1==sn2.excited && sn1.probe==sn2.probe+1)){
				return gp*sqrt(1.0*max(sn1.excited,sn2.excited)*max(sn1.probe,sn2.probe)*max(totalAtom-sn1.excited-sn1.control,totalAtom-sn2.excited-sn2.control))*complex<double>(0,1);
			}
			if((sn1.excited==sn2.excited+1 && sn1.control+1==sn2.control)
			|| (sn1.excited+1==sn2.excited && sn1.control==sn2.control+1)){
				return gc*sqrt(1.0*max(sn1.excited,sn2.excited))*max(sn1.control,sn2.control)*complex<double>(0,1);
			}
			if(sn1.excited==sn2.excited && sn1.probe==sn2.probe){
				return -kp/2.0*sn1.probe-kc/2.0*sn1.control-r/2.0*sn1.excited;
			}
		}
		if(sn1.excited==sn2.excited && sn1.control==sn2.control && (sn1.probe==sn2.probe+1 || sn1.probe+1==sn2.probe)){
			return eps*sqrt(1.0*max(sn1.probe,sn2.probe))*complex<double>(0,1);
		}
		return 0;
	}
};

class Solver{
public:
	int maxene; // 考える最大のエネルギーのmanifold
	int matdim; // 最大エネルギーから定まる次元
	CoefCalc* cc; // 係数の計算

	Eigen::MatrixXcd mat; // 非エルミートの時間発展行列
	Eigen::VectorXcd eigenvec; // 最大固有値固有ベクトル
	Eigen::VectorXd eigenvecpos; // 最大固有値固有ベクトルのノルム
	Eigen::VectorXcd eigenval; // 固有値リスト
	vector<double> eigenvalreal; // 固有値の実部

	vector<double> coherenceTime; // コヒーレンス時間
	double np; // probe光子の期待値
	vector<double> npene; // [energy]のmanifoldでの光子数期待値
	vector<double> npnum; // [probe]個の光子を持つstateの確率和
	vector<double> corp; // [dim]次光子相関
	double nc;
	vector<double> ncene;
	vector<double> ncnum;
	vector<double> corc;
	double ne;
	vector<double> neene;
	vector<double> nenum;
	vector<double> core;

	double meanEne; // エネルギーの期待値
	vector<double> enepos; // [energy]の専有確率
	vector<double> corpos; // [energy]での相関

	double transpro;
	double transref;
	double transspn;
	double transcon;

	Solver(){
		maxene=3;
		cc = new CoefCalc(0.1,1.0,1.0,0.03,6,0.01,1000);
	}
	virtual ~Solver(){
		delete cc;
	}

	void setup(int _maxene,double gp,double gc,double kp,double kc,double r,double eps,int totAtom){
		maxene=_maxene;
		cc->gp=gp;
		cc->gc=gc;
		cc->kp=kp;
		cc->kc=kc;
		cc->r=r;
		cc->eps=eps;
		cc->totalAtom=totAtom;
	}

	void solve(){
		// init param
		matdim = energyDimensionLowerThan(maxene+1);
		mat = Eigen::MatrixXcd::Zero(matdim,matdim);
		for(int i=0;i<matdim;i++){
			for(int j=i;j<matdim;j++){
				if(i==j) mat(i,j)=cc->getCoef(i,j);
				else mat(i,j)=mat(j,i)=cc->getCoef(i,j);
			}
		}

		// calc
		Eigen::ComplexEigenSolver<Eigen::MatrixXcd> es(mat);
		eigenval =  es.eigenvalues();

		// find (sub) stat id
		double statVal=0;
		int statValId=-1;
		for(int i=0;i<eigenval.size();i++){
			if(statValId==-1 || statVal<eigenval(i).real()){
				statValId = i;
				statVal = eigenval(i).real();
			}
		}
		eigenvec = es.eigenvectors().col(statValId);
		eigenvecpos = Eigen::VectorXd(matdim);
		for(int i=0;i<matdim;i++)eigenvecpos(i)=norm(eigenvec(i));

//		cout << eigenvecpos << endl;

		// clear analyzer
		coherenceTime.clear();
		eigenvalreal.clear();
		np=nc=ne=meanEne=0;
		npene.resize(maxene+1);fill(npene.begin(),npene.end(),0);
		npnum.resize(maxene+1);fill(npnum.begin(),npnum.end(),0);
		ncene.resize(maxene+1);fill(ncene.begin(),ncene.end(),0);
		ncnum.resize(maxene+1);fill(ncnum.begin(),ncnum.end(),0);
		neene.resize(maxene+1);fill(neene.begin(),neene.end(),0);
		nenum.resize(maxene+1);fill(nenum.begin(),nenum.end(),0);
		corp.resize(maxene+1);corc.resize(maxene+1);core.resize(maxene+1);
		enepos.resize(maxene+1);fill(enepos.begin(),enepos.end(),0);
		corpos.resize(maxene+1);

		// set stat vec
		for(int i=0;i<matdim;i++){
			eigenvalreal.push_back(eigenval(i).real());
		}
		sort(eigenvalreal.rbegin(),eigenvalreal.rend());
		for(int i=0;i<matdim;i++){
			if(i==statValId)continue;
			coherenceTime.push_back(1.0/(statVal-eigenval(i).real()));
		}
		sort(coherenceTime.begin(),coherenceTime.end());

		for(int i=0;i<matdim;i++){
			StateNumber sn(i);
			double prob = norm(eigenvec(i));

			np+=prob*sn.probe;
			nc+=prob*sn.control;
			ne+=prob*sn.excited;
			meanEne+=prob*sn.getEnergy();
			npene[sn.getEnergy()]+=prob*sn.probe;
			ncene[sn.getEnergy()]+=prob*sn.control;
			neene[sn.getEnergy()]+=prob*sn.excited;
			npnum[sn.probe]+=prob;
			ncnum[sn.control]+=prob;
			nenum[sn.excited]+=prob;

			enepos[sn.getEnergy()]+=prob;
		}
		for(int e=2;e<=maxene;e++){
			double cp,cc,ce,cene;
			cp=cc=ce=cene=0;
			for(int k=0;k<=maxene;k++){
				cp+=npnum[k]*muldown(k,e);
				cc+=ncnum[k]*muldown(k,e);
				ce+=nenum[k]*muldown(k,e);
				cene+=enepos[k]*muldown(k,e);
			}
			corp[e]=cp/pow(np,e);
			corc[e]=cc/pow(nc,e);
			core[e]=ce/pow(ne,e);
			corpos[e]=cene/pow(meanEne,e);
		}

		// trans
		transpro = cc->transpro();
		transspn = transpro * ne/np*cc->r/cc->kp;
		transcon = transpro * nc/np*cc->kc/cc->kp;
		transref = 1-transpro-transspn-transcon;
	}
};



int main(){
	double gp=0.1;
	double gc=1.0;
	double kp=1.0;
	double kc=0.03;
	double r=6.0;
	double eps=1e-5;
	int totAtom=1000;
	int maxene=2;
	string output;

	fstream fs("_config.txt",ios::in);
	if(fs && !fs.bad()){
		string str;
		stringstream ss;
		getline(fs,str);ss << (str);ss >> gp;ss.str("");
		getline(fs,str);ss << (str);ss >> gc;ss.str("");
		getline(fs,str);ss << (str);ss >> kp;ss.str("");
		getline(fs,str);ss << (str);ss >> kc;ss.str("");
		getline(fs,str);ss << (str);ss >> r;ss.str("");
		getline(fs,str);ss << (str);ss >> eps;ss.str("");
		getline(fs,str);ss << (str);ss >> totAtom;ss.str("");
		getline(fs,str);ss << (str);ss >> maxene;ss.str("");
		getline(fs,str);ss << (str);ss >> output;ss.str("");
		fs.close();
	}

	fstream fout(output,ios::out);
	Solver* s = new Solver();
	for(gp=0;gp<=10.0;gp+=0.01){
		s->setup(maxene,gp,gc,kp,kc,r,eps,totAtom);
		s->solve();

		fout << gp << " ";
/*		for(int i=0;i<s->eigenvalreal.size();i++){
			fout << -s->eigenvalreal[i] << " ";
		}*/
//		fout << s->corc[2] << " " << s->corc[3];
		fout << s->corp[2] << " " << s->corc[2] << " " << s->transpro;
		fout << endl;
 	}
	delete s;
	fout.close();

	return 0;
}