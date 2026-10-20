#include <spdlog/spdlog.h>

#include "graphics/scene/Scene.h"

#include "graphics/Frustum.h"
#include "graphics/scene/SceneQuery.h"
#include "graphics/scene/SceneObject.h"
#include "graphics/scene/ScenePointLight.h"
#include "graphics/scene/SceneDirectionalLight.h"

Scene::Scene() : m_ambientColor(0.f), m_ambientIntensity(0.f) {}

Scene::~Scene() {}

SceneObjectId Scene::createObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                                   const glm::vec3& rotation, const glm::vec3& scale,
                                   const OrientedBoundingBox& localBounds)
{
    m_objects.push_back(SceneObject(mesh, material, position, rotation, scale, localBounds));
    return m_objects.size() - 1;
}

bool Scene::getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material,
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

void Scene::setObject(ResourceId id, ResourceId mesh, ResourceId material,
                       const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    // TODO Needs to be changed for better data structures
    if (id < 0 || static_cast<size_t>(id) >= m_objects.size())
    {
        spdlog::error("Invalid objects id {}.", id);
        return;
    }

    // Write data
    m_objects[id].m_mesh = mesh;
    m_objects[id].m_material = material;
    m_objects[id].m_position = position;
    m_objects[id].m_rotation = rotation;
    m_objects[id].m_scale = scale;
    m_objects[id].updateWorldBounds();
    return;
}

SceneObjectId Scene::createPointLight(const glm::vec3& position, float radius,
                                       const glm::vec3& color, float intensity, bool castsShadow)
{
    m_pointLights.push_back(ScenePointLight(position, radius, color, intensity, castsShadow));
    return m_pointLights.size() - 1;
}

bool Scene::getPointLight(SceneObjectId id, glm::vec3& position, float& radius, glm::vec3& color,
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

void Scene::setPointLight(SceneObjectId id, const glm::vec3& position, float radius,
                           const glm::vec3& color, float intensity, bool castsShadow)
{
    // TODO Needs to be changed for better data structures
    if (id < 0 || static_cast<size_t>(id) >= m_pointLights.size())
    {
        spdlog::error("Invalid point lights id {}.", id);
        return;
    }

    // Write data
    m_pointLights[id].m_position = position;
    m_pointLights[id].m_radius = radius;
    m_pointLights[id].m_color = color;
	m_pointLights[id].m_intensity = intensity;
	m_pointLights[id].m_castsShadow = castsShadow;
    return;
}

SceneObjectId Scene::createDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                             float intensity, bool castsShadow)
{
    m_directionalLights.push_back(SceneDirectionalLight(direction, color, intensity, castsShadow));
    return m_directionalLights.size() - 1;
}

bool Scene::getDirectionalLight(SceneObjectId id, glm::vec3& direction, glm::vec3& color,
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

void Scene::setDirectionalLight(SceneObjectId id, const glm::vec3& direction,
                                 const glm::vec3& color, float intensity, bool castsShadow)
{
    // TODO Needs to be changed for better data structures
    if (id < 0 || static_cast<size_t>(id) >= m_directionalLights.size())
    {
        spdlog::error("Invalid directional lights id {}.", id);
        return;
    }

    // Write data
    m_directionalLights[id].m_direction = direction;
	m_directionalLights[id].m_color = color;
	m_directionalLights[id].m_intensity = intensity;
	m_directionalLights[id].m_castsShadow = castsShadow;
    return;
}

void Scene::setAmbientLight(const glm::vec3& color, float intensity)
{
    m_ambientColor = color;
    m_ambientIntensity = intensity;
}

bool Scene::getAmbientLight(glm::vec3& color, float& intensity) const
{
    color = m_ambientColor;
    intensity = m_ambientIntensity;
    return true;
}

bool Scene::getSceneBounds(glm::vec3& center, float& radius) const
{
    if (m_objects.empty())
    {
        center = glm::vec3(0.f);
        radius = 0.f;
        return false;
    }

    // The center is the middle of the object centers, the radius reaches the
    // object that is furthest away from it.
    glm::vec3 minimum = m_objects.front().m_worldBounds.getCenter();
    glm::vec3 maximum = minimum;
    for (const SceneObject& object : m_objects)
    {
        const glm::vec3 objectCenter = object.m_worldBounds.getCenter();
        minimum = glm::min(minimum, objectCenter);
        maximum = glm::max(maximum, objectCenter);
    }
    center = (minimum + maximum) * 0.5f;

    radius = 0.f;
    for (const SceneObject& object : m_objects)
    {
        const glm::vec3 objectCenter = object.m_worldBounds.getCenter();
        // The half extents of the box are the radii of the sphere around it.
        const float objectRadius = glm::length(object.m_worldBounds.getHalfExtents());
        radius = std::max(radius, glm::length(objectCenter - center) + objectRadius);
    }
    return true;
}

ISceneQuery* Scene::createQuery(const glm::mat4& viewProjection) const
{
    // New query with max storage
    // TODO Allocate less storage, needs experiments on how much is actually needed
    SceneQuery* query =
        new SceneQuery((unsigned int)m_objects.size(), (unsigned int)m_pointLights.size());

    Frustum frustum(viewProjection);

    // TODO Occlusion culling, better data structure for objects
    for (unsigned int i = 0; i < m_objects.size(); ++i)
    {
        if (!frustum.intersects(m_objects[i].m_worldBounds))
        {
            continue;
        }
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
