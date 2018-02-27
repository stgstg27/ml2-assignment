#include <bits/stdc++.h>
#include "Eigen/Dense"
#include <string.h>

#define float  double
using namespace Eigen;
using namespace std;

struct params{
	Eigen :: RowVectorXd w;
	float w0;
};

struct data_structure{
	Eigen::MatrixXd features;
	Eigen :: RowVectorXd target_class;

};


data_structure read_data(string file_location)
{
	ifstream trainFile;
	trainFile.open(file_location.c_str());
	vector<vector<float> > train_data;
	vector<int> target_class;
	if(trainFile.is_open())
	{
		while(!trainFile.eof())
		{
			string s;
			vector<float> temp;
			getline(trainFile, s);
			int comma_no = 0;
			int prev = 0;
			for(int i=0;i<s.length(); i++)
			{
				if(s[i]==',')
				{
					string temp_float = s.substr(prev,i-prev);
					prev = i+1;
					temp.push_back(stof(temp_float, NULL));
				}
			}
			string ss = s.substr(prev,s.length()-prev);
			int c = stoi(ss);
			target_class.push_back(c);
			train_data.push_back(temp);
		}
	}

	data_structure d;

	Eigen::MatrixXd train_data_eig(train_data.size(),train_data[0].size());
	for(int i=0;i<train_data.size();i++)
        for(int j=0;j<train_data[0].size();j++)
            train_data_eig(i,j) = train_data[i][j];

    Eigen :: RowVectorXd target_class_eig(target_class.size());
    for(int j=0;j<target_class.size();j++)
            target_class_eig(j) = target_class[j];

	d.features = train_data_eig;
	d.target_class = target_class_eig;
	return d;
}

float accuracy(vector<int> predict,Eigen::RowVectorXd actual)
{
    float count = 0;
    for(int i=0;i<predict.size();i++)
        if(predict[i]== (int) actual(i))
            count++;
    return 100*(count/predict.size());
}

float sigmoid(params p, Eigen::MatrixXd x)
{
    Eigen::MatrixXd m= x*p.w.transpose();
    float z =  p.w0+m(0,0);
    return 1/(exp(-z)+1);
}

int prediction(Eigen::RowVectorXd x,params p)
{
    float val = sigmoid(p,x);
    return val>=0.5?1:0;
}


params train(data_structure data, int MAX_STEPS, float LEARNING_RATE)
{
    params p;
    Eigen::RowVectorXd rr(data.features.cols());
    //rr<<0.5, 0.2, 0.3, 0.4;
    rr<<((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX));

    p.w = rr;

    p.w0=((double) rand() / (RAND_MAX));

    float cost;
    Eigen::RowVectorXd dw(data.features.cols());
    float dw0;

    for(int step=0; step<MAX_STEPS; step++)
    {
        cost=0;
        for(int j=0; j<data.features.cols(); j++)
            dw(j)=0;
        dw0=0;
        for(int i=0; i<data.features.rows(); i++)
        {
            float y = sigmoid(p, data.features.row(i));
            int t = data.target_class[i];
            cost -= (t*log(max(0.001,y))) + (1-t)*log(max(0.001,1-y));
            for(int j=0; j<data.features.cols(); j++)
                dw(j)+=(y-t)*data.features(i,j);
            dw0+=(y-t);
        }
        if(step%100==0)
            cout<<"step:"<<step<<" cost:"<<cost<<" dw1: "<<dw(1)<<endl;
        for(int j=0; j<data.features.cols(); j++)
            {
                p.w(j)-=LEARNING_RATE*dw(j);
                p.w0-=LEARNING_RATE*dw0;
            }
    }
    return p;
}

int main()
{

	data_structure train_data;
	train_data = read_data("data/train.txt");
	params p = train(train_data,3200,0.001);
	data_structure test_data;
	test_data = read_data("data/test.txt");

	vector<int> predictions;
	for(int i = 0;i<test_data.features.rows();i++)
        predictions.push_back(prediction(test_data.features.row(i),p));
    cout<<"Accuracy : "<<accuracy(predictions,test_data.target_class)<<" %"<<endl;

	return 0;
}
