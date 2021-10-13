#include "vrpn_Tracker_OpenVR.h"
#include <openvr.h>
#include <quat.h>
#include <iostream>

vrpn_Tracker_OpenVR::vrpn_Tracker_OpenVR(const std::string& name, vrpn_Connection* connection, vr::IVRSystem * vr) :
	vrpn_Tracker(name.c_str(), connection), name(name), vr(vr), tracker_line_info(name)
{
	// Initialize the vrpn_Tracker
    // We track each device separately so this will only ever have one sensor
	vrpn_Tracker::num_sensors = 1;
}

void vrpn_Tracker_OpenVR::updateTracking(vr::TrackedDevicePose_t *pose) {
	if (!pose->bPoseIsValid) {
		return;
	}

	tracker_line_info.update_status_and_print(pose->eTrackingResult);

	// Sensor, doesn't change since we are tracking individual devices
	d_sensor = 0;

	// Position OLD
	//pos[0] = pose->mDeviceToAbsoluteTracking.m[0][3];
	//pos[1] = pose->mDeviceToAbsoluteTracking.m[1][3];
	//pos[2] = pose->mDeviceToAbsoluteTracking.m[2][3];

	// Position NEW: UE4 uses different axis so we need to convert
	pos[0] = -(pose->mDeviceToAbsoluteTracking.m[2][3]);
	pos[1] = pose->mDeviceToAbsoluteTracking.m[0][3];
	pos[2] = pose->mDeviceToAbsoluteTracking.m[1][3];

	// Quaternion
	ConvertSteamVRMatrixToQMatrix(pose->mDeviceToAbsoluteTracking, matrix);
	q_from_col_matrix(d_quat, matrix);


	// Rotation adjustment: UE4 uses different axis so we need to convert
	vrpn_float64 temp = -d_quat[2];
	d_quat[2] = d_quat[1];
	d_quat[1] = temp;

	temp = d_quat[0];
	d_quat[0] = d_quat[1];
	d_quat[1] = temp;


    // Pack message
	vrpn_gettimeofday(&timestamp, NULL);
	char msgbuf[1000];
	vrpn_int32 len = vrpn_Tracker::encode_to(msgbuf);
	if (d_connection->pack_message(len, timestamp, position_m_id, d_sender_id, msgbuf, vrpn_CONNECTION_LOW_LATENCY)) {
		std::cerr << "[\" << name << \"] Can't write message" << std::endl;
	}
}

void vrpn_Tracker_OpenVR::mainloop() {
    vrpn_gettimeofday( &(vrpn_Tracker_OpenVR::timestamp), NULL );
	vrpn_Tracker::server_mainloop();
}

void vrpn_Tracker_OpenVR::ConvertSteamVRMatrixToQMatrix(const vr::HmdMatrix34_t &matPose, q_matrix_type &matrix) {
	matrix[0][0] = matPose.m[0][0];
	matrix[1][0] = matPose.m[1][0];
	matrix[2][0] = matPose.m[2][0];
	matrix[3][0] = 0.0;
	matrix[0][1] = matPose.m[0][1];
	matrix[1][1] = matPose.m[1][1];
	matrix[2][1] = matPose.m[2][1];
	matrix[3][1] = 0.0;
	matrix[0][2] = matPose.m[0][2];
	matrix[1][2] = matPose.m[1][2];
	matrix[2][2] = matPose.m[2][2];
	matrix[3][2] = 0.0;
	matrix[0][3] = matPose.m[0][3];
	matrix[1][3] = matPose.m[1][3];
	matrix[2][3] = matPose.m[2][3];
	matrix[3][3] = 1.0f;
}