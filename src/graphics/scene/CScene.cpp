#include "CScene.h"

#include "CSceneQuery.h"
#include "SSceneObject.h"
#include "SScenePointLight.h"
#include "SSceneDirectionalLight.h"

CScene::CScene() {}

CScene::~CScene() {}

SceneObjectId CScene::createObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                                   const glm::vec3& rotation, const glm::vec3& scale)
{
    m_objects.push_back(SSceneObject(mesh, material, position, rotation, scale));
    return m_objects.size() - 1;
}

bool CScene::getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material,
                       glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) const
{
    // TODO Needs to be changed for better data structures
    if (id < 0 || ((unsigned int)id) >= m_objects.size())
    {
        return false;
    }

    // Write data
    mesh = m_objects.at(id).m_mesh;
    material = m_objects.at(id).m_material;
    position = m_objects.at(id).m_position;
    rotation = m_objects.at(id).m_rotation;
    scale = m_objects.at(id).m_scale;
    return true;
}

void CScene::setObject(ResourceId id, ResourceId mesh, ResourceId material,
                       const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    // TODO Needs to be changed for better data structures
    assert(id >= 0 && ((unsigned int)id) < m_objects.size() && "Invalid scene object id");

    // Write data
    m_objects[id].m_mesh = mesh;
    m_objects[id].m_material = material;
    m_objects[id].m_position = position;
    m_objects[id].m_rotation = rotation;
    m_objects[id].m_scale = scale;
    return;
}

SceneObjectId CScene::createPointLight(const glm::vec3& position, float radius,
                                       const glm::vec3& color, float intensity, bool castsShadow)
{
    m_pointLights.push_back(SScenePointLight(position, radius, color, intensity, castsShadow));
    return m_pointLights.size() - 1;
}

bool CScene::getPointLight(SceneObjectId id, glm::vec3& position, float& radius, glm::vec3& color,
                           float& intensity, bool& castsShadow) const
{
    // TODO Needs to be changed for better data structures
    if (id < 0 || ((unsigned int)id) >= m_pointLights.size())
    {
        return false;
    }

    // Write data
    position = m_pointLights.at(id).m_position;
    radius = m_pointLights.at(id).m_radius;
    color = m_pointLights.at(id).m_color;
    intensity = m_pointLights.at(id).m_intensity;
	castsShadow = m_pointLights.at(id).m_castsShadow;
    return true;
}

void CScene::setPointLight(SceneObjectId id, const glm::vec3& position, float radius,
                           const glm::vec3& color, float intensity, bool castsShadow)
{
    // TODO Needs to be changed for better data structures
    assert(id >= 0 && ((unsigned int)id) < m_pointLights.size() && "Invalid scene object id");

    // Write data
    m_pointLights[id].m_position = position;
    m_pointLights[id].m_radius = radius;
    m_pointLights[id].m_color = color;
	m_pointLights[id].m_intensity = intensity;
	m_pointLights[id].m_castsShadow = castsShadow;
    return;
}

SceneObjectId CScene::createDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                             float intensity, bool castsShadow)
{
    m_directionalLights.push_back(SSceneDirectionalLight(direction, color, intensity, castsShadow));
    return m_directionalLights.size() - 1;
}

bool CScene::getDirectionalLight(SceneObjectId id, glm::vec3& direction, glm::vec3& color,
                                 float& intensity, bool& castsShadow) const
{
    // TODO Needs to be changed for better data structures
    if (id < 0 || ((unsigned int)id) >= m_directionalLights.size())
    {
        return false;
    }

    // Write data
    direction = m_directionalLights.at(id).m_direction;
    color = m_directionalLights.at(id).m_color;
    intensity = m_directionalLights.at(id).m_intensity;
	castsShadow = m_directionalLights.at(id).m_castsShadow;
    return true;
}

void CScene::setDirectionalLight(SceneObjectId id, const glm::vec3& direction,
                                 const glm::vec3& color, float intensity, bool castsShadow)
{
    // TODO Needs to be changed for better data structures
    assert(id >= 0 && ((unsigned int)id) < m_directionalLights.size() && "Invalid scene object id");

    // Write data
    m_directionalLights[id].m_direction = direction;
	m_directionalLights[id].m_color = color;
	m_directionalLights[id].m_intensity = intensity;
	m_directionalLights[id].m_castsShadow = castsShadow;
    return;
}

void CScene::setAmbientLight(const glm::vec3& color, float intensity)
{
    m_ambientColor = color;
    m_ambientIntensity = intensity;
}

bool CScene::getAmbientLight(glm::vec3& color, float& intensity) const
{
    color = m_ambientColor;
    intensity = m_ambientIntensity;
    return true;
}

ISceneQuery* CScene::createQuery(const ICamera& camera) const
{
    // TODO Extract frustum planes from camera

    // New query with max storage
    // TODO Allocate less storage, needs experiments on how much is actually needed
    CSceneQuery* query =
        new CSceneQuery((unsigned int)m_objects.size(), (unsigned int)m_pointLights.size());

    // TODO Frustum culling, occlusion culling, better data structure for objects
    // For now add all objects
    for (unsigned int i = 0; i < m_objects.size(); ++i)
    {
        // Counter variable is object id
        query->addObject(i);
    }

    // TODO Light culling
    // For now add all point lights
    for (unsigned int i = 0; i < m_pointLights.size(); ++i)
    {
        // Counter variable is light id
        query->addPointLight(i);
    }

    // TODO Directional light culling?
    // For now add all directional lights
    for (unsigned int i = 0; i < m_directionalLights.size(); ++i)
    {
        // Counter variable is light id
        query->addDirectionalLight(i);
    }

    // Return query
    return query;
}
