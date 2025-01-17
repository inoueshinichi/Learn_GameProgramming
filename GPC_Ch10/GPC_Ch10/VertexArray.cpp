//
//  VertexArray.cpp
//  GPC_Ch10
//
//  Created by Inoue Shinichi on 2023/03/11.
//

#include "VertexArray.hpp"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts,
                         unsigned int numVerts,
                         const unsigned int* indices,
                         unsigned int numIndices)
    : mNumVerts(numVerts)
    , mNumIndices(numIndices)
{
    // Create vertex array
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);
    
    // Create vertex buffer
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 numVerts * 8 * sizeof(float),
                 verts,
                 GL_STATIC_DRAW);
    
    // Create index buffer
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 numIndices * sizeof(unsigned int),
                 indices,
                 GL_STATIC_DRAW);
    
    // Specify the vertex attributes
    // (For now, assume one vertex format)
    // Position is 3 float
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, // 属性番号
                          3, // 個数
                          GL_FLOAT, // 値の型
                          GL_FALSE, // 整数型のときtrue
                          8 * sizeof(float), // VertexArrayのサイズ
                          0); // オフセット
    
    // Normal is 3 floats
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          reinterpret_cast<void*>(sizeof(float) * 3));
    
    // Texture coordinates is 2 floats
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          reinterpret_cast<void*>(sizeof(float) * 6));
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mIndexBuffer);
    glDeleteBuffers(1, &mVertexArray);
}

void VertexArray::SetActive()
{
    glBindVertexArray(mVertexArray);
}


