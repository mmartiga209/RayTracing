#pragma once

#include "ShadingStrategy.hh"

class NormalShading: public ShadingStrategy
{
public:
    NormalShading() {};
    vec3 shading(shared_ptr<Scene> scene, vector<shared_ptr<HitRecord>> info, vec3 lookFrom, bool shadow) override;
    ~NormalShading(){};
};

