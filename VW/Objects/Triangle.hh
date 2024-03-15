/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#pragma once

#include "Object.hh"
#include "../TG/TranslateTG.hh"


class Triangle: public Object  {
public:
    Triangle();

    Triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);

    virtual ~Triangle() {}
    virtual bool hit(Ray& r, float tmin, float tmax) const override;
    virtual bool allHits(Ray& r, float tmin, float tmax) const override;

    virtual void update(int nframe) override;
    virtual void aplicaTG(shared_ptr<TG> tg) override;

    virtual void read (const QJsonObject &json) override;
    virtual void write(QJsonObject &json) const override;
    virtual void print(int indentation) const override;

private:
    // Vertexs del triangle
    vec3 v1;
    vec3 v2;
    vec3 v3;

    // Vector normal al triangle
    vec3 normal;
};
