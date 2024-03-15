#pragma once

#include "ShadingStrategy.hh"

class NColorShading: public ShadingStrategy
{
public:
    NColorShading() {};
    vec3 shading(shared_ptr<Scene> scene, vector<shared_ptr<HitRecord>> info, vec3 lookFrom, bool shadow) override;
    ~NColorShading(){};
};

