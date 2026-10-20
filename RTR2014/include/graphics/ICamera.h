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

struct FeatureInfo
{
    FogType fogType = FogType::None;
	bool fogActive = false; // TODO Use this flag instead FogType::NONE
    float fogBegin = 0.f;
    float fogEnd = 0.f;
    float fogDensity = 0.f;

    RenderMode renderMode = RenderMode::Final;

    bool shadowsActive = true;
    
    bool dofActive = true;
    float dofNearFocus = 0.5f;
    float dofNearBlur = 0.1f;
    float dofFarFocus = 50.f;
    float dofFarBlur = 80.f;
    
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

    /**
    * \brief Returns the distance of the near plane.
    */
    virtual float getZNear() const = 0;

    /**
    * \brief Returns the distance of the far plane.
    */
    virtual float getZFar() const = 0;

    virtual glm::vec3 getPosition() const = 0;

    // TODO extract functionality to a better place
    virtual FeatureInfo& getFeatureInfoForWrite() { return m_info; } // TODO Rename
    virtual const FeatureInfo& getFeatureInfo() const { return m_info; }

   private:
    FeatureInfo m_info;
};
