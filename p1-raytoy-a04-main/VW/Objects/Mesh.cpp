#include <QVector>
#include <QVector3D>

#include "Mesh.hh"

Mesh::Mesh(const QString &fileName): Object()
{
    nom = fileName;
    load(fileName);
}

Mesh::~Mesh() {
    if (cares.size() > 0) cares.clear();
    if (vertexs.size() > 0) vertexs.clear();
    if (triangles.size() > 0) triangles.clear();
}

void Mesh::makeBoundingVolumes() {
    float minX = numeric_limits<float>::infinity();
    float minY = numeric_limits<float>::infinity();
    float minZ = numeric_limits<float>::infinity();

    float maxX = -1.0*numeric_limits<float>::infinity();
    float maxY = -1.0*numeric_limits<float>::infinity();
    float maxZ = -1.0*numeric_limits<float>::infinity();

    
    // Busquem les minimes coordenades per cada component
    for(vec4 v : this->vertexs) {
        if(v.x < minX) minX = v.x;
        if(v.y < minY) minY = v.y;
        if(v.z < minZ) minZ = v.z;

        if(v.x > maxX) maxX = v.x;
        if(v.y > maxY) maxY = v.y;
        if(v.z > maxZ) maxZ = v.z;
    }

    vec3 pmin(minX, minY, minZ);
    vec3 pmax(maxX, maxY, maxZ);
    // bounding box: directament amb els dos punts trobats
    this->aabb = make_shared<Box>(pmin, pmax);

    shared_ptr<Material> aabbMat = make_shared<Lambertian>(vec3(255/255.,192/255.,203/255.));
    aabbMat->Kt = vec3(0.7,0.7,0.7);
    this->aabb->setMaterial(aabbMat);

    // bounding sphere
    // implementació naive: centre el punt mig de la bounding box 
    // i radi el maxim entre les distancies del punt als punts min/max de la bbox
    // TODO: calcular una millor esfera, per ex. amb l'algorisme de Welzl 
    vec3 center(
        (minX + maxX)/2,
        (minY + maxY)/2,
        (minZ + maxZ)/2
    );
    this->bSphere = make_shared<Sphere>(center, length(pmin-center));
    shared_ptr<Material> bSphereMat = make_shared<Lambertian>(vec3(76/255.,187/255.,23/255.));
    aabbMat->Kt = vec3(0.8,0.8,0.8);
    this->aabb->setMaterial(aabbMat);

}

void Mesh::makeTriangles() {
    // TO DO Tutorial 1: A implementar (CREAT AMB PUNTERS, MILLORAR EN EL FUTUR, PARLAR ANNA)
    triangles.clear();
    for (Face f : cares) {
        int i=f.idxVertices[0], j=f.idxVertices[1], k=f.idxVertices[2];
        shared_ptr<Triangle> tri = make_shared<Triangle>(vec3(vertexs.at(i)),vec3(vertexs.at(j)),vec3(vertexs.at(k)));
        triangles.push_back(tri);
        tri->setMaterial(this->material);
    }
}

bool Mesh::hit(Ray &raig, float tmin, float tmax) const {
    
    // mirem primer si interseca amb la aabb
    Ray aux2(raig.getOrigin(), raig.getDirection(), tmin, tmax);
    if(!aabb->hit(aux2, tmin, tmax)) {
        return false;
    }

    // Intersecció amb els triangles de la malla
    Ray aux(raig.getOrigin(), raig.getDirection(), tmin, tmax);
    bool any = false;
    for (shared_ptr<Triangle> tr : triangles) {
        any |= tr->hit(aux, tmin, tmax);
    }

    if(any) {
        // Si n'hi ha, agafem només la intersecció més propera
        shared_ptr<HitRecord> min;
        float t_min = tmax;
        for (shared_ptr<HitRecord> h : aux.getHitRecords()) {
            if (h->t < t_min) {
                t_min = h->t;
                min = h;
            }
        }
        raig.addHit(min);
    }

    return any;
}

bool Mesh::allHits(Ray& raig, float tmin, float tmax) const {
    // Intersecció amb els triangles de la malla
    bool any = false;
    for (shared_ptr<Triangle> tr : triangles) {
        any |= tr->allHits(raig, tmin, tmax);
    }
    return any;
}


void Mesh::aplicaTG(shared_ptr<TG> t) {
    // TO DO: Fase 1
}

void Mesh::update(int frame) {
    // TO DO: Cal ampliar-lo per a fer el update de la mesh
}

void Mesh::load (QString fileName) {
    QFile file(fileName);
    if(file.exists()) {
        if(file.open(QFile::ReadOnly | QFile::Text)) {
            while(!file.atEnd()) {
                QString line = file.readLine().trimmed();
                QStringList lineParts = line.split(QRegularExpression("\\s+"));
                if(lineParts.count() > 0) {
                    // if it’s a comment
                    if(lineParts.at(0).compare("#", Qt::CaseInsensitive) == 0)
                    {
                        // qDebug() << line.remove(0, 1).trimmed();
                    }

                    // if it’s a vertex position (v)
                    else if(lineParts.at(0).compare("v", Qt::CaseInsensitive) == 0)
                    {
                        vertexs.push_back(vec4(lineParts.at(1).toFloat(),
                                               lineParts.at(2).toFloat(),
                                               lineParts.at(3).toFloat(), 1.0f));
                    }

                    // if it’s a normal (vn)
                    else if(lineParts.at(0).compare("vn", Qt::CaseInsensitive) == 0)
                    {

                    }
                    // if it’s a texture (vt)
                    else if(lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0)
                    {

                    }

                    // if it’s face data (f)
                    // there’s an assumption here that faces are all triangles
                    else if(lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0)
                    {
                        Face *face = new Face();

                        // get points from v array
                        face->idxVertices.push_back(lineParts.at(1).split("/").at(0).toInt() - 1);
                        face->idxVertices.push_back(lineParts.at(2).split("/").at(0).toInt() - 1);
                        face->idxVertices.push_back(lineParts.at(3).split("/").at(0).toInt() - 1);

                        cares.push_back(*face);
                    }

                }
            }
            makeTriangles();
            makeBoundingVolumes();
            file.close();
        } else {
            qWarning("Boundary object file can not be opened.");
        }
    } else  qWarning("Boundary object file not found.");
}

void Mesh::read (const QJsonObject &json)
{
    Object::read(json);
    if (json.contains("objFileName") && json["objFileName"].isString()) {
        nom = json["objFileName"].toString();
        load(nom);
    }
}


//! [1]
void Mesh::write(QJsonObject &json) const
{
    Object::write(json);
    json["objFileName"] = nom;
}
//! [1]

void Mesh::print(int indentation) const
{
    Object::print(indentation);
    const QString indent(indentation * 2, ' ');
    QTextStream(stdout) << indent << "objFileName:\t" << nom << "\n";
}



