#include "kalman_filter.h"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using namespace std;


KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
	x_ = F_ * x_;
	MatrixXd Ft = F_.transpose();
	P_ = F_ * P_ * Ft + Q_;
	
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
	VectorXd z_pred = H_ * x_;
	VectorXd y = z - z_pred;
	MatrixXd Ht = H_.transpose();
	MatrixXd S = (H_ * P_ * Ht) + R_;
	MatrixXd Si = S.inverse();
	MatrixXd K = P_ * Ht * Si;
	
	// new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;

}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */
	double px = x_(0);
	double py = x_(1);
	double vx = x_(2);
	double vy = x_(3);

	double epsilon = 0.0001;
	if (fabs(px) < epsilon && fabs(py) < epsilon)
	{
		px = epsilon;
		py = epsilon;
	}
	VectorXd hx(3, 1);
	double firstElement = sqrt(pow(px, 2) + pow(py, 2));
	double secondElement = atan2(py, px);
	double thirdElement = ((px * vx) + (py * vy)) / (sqrt(pow(px, 2) + pow(py, 2)));
	hx << firstElement, secondElement, thirdElement;

	VectorXd y = z - hx;
	y[1] = atan2(sin(y[1]), cos(y[1]));//normalize the angle
	MatrixXd Ht = H_.transpose();
	MatrixXd S = (H_ * P_ * Ht) + R_;
	MatrixXd Si = S.inverse();
	MatrixXd K = P_ * Ht * Si;

	// new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;
}
