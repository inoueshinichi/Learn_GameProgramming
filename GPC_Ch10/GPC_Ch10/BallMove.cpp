//
//  BallMove.cpp
//  GPC_Ch10
//
//  Created by Inoue Shinichi on 2023/05/30.
//

#include "BallMove.hpp"
#include "Actor.hpp"
#include "Game.hpp"
#include "PhysWorld.hpp"
#include "TargetActor.hpp"
#include "BallActor.hpp"

BallMove::BallMove(Actor* owner)
    : MoveComponent(owner)
{}

void BallMove::Update(float deltaTime)
{
    // Construct segment in direction of travel
    const float segmentLength = 30.0f;
    Vector3 start = mOwner->GetPosition();
    Vector3 dir = mOwner->GetForward();
    Vector3 end = start + dir * segmentLength;
    
    // Create line segment
    LineSegment l(start, end);
    
    // Test segment vs world
    PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
    PhysWorld::CollisionInfo info;
    
    // (Don't collide vs player)
    if (phys->SegmentCast(l, info) && info.mActor != mPlayer)
    {
        // If we collided, reflect the ball about the normal
        dir = Vector3::Reflect(dir, info.mNormal);
        mOwner->RotateToNewForward(dir);
        
        // Did we hit a target?
        TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
        if (target)
        {
            static_cast<BallActor*>(mOwner)->HitTarget();
        }
    }
    
    // Base class update moves based on forward speed
    MoveComponent::Update(deltaTime);
}
