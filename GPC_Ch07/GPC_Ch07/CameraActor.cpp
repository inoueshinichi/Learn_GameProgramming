//
//  CameraActor.cpp
//  GPC_Ch06
//
//  Created by Inoue Shinichi on 2022/07/20.
//

#include "CameraActor.hpp"
#include "MoveComponent.hpp"
#include "SDL_scancode.h"
#include "Renderer.hpp"
#include "Game.hpp"
#include "AudioSystem.hpp"
#include "AudioComponent.hpp"

CameraActor::CameraActor(Game* game)
    : Actor(game)
{
    mMoveComp = new MoveComponent(this);
    mAudioComp = new AudioComponent(this);
    mLastFootstep = 0.0f;
    mFootstep = mAudioComp->PlayEvent("event:/Footstep");
    mFootstep.SetPaused(true);
    
}

void CameraActor::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);
    
    // Play the footstep if we're moving and haven't recently
    mLastFootstep -= deltaTime;
    if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f)
    {
        mFootstep.SetPaused(false);
        mFootstep.Restart();
        mLastFootstep = 0.5f;
    }
    
    // Compute new camera from this actor
    Vector3 cameraPos = GetPosition();
    Vector3 target = GetPosition() + GetForward() * 100.0f;
    Vector3 up = Vector3::UnitZ;
    
    Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
    GetGame()->GetRenderer()->SetViewMatrix(view);
}

void CameraActor::ActorInput(const uint8_t* keys)
{
    float forwardSpeed = 0.0f;
    float angularSpeed = 0.0f;
    
    // wasd movement
    if (keys[SDL_SCANCODE_W])
    {
        forwardSpeed += 300.0f;
    }
    if (keys[SDL_SCANCODE_S])
    {
        forwardSpeed -= 300.0f;
    }
    if (keys[SDL_SCANCODE_A])
    {
        angularSpeed -= Math::TwoPi;
    }
    if (keys[SDL_SCANCODE_D])
    {
        angularSpeed += Math::TwoPi;
    }
    
    mMoveComp->SetForwardSpeed(forwardSpeed);
    mMoveComp->SetAngularSpeed(angularSpeed);
}

void CameraActor::SetFootstepSurface(float value)
{
    // Pause here because the way I setup the parameter in FMOD
    // changing it will play a footstep
    mFootstep.SetPaused(true);
    mFootstep.SetParameter("Surface", value);
}
