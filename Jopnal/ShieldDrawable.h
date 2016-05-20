#ifndef SHIELD_DRAWABLE
#define SHIELD_DRAWABLE

#include <Jopnal/Jopnal.hpp>

class ShieldDrawable : public jop::GenericDrawable
{
public:
    ShieldDrawable(jop::Object& object, jop::Renderer& renderer)
        : GenericDrawable(object, renderer)
    {}

    void draw(const jop::Camera* camera, const jop::LightContainer& lights, jop::Shader& shader)const override
    {
        jop::GlState::setFaceCull(false);
        GenericDrawable::draw(camera, lights, shader);
        jop::GlState::setFaceCull(true);
    }

private:

};

#endif