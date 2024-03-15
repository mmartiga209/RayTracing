#pragma once

#include "ColorShading.hh"
#include "NormalShading.hh"
#include "NColorShading.hh"
#include "BlinnPhongShading.hh"
#include "CelShading.hh"

class ShadingFactory
{
public:
    ShadingFactory() {};
    typedef enum  SHADING_TYPES{
        COLORSHADING,
        NORMAL,
        NCOLORSHADING,
        BLINNPHONG,
        TOON
    } SHADING_TYPES;

    static ShadingFactory& getInstance() {
        static ShadingFactory instance;
        return instance;
    }

    shared_ptr<ShadingStrategy>   createShading(SHADING_TYPES t);
    SHADING_TYPES                 getShadingType(QString s);
    QString                       getNameType(SHADING_TYPES t);
    ShadingFactory::SHADING_TYPES getIndexType(shared_ptr<ShadingStrategy> m);
};

