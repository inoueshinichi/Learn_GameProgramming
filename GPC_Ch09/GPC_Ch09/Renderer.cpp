//
//  Renderer.cpp
//  GPC_Ch09
//
//  Created by Inoue Shinichi on 2022/08/18.
//

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include <algorithm>
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "SpriteComponent.hpp"
#include "MeshComponent.hpp"
#include <GL/glew.h>

#include "Game.hpp"

Renderer::Renderer(Game* game)
    : mGame(game)
    , mSpriteShader(nullptr)
    , mMeshShader(nullptr)
{
    
}

Renderer::~Renderer()
{
    
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    
    // Set OpenGL attributes
    // Use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Specify version3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    // Request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    
    // Create Main Window
    mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 9)",
                               100,
                               100,
                               static_cast<int>(mScreenWidth),
                               static_cast<int>(mScreenHeight),
                               SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    
    // Create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }
    
    // On some platforms, GLEW will emit a benign error code,
    // so clear it
    glGetError();
    
    
    // Make sure we can create/compile shaders
    if (!LoadShaders())
    {
        SDL_Log("Failed to load shaders.");
        return false;
    }

    // Create quad for drawing sprites
    CreateSpriteVerts();
    
    return true;
}

void Renderer::Shutdown()
{
    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;
    mMeshShader->Unload();
    delete mMeshShader;
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();
    
    // Destroy meshes
    for (auto i : mMeshes)
    {
        i.second->Unload();
        delete i.second;
    }
    mMeshes.clear();
}

void Renderer::Draw()
{
    // Set the clear color to light grey
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw mesh components
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    // Set the mesh shader active
    mMeshShader->SetActive();
    
    // Update view-projection matrix
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
    
    // Update lighting uniforms
    SetLightUniforms(mMeshShader);
    for (auto mc : mMeshComps)
    {
        if (mc->GetVisible())
        {
            mc->Draw(mMeshShader);
        }
    }
    
    // Draw all sprite components
    // Disable depth buffering
    glDisable(GL_DEPTH_TEST);
    // Enable alpha blending on the color buffer
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,
                        GL_ZERO);
    
    // Set shader/vao as active
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();
    for (auto sprite : mSprites)
    {
        if (sprite->GetVisible())
        {
            sprite->Draw(mSpriteShader);
        }
    }
    
    // Swap the buffers
    SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
    // Find the insertion point in the sorted vector
    // (The first element with a highter draw order than me)
    int myDrawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (;
         iter != mSprites.end();
         ++iter)
    {
        if (myDrawOrder < (*iter)->GetDrawOrder())
        {
            break;
        }
    }
    
    // Inserts element before position of iterator
    mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    mSprites.erase(iter);
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
    mMeshComps.emplace_back(mesh);
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
    auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
    mMeshComps.erase(iter);
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName))
        {
            mTextures.emplace(fileName, tex);
        }
        else
        {
            delete tex;
            tex = nullptr;
        }
    }
    return tex;
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
    Mesh* m = nullptr;
    auto iter = mMeshes.find(fileName);
    if (iter != mMeshes.end())
    {
        m = iter->second;
    }
    else
    {
        m = new Mesh();
        if (m->Load(fileName, this))
        {
            mMeshes.emplace(fileName, m);
        }
        else
        {
            delete m;
            m = nullptr;
        }
    }
    return m;
}

bool Renderer::LoadShaders()
{
    // Create sprite shader
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load(SHADERS_DIR "Shaders/Sprite.vert",
                             SHADERS_DIR "Shaders/Sprite.frag"))
    {
        return false;
    }
    
    mSpriteShader->SetActive();
    // Set the view-projection matrix
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth,
                                                     mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
    
    // Create basic mesh shader
    mMeshShader = new Shader();
    if (!mMeshShader->Load(SHADERS_DIR "Shaders/Phong.vert",
                           SHADERS_DIR "Shaders/Phong.frag"))
    {
        return false;
    }
    
    mMeshShader->SetActive();
    // Set the view-projection matrix
    mView = Matrix4::CreateLookAt(Vector3::Zero,
                                  Vector3::UnitX,
                                  Vector3::UnitZ);
    
    mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
                                                mScreenWidth,
                                                mScreenHeight,
                                                10.0f,
                                                10000.0f);
    
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
    return true;
}

void Renderer::CreateSpriteVerts()
{
    float vertices[] = {
        -0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
        0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
        0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
        -0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
    };
    
    /*
        0 --- 1
        |＼   |
        |  ＼ |
        3 --- 2
     */
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
    // Camera position is from inverted view
    Matrix4 invView = mView;
    invView.Invert();
    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
    // Ambient light
    shader->SetVectorUniform("uAmbientLight", mAmbientLight);
    // Directional light
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);
}

Vector3 Renderer::Unproject(const Vector3& scenePoint) const
{
    // Convert scenePoint to device coordinates (between -1 and +1)
    Vector3 deviceCoord = scenePoint; // NDC
    deviceCoord.x /= (mScreenWidth) * 0.5f;
    deviceCoord.y /= (mScreenHeight) * 0.5f;
    
    // Transform vector by unprojection matrix
    Matrix4 unprojection = mView * mProjection; // ビュー行列と射影行列の積の逆行列が逆射影行列
    unprojection.Invert();
    return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
    // Get start point (in center of screen on near plane)
    Vector3 screenPoint(0.0f, 0.0f, 0.0f);
    outStart = Unproject(screenPoint);
    // Get end point (in center of screen , between near and far)
    screenPoint.z = 0.9f;
    Vector3 end = Unproject(screenPoint);
    // Get direction vector
    outDir = end - outStart;
    outDir.Normalize();
}
