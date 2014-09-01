#pragma once

#include <memory>
#include <vector>

#include "graphics/ResourceConfig.h"
#include "core/CVertexBuffer.h"
#include "core/CIndexBuffer.h"

/**
* \brief Contains mesh data (vertices, faces, normals and uv data).
*
* Represents mesh data in the VRAM. It holds a buffer object for vertices,
* one for texture coordinates and one for nomal data. the data can be used for
* direct rendering or used in conjunction with an VAO.
*/
class CMesh
{
   public:
    CMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
          const std::vector<float>& normals, const std::vector<float>& uvs, EPrimitiveType type);

    CMesh(const CMesh&) = delete;
    CMesh& operator=(const CMesh&) = delete;

    /**
    * \brief Deletes graphic resources and frees memory.
    */
    ~CMesh();

    /**
    * \brief Initializes mesh with data.
    */
    bool init(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
              const std::vector<float>& normals, const std::vector<float>& uvs,
              EPrimitiveType type);

    /**
    * \brief Read access to vertex buffer
    */
    const std::unique_ptr<CVertexBuffer>& getVertexBuffer() const;

    /**
    * \brief Read access to index buffer.
    */
    const std::unique_ptr<CIndexBuffer>& getIndexBuffer() const;

    /**
    * \brief Read access to normal buffer
    */
    const std::unique_ptr<CVertexBuffer>& getNormalBuffer() const;

    /**
    * \brief Read access to texture uv buffer
    */
    const std::unique_ptr<CVertexBuffer>& getUVBuffer() const;

   private:
    std::unique_ptr<CVertexBuffer> m_vertices; /**< Mesh vertices. */
    std::unique_ptr<CIndexBuffer> m_indices;   /**< Mesh indices. */
    std::unique_ptr<CVertexBuffer> m_normals;  /**< Per vertex normals. */
    std::unique_ptr<CVertexBuffer> m_uvs;      /**< Texture coordinates. */
    EPrimitiveType m_type;                     /**< Mesh primitoive type. */
};
