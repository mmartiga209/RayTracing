#pragma once

#include <vector>

#include <QString>
#include <QFile>
#include <QRegularExpression>

#include "Object.hh"
#include "Face.hh"
#include "Triangle.hh"
#include "Box.hh"
#include "Sphere.hh"

using namespace std;

class Mesh : public Object
{
public:
    Mesh() {};
    Mesh(const QString &fileName);

    virtual bool hit( Ray& r, float tmin, float tmax) const override;
    virtual bool allHits(Ray& r, float tmin, float tmax) const override;

    virtual void update(int nframe) override;
    virtual void aplicaTG(shared_ptr<TG> tg) override;

    virtual void read (const QJsonObject &json) override;
    virtual void write(QJsonObject &json) const override;
    virtual void print(int indentation) const override;

    virtual ~Mesh();
private:


    QString nom;
    vector<Face> cares; // facees o cares de l'objecte
    vector<vec4> vertexs; // vertexs de l'objecte sense repetits
    vector<shared_ptr<Triangle>> triangles; // triangles de la malla

    shared_ptr<Box> aabb; // axis aligned bounding box
    shared_ptr<Sphere> bSphere; // bounding sphere

    void load(QString filename);
    void makeTriangles();
    void makeBoundingVolumes();
};

