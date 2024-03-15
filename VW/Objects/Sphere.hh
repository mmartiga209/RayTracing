/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#pragma once

#include "Object.hh"
#include "../TG/TranslateTG.hh"


class Sphere: public Object  {
public:
    Sphere();

    Sphere(vec3 cen, float r);

    virtual ~Sphere() {}
    virtual bool hit(Ray& r, float tmin, float tmax) const override;
    virtual bool allHits(Ray& r, float tmin, float tmax) const override;

    virtual void update(int nframe) override;
    virtual void aplicaTG(shared_ptr<TG> tg) override;

    virtual void read (const QJsonObject &json) override;
    virtual void write(QJsonObject &json) const override;
    virtual void print(int indentation) const override;


    vec3  getCenter() { return center;};
    float getRadius() { return radius;}

private:
    // Centre de l'esfera
    vec3 center;
    // Radi de l'esfera
    float radius;
};





