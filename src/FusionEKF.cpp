#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  H_laser_ << 1, 0, 0, 0,
			0, 1, 0, 0;
  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
        0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
        0, 0.0009, 0,
        0, 0, 0.09;

  /**
  TODO:
    * Finish initializing the FusionEKF.
    * Set the process and measurement noises
  */
  // initialize the process uncertainty
  ekf_.P_ = MatrixXd(4, 4);
  ekf_.P_ << 1, 0, 0, 0,
	  0, 1, 0, 0,
	  0, 0, 1000, 0,
	  0, 0, 0, 1000;

  //set Q- process noise
  ekf_.Q_ = MatrixXd(4, 4);

  //set state transistion matrix
  ekf_.F_ = MatrixXd(4, 4);
  ekf_.F_ << 1, 0, 1, 0,
			 0, 1, 0, 1,
			 0, 0, 1, 0,
			 0, 0, 0, 1;
  //define noise noise variation
  double noise_ax = 9;
  double noise_ay = 9;
}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {


  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    /**
    TODO:
      * Initialize the state ekf_.x_ with the first measurement.
      * Create the covariance matrix.
      * Remember: you'll need to convert radar from polar to cartesian coordinates.
    */
    // first measurement
    ekf_.x_ = VectorXd(4);
	//ekf_.x_ << 1, 1, 1, 1;


    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      /**
      Convert radar from polar to cartesian coordinates and initialize state.
      */
		double rho = measurement_pack.raw_measurements_[0];
		double phi = measurement_pack.raw_measurements_[1];

		double px = rho * cos(phi);
		double py = rho * sin(phi);
		
		ekf_.x_ << px, py, 0, 0;

    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
      Initialize state.
      */
		ekf_.x_ << measurement_pack.raw_measurements_[0], measurement_pack.raw_measurements_[1], 0, 0;

		//cout << "Init_x_Laser: " << endl;
    }
	previous_timestamp_ = measurement_pack.timestamp_;
    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  /**
   TODO:
     * Update the state transition matrix F according to the new elapsed time.
      - Time is measured in seconds.
     * Update the process noise covariance matrix.
     * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
   //compute time elapsed
  double dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
  cout << "dt : " << dt << endl;
  previous_timestamp_ = measurement_pack.timestamp_;

  // modify F matrix based on elapsed time
  ekf_.F_(0, 2) = dt;
  ekf_.F_(1, 3) = dt;

  //2. Set the process covariance matrix Q
  double dt2 = dt * dt;
  double dt3 = (dt * dt * dt) / 2;
  double dt4 = (dt * dt * dt * dt) / 4;
  ekf_.Q_ << dt4 * noise_ax, 0, dt3 * noise_ax, 0,
	  0, dt4 * noise_ay, 0, dt3 * noise_ay,
	  dt3 * noise_ax, 0, dt2 * noise_ax, 0,
	  0, dt3 * noise_ay, 0, dt2 * noise_ay;


  ekf_.Predict();

  /*cout << "Q : " << ekf_.Q_;
  cout << "After predict:" << endl;
  cout << "x : " << ekf_.x_ << endl;
  cout << "F : " << ekf_.F_ << endl;*/

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  /**
   TODO:
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
   */

  //if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
  //  // Radar updates
	 // ekf_.H_ = tools.CalculateJacobian(ekf_.x_);
	 // ekf_.R_ = R_radar_;
	 //// For radar, specify Hj and R_radar
	 //ekf_.UpdateEKF(measurement_pack.raw_measurements_);
	 ///*cout << "Radar" << endl;
	 //cout << "After Update- x :" << ekf_.x_ << endl;
	 //cout << "After Update- P :" << ekf_.P_ << endl;
	 //cout << "After Update- Hj :" << ekf_.H_ << endl;*/

	 // 
  //} 
 /* else {*/
    // Laser updates
  if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
	//For laser specify H_laser and R_laser
	  ekf_.H_ = H_laser_;
	  ekf_.R_ = R_laser_;

	  ekf_.Update(measurement_pack.raw_measurements_);
	  /*cout << "Lidar" << endl;
	  cout << "After Update- x :" << ekf_.x_ << endl;
	  cout << "After Update- P :" << ekf_.P_ << endl;
	  cout << "After Update- H :" << ekf_.H_ << endl;*/
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
