
#include <iostream>
#include <fstream>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <vector>

using namespace std;

class EigenSet{
	complex<double> val;
	Eigen::VectorXcd vec;
};

int main(){
	double k1=1.0;
	double k2=0.03;
	double r=6;
	double gp=0.1*sqrt(1000);
	double gc=1.0;
	fstream fs("out.txt",ios::out);
	fs.close();

/*	for(gc=0.0;gc<3;gc+=0.01){
		Eigen::MatrixXcd mat1;
		mat1 = Eigen::MatrixXcd(3,3);
		for(int i=0;i<3;i++)for(int j=0;j<3;j++)mat1(i,j)=0;
		mat1(0,0)=-k1/2;
		mat1(1,1)=-r/2;
		mat1(2,2)=-k2/2;
		mat1(0,1)=mat1(1,0)=gp*complex<double>(0,1);
		mat1(2,1)=mat1(1,2)=gc*complex<double>(0,1);

		Eigen::ComplexEigenSolver<Eigen::MatrixXcd> es(mat1);
		auto eigenvec = es.eigenvectors();
		auto eigenval = es.eigenvalues();

		fstream fs("out.txt",ios::app);
		fs << gc;
		for(int i=0;i<3;i++){
			fs << " "<<eigenval[i].real();
//			cout << eigenval[i] << endl;
//			cout << eigenvec.col(i) << endl << endl;
		}
		fs << endl;
	}
	*/
	for(gc=0.0;gc<3;gc+=0.01){
		Eigen::MatrixXcd mat1;
		mat1 = Eigen::MatrixXcd(6,6);
		for(int i=0;i<6;i++)for(int j=0;j<6;j++)mat1(i,j)=0;
		mat1(0,0)=-k1;
		mat1(1,1)=-r/2-k1/2;
		mat1(2,2)=-k2/2-k1/2;
		mat1(3,3)=-k2/2-r/2;
		mat1(4,4)=-k2;
		mat1(5,5)=-r;
		mat1(0,1)=mat1(1,0)=sqrt(2)*gp*complex<double>(0,1);
		mat1(2,1)=mat1(1,2)=gc*complex<double>(0,1);
		mat1(2,3)=mat1(3,2)=gp*complex<double>(0,1);
		mat1(4,3)=mat1(3,4)=2*gc*complex<double>(0,1);
		mat1(1,5)=mat1(5,1)=sqrt(2)*gp*complex<double>(0,1);
		mat1(3,5)=mat1(5,3)=sqrt(2)*gc*complex<double>(0,1);

		Eigen::ComplexEigenSolver<Eigen::MatrixXcd> es(mat1);
		auto eigenvec = es.eigenvectors();
		vector<Eigen::VectorXcd> vec;
		for(int i=0;i<6;i++){
			vec.push_back(eigenvec.col(i));
		}
		auto eigenval = es.eigenvalues();

		for(int i=0;i<6;i++){
			for(int j=0;j<6-i-1;j++){
				if(eigenval[j].real()<eigenval[j+1].real()){
					auto temp = eigenval[j+1];
					auto tempv = vec[j+1];
					eigenval[j+1]=eigenval[j];
					vec[j+1]=vec[j];
					eigenval[j]=temp;
					vec[j]=tempv;
				}
			}
		}

		fstream fs("out.txt",ios::app);
		fs << gc;
		for(int i=0;i<6;i++){
			fs << " "<<eigenval[i].real();
//			cout << eigenval[i] << endl;
//			cout << eigenvec.col(i) << endl << endl;
		}
		fs << endl;
	}

	return 0;
}