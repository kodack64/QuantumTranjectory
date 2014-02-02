
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

const double N = 1;
double gp=10;
double gc=0.2;
double kp=1.0;
double kc=0.03;
const double gamma=3;

double etap(){
	return gp*gp*4/kp/gamma;
}
double etac(){
	return gc*gc*4/kc/gamma;
}
double alpha(){
	return pow(gc/(sqrt(N)*gp),2);
}

double calcNpN(int n,int k){
	double val = pow(alpha(),k);
	for(int i=1;i<=n;i++){
		val*=i;
	}
	for(int i=1;i<=k;i++){
		val/=i*i;
	}
	for(int i=1;i<=n-k;i++){
		val/=i;
	}
	return val;
}

double calcNp(int n){
	if(n==0)return 0;
	double sumu=0;
	double sumd=0;
	for(int i=0;i<=n;i++){
		double val=calcNpN(n,i);
		sumd+=val;
		sumu+=i*val;
	}
	return sumu/sumd;
}
double trans(double np,double nc){
	return 1.0/(1+N*(np+1)*etap()/(1+(nc+1)*etac()));
}

double g2(){
	double a = alpha();
	return 1 + 1/(2*a*a+4*a+1);
}
double TR0(){
	return trans(0,0);
}
double TR1(){
	double np1 = calcNp(1);
	double nc1 = 1-np1;
	return trans(np1,nc1);
	double tr = trans(0,0);
	double npd1 = np1*tr*tr;
	double ncd1 = nc1*tr*tr;
	return trans(npd1,ncd1);
}
double TR2(){
	double np2 = calcNp(2);
	double nc2 = 2-np2;
	return trans(np2,nc2);
}
double TR(){
	return TR1()/TR2();
}

int main(){
	ofstream ofs("npnc.txt",ios::out);
	for(int i=0;i<20;i++){
		double np = calcNp(i);
		double nc = i-np;
		ofs << i << " " << (i==0?0:np/i) << " " << (i==0?0:nc/i) << " " << trans(np,nc) << endl;
		cout << i << " " << (i==0?0:np/i) << " " << (i==0?0:nc/i) << " " << trans(np,nc) << endl;
	}
	ofs.close();

	ofs.open("g2t.txt",ios::out);
	for(gp=0.001;gp<10;gp+=0.1){
		for(gc=0.001;gc<2.0;gc+=0.1){
//			ofs << trans(calcNp(1),2-calcNp(1)) << " ";
//			ofs << TR0() << " ";
			ofs << pow(TR1()/TR0(),2) << " ";
		}
		ofs << endl;
	}
	ofs.close();

	kp=1;
	kc=0.03;
	gc=1.0;
	ofs.open("rel1.txt",ios::out);
	for(gp=0.01*sqrt(1000);gp<0.15*sqrt(1000);gp+=0.001){
//		ofs << gp/sqrt(1000) << " " << pow(TR1()/TR0(),4) << endl;
		ofs << gp/sqrt(1000) << " " << pow(TR0(),2) << endl;
	}
	ofs.close();

	gp=0.1*sqrt(1000);
	ofs.open("rel2.txt",ios::out);
	for(gc=0.01;gc<2.0;gc+=0.001){
//		ofs << gc << " " << pow(TR1()/TR0(),4) << endl;
		ofs << gc << " " << pow(TR0(),2) << endl;
	}
	ofs.close();

	gc=1.0;
	ofs.open("rel3.txt",ios::out);
	for(kp=0.2;kp<1.8;kp+=0.001){
//		ofs << kp << " " << pow(TR1()/TR0(),4) << endl;
		ofs << kp << " " << pow(TR0(),2) << endl;
	}
	ofs.close();

	kp=1.0;
	ofs.open("rel4.txt",ios::out);
	for(kc=0.01;kc<0.1;kc+=0.001){
//		ofs << kc << " " << pow(TR1()/TR0(),4) << endl;
		ofs << kc << " " << pow(TR0(),2) << endl;
	}
	ofs.close();

	return 0;
}