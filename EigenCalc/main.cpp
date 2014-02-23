
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <iostream>
#include <complex>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

#define mg00 0
#define mg10 1
#define me00 2
#define	mf01 3
#define mg20 4
#define me10 5 
#define mf11 6
#define mef01 7
#define m2f02 8
#define m2e00 9
const int n=10;

class CalcDiagonal{
public:
	double eps;
	double k1;
	double k2;
	double r;
	double gp;
	double gc;
	Eigen::MatrixXcd mat;
	void init(){
		mat = Eigen::MatrixXcd::Zero(n,n);
		mat(mg00,mg00)=0;

		mat(mg10,mg10)=-k1;
		mat(me00,me00)=-r;
		mat(mf01,mf01)=-k2;
	
		mat(mg20,mg20)=-2*k1;
		mat(me10,me10)=-k1-r;
		mat(mf11,mf11)=-k1-k2;
		mat(mef01,mef01)=-r-k2;
		mat(m2f02,m2f02)=-2*k2;
		mat(m2e00,m2e00)=-2*r;

		mat(mg00,mg10) = mat(mg10,mg00) = complex<double>(0,eps);
		mat(mg10,mg20) = mat(mg20,mg10) = complex<double>(0,sqrt(2)*eps);
		mat(me00,me10) = mat(me10,me00) = complex<double>(0,eps);
		mat(mf01,mf11) = mat(mf11,mf01) = complex<double>(0,eps);

		mat(mg10,me00) = mat(me00,mg10) = complex<double>(0,gp);
		mat(mf01,me00) = mat(me00,mf01) = complex<double>(0,gc);

		mat(mg20,me10) = mat(me10,mg20) = complex<double>(0,sqrt(2)*gp);
		mat(mf11,me10) = mat(me10,mf11) = complex<double>(0,gc);
		mat(mf11,mef01) = mat(mef01,mf11) = complex<double>(0,gp);
		mat(m2f02,mef01) = mat(mef01,m2f02) = complex<double>(0,2*gc);

		mat(m2e00,me10) = mat(me10,m2e00) = complex<double>(0,sqrt(2)*gp);
		mat(m2e00,mef01) = mat(mef01,m2e00) = complex<double>(0,sqrt(2)*gc);
	}

	double np1;
	double np2;
	double nc1;
	double nc2;
	double ne1;
	double ne2;
	double g2p;
	double g2c;
	double g2e;
	double maxreal;
	double secreal;
	double coherence;
	double trans;
	double pos0;
	double pos1;
	double pos2;
	double dark0;
	double dark1;
	double dark2;
	Eigen::VectorXd eigenvecpos;
	Eigen::VectorXcd eigenval;
	virtual void compute(){
		Eigen::ComplexEigenSolver<Eigen::MatrixXcd> es(mat);
		eigenval =  es.eigenvalues();

		vector<double> reals;
		for(int i=0;i<n;i++)reals.push_back(eigenval(i).real());
		int maxind;
		maxreal=-1e10;
		for(int i=0;i<n;i++)if(reals[i]>maxreal){
			maxreal=reals[i];
			maxind=i;
		}
		sort(reals.rbegin(),reals.rend());
		double secreal = reals[1];
		Eigen::VectorXcd eigenvec = es.eigenvectors().col(maxind);
		eigenvecpos = Eigen::VectorXd(n);
		for(int i=0;i<n;i++){
			eigenvecpos(i) = norm(eigenvec(i));
		}

		coherence = 1.0/(maxreal-secreal);
		np1 = eigenvecpos(mg10);
		nc1 = eigenvecpos(mf01);
		ne1 = eigenvecpos(me00);
		np2 = 2*eigenvecpos(mg20)+eigenvecpos(me10)+eigenvecpos(mf11);
		nc2 = eigenvecpos(mf11)+eigenvecpos(mef01)+2*eigenvecpos(m2f02);
		ne2 = eigenvecpos(me10)+eigenvecpos(mef01)+2*eigenvecpos(m2e00);
		g2p=2*eigenvecpos(mg20)/pow(eigenvecpos(mg10),2);
		g2c=2*eigenvecpos(m2f02)/pow(eigenvecpos(mf01),2);
		g2e=2*eigenvecpos(m2e00)/pow(eigenvecpos(me00),2);

		pos0 = eigenvecpos(mg00);
		pos1 = eigenvecpos(mg10)+eigenvecpos(me00)+eigenvecpos(mf01);
		pos2 = eigenvecpos(mg20)+eigenvecpos(me10)+eigenvecpos(mf11)+eigenvecpos(mef01)+eigenvecpos(m2f02)+eigenvecpos(m2e00);

		if(gp==0 && gc==0){
			dark0=pos0;
			dark1=dark2=0;
		}else{
			dark0 = pos0;
			dark1 = norm((eigenvec(mg10)/sqrt(pos1)*gc-eigenvec(mf01)/sqrt(pos1)*gp)/sqrt(gp*gp+gc*gc));
			dark2 = norm((eigenvec(mg20)/sqrt(pos2)*gc*gc/sqrt(2.0)-eigenvec(mf11)/sqrt(pos2)*gp*gc+0.5*gp*gp*eigenvec(m2f02))/sqrt(0.5*pow(gc,4)+gp*gp*gc*gc+0.25*pow(gp,4)/sqrt(pos2)));
		}

		trans=1.0/(1.0+ (gp*gp/k1/r) / (1.0+gc*gc/k2/r));
	}
	void consoleOut(){
		cout << eigenval << endl;
		cout << "max eigen value	:" << maxreal << endl;
		cout << "second eigen value	:" << secreal << endl;
		cout << "coherence time		:" << coherence << "us" <<endl;
		cout << "g00	:" << eigenvecpos(mg00) << endl;
		cout << "g10	:" << eigenvecpos(mg10) << endl;
		cout << "e00	:" << eigenvecpos(me00) << endl;
		cout << "f01	:" << eigenvecpos(mf01) << endl;
		cout << "g20	:" << eigenvecpos(mg20) << endl;
		cout << "e10	:" << eigenvecpos(me10) << endl;
		cout << "f11	:" << eigenvecpos(mf11) << endl;
		cout << "ef01	:" << eigenvecpos(mef01) << endl;
		cout << "2f02	:" << eigenvecpos(m2f02) << endl;
		cout << "2e00	:" << eigenvecpos(m2e00) << endl;
		cout << "sum	:" << eigenvecpos.sum() << endl;
		cout << "np1	:" << np1 << endl;
		cout << "nc1	:" << nc1 << endl;
		cout << "ne1	:" << ne1 << endl;
		cout << "np2	:" << np2 << endl;
		cout << "nc2	:" << nc2 << endl;
		cout << "ne2	:" << ne2 << endl;
		cout << "g2p	:" << g2p << endl;
		cout << "g2c	:" << g2c << endl;
		cout << "g2e	:" << g2e << endl;
	}
};

int main(){
	int n=10;
	double eps=1e-5;
	double k1=1.0/2;
	double k2=0.03/2;
	double r=6.0/2;
	double gp=0.1;
	double gc=1;
/*	double k1=1000.0/2;
	double k2=1.0/2;
	double r=3.0/2;
	double gp=5;
	double gc=10;*/
	int N=1000;

	CalcDiagonal* cd = new CalcDiagonal();
	cd->eps=eps;
	cd->k1=k1;
	cd->k2=k2;
	cd->r=r;
	cd->gp=gp*sqrt(N);
	cd->gc=gc;


	fstream fout,fout2,fout3,fout4;
	vector<fstream> fsv;
	fout.open("gpout.txt",ios::out);
	for(int i=0;i<1000;i++){
		cd->gp=i*0.001*sqrt(1000);
		cd->init();
		cd->compute();
		fout << i*0.001 << " " << cd->g2p << " " << cd->coherence << endl;
	}
	fout.close();cd->gp=gp*sqrt(N);

	fout.open("gcout.txt",ios::out);
	for(int i=0;i<1000;i++){
		cd->gc=i*0.01;
		cd->init();
		cd->compute();
		fout << i*0.01 << " " << cd->g2p << " " << cd->coherence << endl;
	}
	fout.close();cd->gc=gc;


	fout.open("kpout.txt",ios::out);
	for(int i=0;i<1000;i++){
		cd->k1=i*0.1/2;
		cd->init();
		cd->compute();
		fout << i*0.1 << " " << cd->g2p << " " << cd->coherence << endl;
	}
	fout.close();cd->k1=k1;

	fout.open("kcout.txt",ios::out);
	for(int i=0;i<1000;i++){
		cd->k2=i*0.1/2;
		cd->init();
		cd->compute();
		fout << i*0.1 << " " << cd->g2p << " " << cd->coherence << endl;
	}
	fout.close();cd->k2=k2;


	fsv.resize(12);
	fsv[0].open("gpgcg2p.txt",ios::out);
	fsv[1].open("gpgccoh.txt",ios::out);
	fsv[2].open("gpgctrans.txt",ios::out);
	fsv[3].open("gpgcg2c.txt",ios::out);
	fsv[4].open("gpgcg2pos.txt",ios::out);
	fsv[5].open("gpgcdark1.txt",ios::out);
	fsv[6].open("gpgcdark2.txt",ios::out);
	fsv[7].open("gpgcdarkr.txt",ios::out);
	fsv[8].open("gpgcpos1.txt",ios::out);
	fsv[9].open("gpgcpos2.txt",ios::out);
	fsv[10].open("gpgcg2dark.txt",ios::out);
	fsv[11].open("gpgcg2e.txt",ios::out);
	for(int i=1;i<100;i++){
		for(int j=1;j<100;j++){
			if(i==-1){
//				for(unsigned int k=0;k<fsv.size();k++)fsv[k] << j*(2.0/100) << " ";
			}else if(j==-1){
//				for(unsigned int k=0;k<fsv.size();k++)fsv[k] << i*(10.0/sqrt(N)/100)*sqrt(N) << " ";
			}else{
				cd->gp=i*(5.0/sqrt(N)/100)*sqrt(N);
				cd->gc=j*(2.0/100);
				cd->init();
				cd->compute();
				fsv[0] << cd->g2p << " " ;
				fsv[1] << cd->coherence << " ";
				fsv[2] << cd->trans << " ";
				fsv[3] << cd->g2c << " ";
				fsv[4] << 2*cd->pos2 / (pow(cd->pos1,2)) << " ";
				fsv[5] << cd->dark1 << " ";
				fsv[6] << cd->dark2 << " ";
				fsv[7] << cd->dark2/cd->dark1 << " ";
				fsv[8] << cd->pos1 << " ";
				fsv[9] << cd->pos2 << " ";
				fsv[10] << 1 + (0.5*pow(cd->gp,4)/(pow(cd->gc,4)+2*cd->gp*cd->gp*cd->gc*cd->gc+0.5*pow(cd->gp,4))) << " ";
				fsv[11] << cd->g2e << " ";
			}
		}
		for(unsigned int k=0;k<fsv.size();k++)fsv[k] << endl;
		cout << i << endl;
	}
	for(unsigned int k=0;k<fsv.size();k++)fsv[k].close();
	cd->gc=gc;cd->gp=gp*sqrt(N);

	fsv.resize(8);
	fsv[0].open("kpkcg2p.txt",ios::out);
	fsv[1].open("kpkccoh.txt",ios::out);
	fsv[2].open("kpkctrans.txt",ios::out);
	fsv[3].open("kpkcg2c.txt",ios::out);
	fsv[4].open("kpkcg2pos.txt",ios::out);
	fsv[5].open("kpkcdark1.txt",ios::out);
	fsv[6].open("kpkcdark2.txt",ios::out);
	fsv[7].open("kpkcdarkr.txt",ios::out);
	for(int i=-1;i<100;i++){
		for(int j=-1;j<100;j++){
			if(i==-1){
//				for(unsigned int k=0;k<fsv.size();k++)fsv[k] << j*0.001 << " ";
			}else if(j==-1){
//				for(unsigned int k=0;k<fsv.size();k++)fsv[k] << i*0.01 << " ";
			}else{
				cd->k1=i*0.01/2;
				cd->k2=j*0.001/2;
				cd->init();
				cd->compute();
				fsv[0] << cd->g2p << " " ;
				fsv[1] << cd->coherence << " ";
				fsv[2] << cd->trans << " ";
				fsv[3] << cd->g2c << " ";
				fsv[4] << cd->pos2 / (pow(cd->pos1,2)/2) << " ";
				fsv[5] << cd->dark1 << " ";
				fsv[6] << cd->dark2 << " ";
				fsv[7] << cd->dark2/cd->dark1 << " ";
			}
		}
		for(unsigned int k=0;k<fsv.size();k++)fsv[k] << endl;
		cout << i << endl;
	}
	for(unsigned int k=0;k<fsv.size();k++)fsv[k].close();
	cd->k1=k1;cd->k2=k2;

	delete cd;
	return 0;
}