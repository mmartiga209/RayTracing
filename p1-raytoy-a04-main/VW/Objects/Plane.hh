#pragma once

#include "Object.hh"
#include "../TG/TranslateTG.hh"


class Plane: public Object{
public:
    Plane() {};
    Plane(vec3 normal, vec3 pass_point);
    Plane(vec3 normal, float d);

    virtual ~Plane(){}
    virtual bool hit(Ray &r, float tmin, float tmax) const override;
    virtual bool allHits(Ray& r, float tmin, float tmax) const override;

    virtual void update(int nframe) override;
    virtual void aplicaTG(shared_ptr<TG> tg) override;


    virtual void read (const QJsonObject &json) override;
    virtual void write(QJsonObject &json) const override;
    virtual void print(int indentation) const override;

    vec3 normal;
    vec3 point;
private:

};


