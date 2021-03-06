
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>
#include <iostream>
#include <complex>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <conio.h>

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
	double a0;
	double a0d;
	double b0;
	double b0d;
	double g2p;
	double maxreal;

	Eigen::VectorXcd eigenvec;
	Eigen::VectorXd eigenvecpos;
	Eigen::VectorXcd eigenval;
	Eigen::VectorXcd evec;
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
		cout << eigenval(maxind) << endl;
		eigenvec = es.eigenvectors().col(maxind);

		eigenvecpos = Eigen::VectorXd(n);
		for(int i=0;i<n;i++){
			eigenvecpos(i) = norm(eigenvec(i));
		}
		Eigen::VectorXcd eigenvecsub = Eigen::VectorXcd(n);
		for(int i=0;i<n;i++){
			eigenvecsub(i) = eigenvec(i);
		}
		eigenvecsub[mg00]*=0;
		eigenvecsub[mg20]*=2;
		eigenvecsub[me10]*=2;
		eigenvecsub[mf11]*=2;
		eigenvecsub[mef01]*=2;
		eigenvecsub[m2f02]*=2;
		eigenvecsub[m2e00]*=2;

		g2p=2*eigenvecpos(mg20)/pow(eigenvecpos(mg10),2);

		Eigen::FullPivLU<Eigen::MatrixXcd> lu(mat);
		evec = lu.solve(eigenvecsub);
//		cout << eigenvec << endl;
//		cout << evec << endl;
//		cout << eigenvecsub << endl;
//		cout << mat*evec << endl;
//		_getch();
//
//		Eigen::VectorXcd evecd = mat*evec;

//		a0 = abs(eigenvec[mg10]);
//		a0d = abs(evec[mg10]);
//		b0 = abs(eigenvec[mg20]);
//		b0d = abs(evec[mg20]);

//		cout << eigenvec << endl;
//		cout << evecd << endl;
//		_getch();
	}
};

int main(){
	int n=10;
	double eps=1e-4;
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
/*
	double sigma2=25*25;

	fstream fs("test.txt",ios::out);
	for(double t=-50;t<50;t+=0.1){
		double delta = -t/sigma2;
		double gauss =1e-3*exp(-t*t/(2*sigma2))/sqrt(2*M_PI*sigma2);

		cd->eps=gauss;
		cd->init();
		cd->compute();

		double dds = pow(norm(cd->eigenvec[mg10]),2);
		double ddu = 2*norm(cd->eigenvec[mg20]);
		double g2s = ddu/dds;

		Eigen::VectorXcd vec = cd->eigenvec + delta*cd->evec;

		double nu=2*norm(vec[mg20]);
		double nd=pow(norm(vec[mg10]),2);
		double g2d = nu/nd;

		complex<double> b0=cd->eigenvec[mg20];
		complex<double> b0d=cd->evec[mg20];

//		double ns = pow(norm(cd->eigenvec[mg10]-delta*cd->evec[mg10]),2);
//		double nu = 2*norm(cd->eigenvec[mg20]-delta*cd->evec[mg20]);
//		double g2d = nu/ns;
		cout << t << " " << g2s << " " << g2d << endl;
		fs << t << " " << g2s << " " << g2d << " " << nu << " " << nd << " " << abs(b0) << " " << abs(b0d) << " " << g2s*(1+2*delta*abs(b0d/b0)/2/M_PI) << endl;
//		cout << vec << endl;
//		_getch();
	}
	fs.close();
*/
//	cd->init();
//	cd->compute();


	vector<fstream> fsv;
	fsv.resize(7);
	fsv[0].open("gpgca0.txt",ios::out);
	fsv[1].open("gpgca0d.txt",ios::out);
	fsv[2].open("gpgcb0.txt",ios::out);
	fsv[3].open("gpgcb0d.txt",ios::out);
	fsv[4].open("gpgca0da0.txt",ios::out);
	fsv[5].open("gpgcb0db0.txt",ios::out);
	fsv[6].open("gpgcdif.txt",ios::out);
	for(int i=-1;i<100;i++){
		for(int j=-1;j<100;j++){
			if(i==-1){
//				for(unsigned int k=0;k<fsv.size();k++)fsv[k] << j*(2.0/100) << " ";
			}else if(j==-1){
//				for(unsigned int k=0;k<fsv.size();k++)fsv[k] << i*(10.0/sqrt(N)/100)*sqrt(N) << " ";
			}else{
				cd->gp=i*(10.0/sqrt(N)/100)*sqrt(N);
				cd->gc=j*(2.0/100);
				cd->init();
				cd->compute();
//				fsv[0] << cd->a0 << " " ;
//				fsv[1] << cd->a0d << " " ;
//				fsv[2] << cd->b0 << " " ;
//				fsv[3] << cd->b0d << " " ;
//				fsv[4] << cd->a0d/cd->a0 << " " ;
//				fsv[5] << cd->b0d/cd->b0 << " " ;
//				fsv[6] << cd->b0d/cd->b0 - 2 * cd->a0d/cd->a0 << " " ;
				fsv[2] << cd->eigenvec[mg20].real() << " " ;
				fsv[3] << cd->evec[mg20].real() << " " ;
				fsv[6] << cd->evec[mg20].imag() / cd->eigenvec[mg20].real() << " " ;
			}
		}
		for(unsigned int k=0;k<fsv.size();k++)fsv[k] << endl;
		cout << i << endl;
	}
	for(unsigned int k=0;k<fsv.size();k++)fsv[k].close();

	delete cd;
	return 0;
}