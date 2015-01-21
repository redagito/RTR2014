#pragma once

#include <glm/glm.hpp>

enum class FogType
{
    None,
    Linear,
    Exp,
    Exp2
};

enum class RenderMode
{
    Final,
    Color,
    Depth,
    Normals,
    Lights
};

struct SFeatureInfo
{
    FogType fogType = FogType::None;
    float fogBegin = 0.f;
    float fogEnd = 0.f;
    float fogDensity = 0.f;

    RenderMode renderMode = RenderMode::Final;

    bool shadowsActive = true;
    
    bool dofActive = true;
    float dofNearFocus = 0.3f;
    float dofNearBlur = 0.1f;
    float dofFarFocus = 10.f;
    float dofFarBlur = 20.f;
    
    bool fxaaActive = true;
    
    bool normalMappingActive = true;
};

/**
 * \brief Camera interface class.
 */
class ICamera
{
   public:
    virtual ~ICamera();

    /**
    * \brief Returns view matrix.
    */
    virtual const glm::mat4& getView() const = 0;

    /**
    * \brief Returns projection matrix.
    */
    virtual const glm::mat4& getProjection() const = 0;

    virtual glm::vec3 getPosition() const = 0;

    // TODO extract functionality to a better place
    virtual SFeatureInfo& getFeatureInfoForWrite() { return m_info; } // TODO Rename
    virtual const SFeatureInfo& getFeatureInfo() const { return m_info; }

   private:
    SFeatureInfo m_info;
};
