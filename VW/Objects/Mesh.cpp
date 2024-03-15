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
    if (plans.size() > 0) plans.clear();
}

void Mesh::makeBoundingVolumes() {
    float minX = numeric_limits<float>::infinity();
    float minY = numeric_limits<float>::infinity();
    float minZ = numeric_limits<float>::infinity();

    float maxX = -1.0*numeric_limits<float>::infinity();
    float maxY = -1.0*numeric_limits<float>::infinity();
    float maxZ = -1.0*numeric_limits<float>::infinity();

    
    // Busquem les minimes coordenades per cada component
    for(vec3 v : this->vertexs) {
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

// Indexacio dels plans a les cares de la malla
void Mesh::makeTriangles() {
    if (plans.size() > 0) plans.clear();

    // O(F²), F nº cares de la mesh. Trigara en carregar l'escena pero estalviem espai
    for (shared_ptr<Face> f : cares) {
        vec3 v1 = vertexs.at(f->idxVertices[0]);
        vec3 normal = normalize(cross(vertexs.at(f->idxVertices[1])-v1, vertexs.at(f->idxVertices[2])-v1));
        float D = -dot(normal, v1);

        // Index del pla al vector plans
        bool keep = true;
        int i = 0;
        while (i < plans.size() && keep) {
            shared_ptr<Plane> p = plans.at(i);
            if (p->isNormal(normal) && p->isD(D)) {
                keep = false;
            } else {
                i++;
            }
        }
        f->idxPlane = i;

        if (keep) {
            // Nou pla: l'afegim al vector
            plans.push_back(make_shared<Plane>(normal,v1,D,material));  
        }
    }
}

bool Mesh::hit(Ray &raig, float tmin, float tmax) const {
    // Opcio de mostrar bounding volumes activada
    if (Controller::getInstance()->getSetUp()->getBoundingVolumes()) {
        return aabb->hit(raig,tmin,tmax);
    }

    // Mirem primer si interseca amb la aabb
    Ray aux2(raig.getOrigin(), raig.getDirection(), tmin, tmax);
    if(!aabb->hit(aux2, tmin, tmax)) {
        return false;
    }

    // Intersecció amb els triangles de la malla
    Ray aux(raig.getOrigin(), raig.getDirection(), tmin, tmax);
    bool any = false;
    for (shared_ptr<Face> f : cares) {
        any |= f->hit(vertexs, plans, aux, tmin, tmax);
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

    // Mirem primer si interseca amb la aabb
    Ray aux(raig.getOrigin(), raig.getDirection(), tmin, tmax);
    if (!aabb->allHits(aux, tmin, tmax)) {
        return false;
    }

    bool any;
    if (Controller::getInstance()->getSetUp()->getBoundingVolumes()) {
        // Opcio de mostrar bounding volumes activada
        any = true;
        for (shared_ptr<HitRecord> h : aux.getHitRecords()) {
            raig.insertHit(h);
        }
    } else {
        any = false;
    }

    // Intersecció amb els triangles de la malla
    for (shared_ptr<Face> f : cares) {
        any |= f->allHits(vertexs, plans, raig, tmin, tmax);
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
                        vertexs.push_back(vec3(lineParts.at(1).toFloat(),
                                               lineParts.at(2).toFloat(),
                                               lineParts.at(3).toFloat()));
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
                        shared_ptr<Face> face = make_shared<Face>();

                        // get points from v array
                        face->idxVertices.push_back(lineParts.at(1).split("/").at(0).toInt() - 1);
                        face->idxVertices.push_back(lineParts.at(2).split("/").at(0).toInt() - 1);
                        face->idxVertices.push_back(lineParts.at(3).split("/").at(0).toInt() - 1);

                        cares.push_back(face);
                    }

                }
            }
            file.close();
            makeTriangles();
            makeBoundingVolumes();
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



