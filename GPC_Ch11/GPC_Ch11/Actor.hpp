//
//  Actor.hpp
//  GPC_Ch11
//
//  Created by Inoue Shinichi on 2023/07/02.
//

#ifndef Actor_hpp
#define Actor_hpp

#include <vector>
#include "Math.hpp"
#include <cstdint>

class Actor
{
public:
    enum State
    {
        EActive,
        EPaused,
        EDead,
    };
    
    Actor(class Game* game);
    virtual ~Actor();
    
    // Update function called from Game (not override)
    void Update(float deltaTime);
    
    // Updates all the components attached to the actor (not override)
    void UpdateComponents(float deltaTime);
    
    // Any actor-specific update code (overridable)
    virtual void UpdateActor(float deltaTime);
    
    // ProcessInput function called from Game (not overridable)
    void ProcessInput(const uint8_t* keyState);
    
    // Any actor-specific input code (overridable)
    virtual void ActorInput(const uint8_t* keyState);
    
    // Getter / Setter
    const Vector3& GetPosition() const { return mPosition; }
    void SetPosition(const Vector3& pos)
    {
        mPosition = pos;
        mRecomputeWorldTransform = true;
    }
    
    float GetScale() const { return mScale; }
    void SetScale(float scale)
    {
        mScale = scale;
        mRecomputeWorldTransform = true;
    }
    
    const Quaternion& GetRotation() const { return mRotation; }
    void SetRotation(const Quaternion& rotation)
    {
        mRotation = rotation;
        mRecomputeWorldTransform = true;
    }
    
    void ComputeWorldTransform();
    const Matrix4& GetWorldTransform() const { return mWorldTransform; }
    
    Vector3 GetForward() const
    {
        return Vector3::Transform(Vector3::UnitX, mRotation);
    }
    Vector3 GetRight() const
    {
        return Vector3::Transform(Vector3::UnitY, mRotation);
    }
    
    void RotateToNewForward(const Vector3& forward);
    
    State GetState() const { return mState; }
    void SetState(State state) { mState = state; }
    
    class Game* GetGame() { return mGame; }
    
    // Add / Remove components
    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);
    
private:
    // Actor's state
    State mState;
    
    // Transform
    Matrix4 mWorldTransform;
    Vector3 mPosition;
    Quaternion mRotation;
    float mScale;
    bool mRecomputeWorldTransform;
    
    std::vector<class Component*> mComponents;
    class Game* mGame;
};

#endif /* Actor_hpp */
