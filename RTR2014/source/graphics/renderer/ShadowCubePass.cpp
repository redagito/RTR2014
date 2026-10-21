#include "graphics/renderer/ShadowCubePass.h"

#include <spdlog/spdlog.h>

#include <glm/ext/matrix_transform.hpp>
#include <string>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/IWindow.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Transformer.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"

const ShadowCubePass::CameraDirection ShadowCubePass::cameraDirections[6] = {
    {0, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {1, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {2, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {3, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {4, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {5, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)}};

ShadowCubePass::ShadowCubePass() { return; }

ShadowCubePass::~ShadowCubePass() { return; }

void ShadowCubePass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
                            const IGraphicsResourceManager& manager)
{
    m_shadowCubePassShader = manager.getShaderProgram(m_shadowCubePassShaderId);
    if (m_shadowCubePassShader == nullptr)
    {
        fatalError("Shader program for shadow cube pass could not be retrieved.");
    }
    m_shadowCubePassShader->setActive();

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glDisable(GL_BLEND);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Reset viewport
    glViewport(0, 0, 1024, 1024);

    m_shadowCubePassShader->setUniform(lightPositionUniformName, camera.getPosition());

    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowCubeBuffer.getId());
        glNamedFramebufferTextureLayer(m_shadowCubeBuffer.getId(), GL_COLOR_ATTACHMENT0,
                                       m_shadowCubeTexture->getId(), 0,
                                       cameraDirections[i].cubemapFace);
        glNamedFramebufferDrawBuffer(m_shadowCubeBuffer.getId(), GL_COLOR_ATTACHMENT0);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, 1024, 1024);

        // Send view/projection to default shader
        glm::mat4 view =
            glm::lookAt(camera.getPosition(), camera.getPosition() + cameraDirections[i].target,
                        cameraDirections[i].up);

        m_shadowCubePassShader->setUniform(projectionMatrixUniformName, camera.getProjection());
        m_shadowCubePassShader->setUniform(viewMatrixUniformName, view);

        // Query visible scene objects
        auto query = scene.createQuery(camera.getProjection() * view);

        // Traverse visible objects
        while (query->hasNextObject())
        {
            // Get next visible object
            SceneObjectId id = query->getNextObject();

            // Object attributes
            ResourceId meshId = -1;
            ResourceId materialId = -1;
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
            bool castsShadow = true;

            // Retrieve object data
            if (!scene.getObject(id, meshId, materialId, position, rotation, scale, castsShadow))
            {
                // Invalid id
                fatalError("Invalid scene object id {}.", id);
            }
            else
            {
                // Resolve ids
                Mesh* mesh = manager.getMesh(meshId);
                if (mesh == nullptr)
                {
                    // The graphics resource manager logged the missing resource
                    continue;
                }

                Transformer transformer;

                // Set transformations
                transformer.setPosition(position);
                transformer.setRotation(rotation);
                transformer.setScale(scale);

                // The shadow cube only needs the transformed geometry, the
                // material does not matter.
                m_shadowCubePassShader->setActive();
                m_shadowCubePassShader->setUniform(modelMatrixUniformName,
                                                   transformer.getModelMatrix());
                Renderer::draw(mesh);
            }
        }
    }

    // Post draw error check

    // Disable buffer
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    glClearColor(0, 0, 0, 0);
    glCullFace(GL_BACK);
}

bool ShadowCubePass::init(IResourceManager* manager)
{
    std::string shaderFile("data/shader/shadow_cube_pass.ini");
    m_shadowCubePassShaderId = manager->loadShader(shaderFile);

    if (m_shadowCubePassShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.", shaderFile.c_str());
    }

    m_shadowCubeBuffer.resize(1024, 1024);

    // Depth texture
    m_shadowCubeDepthTexture = std::make_shared<Texture>();
    m_shadowCubeDepthTexture->init(1024, 1024, GL_DEPTH_COMPONENT24);

    glTextureParameteri(m_shadowCubeDepthTexture->getId(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_shadowCubeDepthTexture->getId(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_shadowCubeDepthTexture->getId(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_shadowCubeDepthTexture->getId(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint textureId;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureId);

    // Allocate the storage of all six faces
    // The storage must exist before the parameters are set, a storage allocation resets them
    glTextureStorage2D(textureId, 1, GL_R32F, 1024, 1024);

    glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_shadowCubeTexture = std::make_shared<Texture>(textureId, false, 1024, 1024, GL_R32F, GL_RED);

    // The depth attachment is registered with the frame buffer, the color
    // attachment is a layer of the cube map that is pointed at the face that
    // is rendered, so it is attached per face.
    m_shadowCubeBuffer.attach(m_shadowCubeDepthTexture, GL_DEPTH_ATTACHMENT);

    glNamedFramebufferDrawBuffer(m_shadowCubeBuffer.getId(), GL_NONE);
    glNamedFramebufferReadBuffer(m_shadowCubeBuffer.getId(), GL_NONE);

    spdlog::info("Shadow cube buffer state: {}.", m_shadowCubeBuffer.getState().c_str());

    // Reset framebuffer
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    return true;
}

const std::shared_ptr<Texture>& ShadowCubePass::getShadowMap() const { return m_shadowCubeTexture; }