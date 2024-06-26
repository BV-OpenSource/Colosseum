// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef air_FixedWingRpcLibAdaptors_hpp
#define air_FixedWingRpcLibAdaptors_hpp

#include "common/Common.hpp"
#include "common/CommonStructs.hpp"
#include "api/RpcLibAdaptorsBase.hpp"
#include "vehicles/fixedwing/api/FixedWingCommon.hpp"
#include "vehicles/fixedwing/api/FixedWingApiBase.hpp"
#include "common/ImageCaptureBase.hpp"
#include "safety/SafetyEval.hpp"

#include "common/common_utils/WindowsApisCommonPre.hpp"
#include "rpc/msgpack.hpp"
#include "common/common_utils/WindowsApisCommonPost.hpp"

namespace msr { namespace airlib_rpclib {
    //Left in but this shouldn't be used for a fixedwing aircraft. 
class FixedWingRpcLibAdaptors : public RpcLibAdaptorsBase {
public:
    struct YawMode {
        bool is_rate = true;
        float yaw_or_rate = 0;
        MSGPACK_DEFINE_MAP(is_rate, yaw_or_rate);
    	
        YawMode()
        {}

        YawMode(const msr::airlib::YawMode& s)
        {
            is_rate = s.is_rate;
            yaw_or_rate = s.yaw_or_rate;
        }
        msr::airlib::YawMode to() const
        {
            return msr::airlib::YawMode(is_rate, yaw_or_rate);
        }
    };

    struct FixedWingState {
        CollisionInfo collision;
        KinematicsState kinematics_estimated;
        KinematicsState kinematics_true;
        GeoPoint gps_location;
        uint64_t timestamp;
        LandedState landed_state;
        RCData rc_data;
        bool ready;
        std::string ready_message;
        std::vector<std::string> controller_messages;
        bool can_arm;

        MSGPACK_DEFINE_MAP(collision, kinematics_estimated, gps_location, timestamp, landed_state, rc_data);

        FixedWingState()
        {}

        FixedWingState(const msr::airlib::FixedWingState& s)
        {
            collision = s.collision;
            kinematics_estimated = s.kinematics_estimated;
            gps_location = s.gps_location;
            timestamp = s.timestamp;
            landed_state = s.landed_state;
            rc_data = RCData(s.rc_data);
            ready = s.ready;
            ready_message = s.ready_message;
            can_arm = s.can_arm;
        }

        msr::airlib::FixedWingState to() const
        {
            return msr::airlib::FixedWingState(collision.to(), kinematics_estimated.to(),
                gps_location.to(), timestamp, landed_state, rc_data.to(), ready, ready_message, can_arm);
        }
    };
};

}} //namespace

MSGPACK_ADD_ENUM(msr::airlib::DrivetrainType);
MSGPACK_ADD_ENUM(msr::airlib::LandedState);


#endif
