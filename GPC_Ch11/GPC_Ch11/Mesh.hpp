//
//  Mesh.hpp
//  GPC_Ch11
//
//  Created by Inoue Shinichi on 2023/07/13.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>
#include <string>
#include "Collision.hpp"

class Mesh
{
public:
    Mesh();
    ~Mesh();
    
    bool Load(const std::string& fileName, class Renderer* renderer);
    void Unload();
    
    // Get the vertex array associated with this mesh
    class VertexArray* GetVertexArray() { return mVertexArray; }
    
    // Get a texture from specified index
    class Texture* GetTexture(size_t index);
    
    // Get name of shader
    const std::string& GetShaderName() const { return mShaderName; }
    
    // Get object space bounding box
    const AABB& GetBox() const { return mBox; }
    
    // Get specular power of mesh
    float GetSpecPower() const { return mSpecPower; }
    
private:
    // AABB collision
    AABB mBox;
    
    // Textures associated with this mesh
    std::vector<class Texture*> mTextures;
    
    // Vertex array associated by mesh
    class VertexArray* mVertexArray;
    
    // Name of shader specified by mesh
    std::string mShaderName;
    
    // Stores object space bounding shere radius
    float mRadius;
    
    // Specular power of surface
    float mSpecPower;
};

#endif /* Mesh_hpp */
