//
//  PhysWorld.cpp
//  GPC_Ch10
//
//  Created by Inoue Shinichi on 2023/05/28.
//

#include "PhysWorld.hpp"
#include <algorithm>
#include "BoxComponent.hpp"
#include <SDL.h>

PhysWorld::PhysWorld(Game* game)
    : mGame(game)
{}

bool PhysWorld::SegmentCast(const LineSegment& l,
                            CollisionInfo& outColl)
{
    bool collided = false;
    
    // Initialize closestT to infinity. so first
    // intersection will always update closestT
    float closestT = Math::Infinity;
    Vector3 normal; // 法線
    // Test against all boxes
    for (auto box : mBoxes)
    {
        float t;
        // Does the segment intersect with the box ?
        if (Intersect(l, box->GetWorldBox(), t, normal))
        {
            // Is this closer than previous intersection ?
            if (t < closestT)
            {
                closestT = t;
                outColl.mPoint = l.PointOnSegment(t);
                outColl.mNormal = normal;
                outColl.mBox = box;
                outColl.mActor = box->GetOwner();
                collided = true;
            }
        }
    } // for
    return collided;
}

void PhysWorld::TestPairwise(std::function<void(Actor*, Actor*)> f)
{
    // Naive implementation O(n^2)
    for (size_t i = 0; i < mBoxes.size(); ++i)
    {
        // Don't need to test vs itself and any previous i values
        for (size_t j = i + 1; j < mBoxes.size(); ++j)
        {
            BoxComponent* a = mBoxes[i];
            BoxComponent* b = mBoxes[j];
            if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
            {
                // Call supplied function to handle intersection
                f(a->GetOwner(), b->GetOwner());
            }
        } // for j
    } // for i
}

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> f)
{
    // Sort by min.x
    std::sort(mBoxes.begin(),
              mBoxes.end(),
              [](BoxComponent* a, BoxComponent* b) {
        return a->GetWorldBox().mMin.x < b->GetWorldBox().mMin.x;
    });
    
    for (size_t i = 0; i < mBoxes.size(); ++i)
    {
        // Get max.x for current box
        BoxComponent* a = mBoxes[i];
        float max = a->GetWorldBox().mMax.x;
        for (size_t j = i + 1; j < mBoxes.size(); ++j)
        {
            BoxComponent* b = mBoxes[j];
            // If AABB[j] min is past the max bounds of AABB[i].
            // then there aren't any other possible intersections
            // against AABB[i]
            
            /*
             AABB[j] min が AABB[i] の最大境界を超えている場合、
             AABB[i] と交差する可能性のあるものは他にありません。
             */
            if (b->GetWorldBox().mMin.x > max)
            {
                break;
            }
            else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
            {
                f(a->GetOwner(), b->GetOwner());
            }
        } // for j
    } // for i
}

void PhysWorld::AddBox(BoxComponent* box)
{
    mBoxes.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
    auto iter = std::find(mBoxes.begin(), mBoxes.end(), box);
    if (iter != mBoxes.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mBoxes.end() - 1);
        mBoxes.pop_back();
    }
}
