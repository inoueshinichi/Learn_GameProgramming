//
//  CircleComponent.hpp
//  GPC_Ch09
//
//  Created by Inoue Shinichi on 2022/08/18.
//

#ifndef CircleComponent_hpp
#define CircleComponent_hpp

#include "Component.hpp"
#include "Math.hpp"

class CircleComponent : public Component
{
public:
    CircleComponent(class Actor* owner);
    
    void SetRadius(float radius) { mRadius = radius; }
    float GetRadius() const;
    
    const Vector3& GetCenter() const;
    
private:
    float mRadius;
};

#endif /* CircleComponent_hpp */
