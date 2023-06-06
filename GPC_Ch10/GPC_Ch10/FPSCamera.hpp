//
//  FPSCamera.hpp
//  GPC_Ch10
//
//  Created by Inoue Shinichi on 2023/05/30.
//

#ifndef FPSCamera_hpp
#define FPSCamera_hpp

#include "CameraComponent.hpp"

class FPSCamera : public CameraComponent
{
public:
    FPSCamera(class Actor* owner);
    
    void Update(float deltaTime) override;
    
    float GetPitch() const { return mPitch; }
    float GetPitchSpeed() const { return mPitchSpeed; }
    float GetMaxPitch() const { return mMaxPitch; }
    
    void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
    void SetMaxPitch(float pitch) { mMaxPitch = pitch; }
private:
    // Rotation/sec speed of pitch
    float mPitchSpeed;
    // Maximum pitch deviation from forward
    float mMaxPitch;
    // Current pitch
    float mPitch;
};

#endif /* FPSCamera_hpp */
