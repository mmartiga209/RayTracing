#include "Triangle.hh"

Triangle::Triangle() {
    // Triangle per defecte (normal: (0,-1,0))
    Triangle(vec3(0.0,0.0,0.0),vec3(1.0,0.0,0.0),vec3(0.0,0.0,1.0));
}

Triangle::Triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3)  {
    // Sentit antihorari
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;

    normal = (normalize(cross(v2-v1, v3-v1)));
}

bool Triangle::hit(Ray &raig, float tmin, float tmax) const {
    vec3 vp = raig.getDirection();

    // Si el raig i el normal del triangle són ortogonals, no hi ha intersecció
    if(abs(dot(vp, normal))<DBL_EPSILON){
        return false;
    }

    // En els altres casos hi haurà interseccio (si estem en el rang de min/max).
    // Cal omplir la informació del hit.

    // PLA: Ax+By+Cz+D=0
    // on A,B,C = normal

    // 1) Calculem la D = -Ax-By-Cz
    float d = -normal[0]*v1[0] - normal[1]*v1[1] - normal[2]*v1[2];

    // 2) Imposem que la recta p+tv compleixi l'eq del pla
    // A(p1 + t*v1) + ... + D = 0
    // Aillem la t
    vec3 rp = raig.getOrigin();
    float temp = -normal[0]*rp[0] - normal[1]*rp[1] - normal[2]*rp[2] - d;
    temp /= normal[0]*vp[0] + normal[1]*vp[1] + normal[2]*vp[2];

    // Retornem false si no estem en el rang demanat
    if (temp > tmax || temp < tmin) {
        return false;
    }    

    // Comprovacio interseccio dins el triangle
    vec3 p = raig.pointAt(temp);

    // Coordenades baricentriques (no divisio per 2, es cancel·la amb el quocient)
    float div = 1 / length(cross(v2 - v1, v3 - v1));
    float v = length(cross(v2 - v1, p - v1)) * div;
    float u = length(cross(p - v1, v3 - v1)) * div;
    float w = length(cross(v2 - p, v3 - p)) * div;

    // 0 < u,v,w < 1
    if (0 >= u || 0 >= v || 0 >= w || u >= 1 || v >= 1 || w >= 1) {
        return false;
    }

    // Si la suma de les coordenades baricentriques es 1, estem dins el triangle
    if (abs(1 - (u+v+w)) < DBL_EPSILON) {
        shared_ptr<HitRecord> info = make_shared<HitRecord>();

        // Omplim el camp de info:
        info->t = temp;
        info->p = p;
        // La normal a un triangle es la mateixa per tots els punts
        info->normal = normal;
        info->mat = material;
        raig.addHit(info);
        return true;
    }
    
    // No està dins el triangle
    return false;
}

// Mateixa estrategia que hit(), usant raig.insertHit()
bool Triangle::allHits(Ray& raig, float tmin, float tmax) const {
    vec3 vp = raig.getDirection();

    // Si el raig i el normal del triangle són ortogonals, no hi ha intersecció
    if(abs(dot(vp, normal))<DBL_EPSILON){
        return false;
    }

    // En els altres casos hi haurà interseccio (si estem en el rang de min/max).
    // Cal omplir la informació del hit.

    // PLA: Ax+By+Cz+D=0
    // on A,B,C = normal

    // 1) Calculem la D = -Ax-By-Cz
    float d = -normal[0]*v1[0] - normal[1]*v1[1] - normal[2]*v1[2];

    // 2) Imposem que la recta p+tv compleixi l'eq del pla
    // A(p1 + t*v1) + ... + D = 0
    // Aillem la t
    vec3 rp = raig.getOrigin();
    float temp = -normal[0]*rp[0] - normal[1]*rp[1] - normal[2]*rp[2] - d;
    temp /= normal[0]*vp[0] + normal[1]*vp[1] + normal[2]*vp[2];

    // Retornem false si no estem en el rang demanat
    if (temp > tmax || temp < tmin) {
        return false;
    }    

    // Comprovacio interseccio dins el triangle
    vec3 p = raig.pointAt(temp);

    // Coordenades baricentriques (no divisio per 2, es cancel·la amb el quocient)
    float div = 1 / length(cross(v2 - v1, v3 - v1));
    float v = length(cross(v2 - v1, p - v1)) * div;
    float u = length(cross(p - v1, v3 - v1)) * div;
    float w = length(cross(v2 - p, v3 - p)) * div;

    // 0 < u,v,w < 1
    if (0 >= u || 0 >= v || 0 >= w || u >= 1 || v >= 1 || w >= 1) {
        return false;
    }

    // Si la suma de les coordenades baricentriques es 1, estem dins el triangle
    if (abs(1 - (u+v+w)) < DBL_EPSILON) {
        shared_ptr<HitRecord> info = make_shared<HitRecord>();

        // Omplim el camp de info:
        info->t = temp;
        info->p = p;
        // La normal a un triangle es la mateixa per tots els punts
        info->normal = normal;
        info->mat = material;
        raig.insertHit(info);
        return true;
    }
    
    // No està dins el triangle
    return false;
}

void Triangle::update(int frame) {
}

void Triangle::aplicaTG(shared_ptr<TG> t) {

}

void Triangle::read (const QJsonObject &json)
{
    Object::read(json);

    if (json.contains("v1") && json["v1"].isArray()) {
        QJsonArray auxVec = json["v1"].toArray();
        v1[0] = auxVec[0].toDouble();
        v1[1] = auxVec[1].toDouble();
        v1[2] = auxVec[2].toDouble();
    }
    if (json.contains("v2") && json["v2"].isArray()) {
        QJsonArray auxVec = json["v2"].toArray();
        v2[0] = auxVec[0].toDouble();
        v2[1] = auxVec[1].toDouble();
        v2[2] = auxVec[2].toDouble();
    }
    if (json.contains("v3") && json["v3"].isArray()) {
        QJsonArray auxVec = json["v3"].toArray();
        v3[0] = auxVec[0].toDouble();
        v3[1] = auxVec[1].toDouble();
        v3[2] = auxVec[2].toDouble();
    }

    normal = (normalize(cross(v2-v1, v3-v1)));

}


//! [1]
void Triangle::write(QJsonObject &json) const
{
    Object::write(json);

    QJsonArray auxArray;
    auxArray.append(v1[0]);
    auxArray.append(v1[1]);
    auxArray.append(v1[2]);
    json["v1"] = auxArray;

    auxArray = QJsonArray();
    auxArray.append(v2[0]);
    auxArray.append(v2[1]);
    auxArray.append(v2[2]);
    json["v2"] = auxArray;

    auxArray = QJsonArray();
    auxArray.append(v3[0]);
    auxArray.append(v3[1]);
    auxArray.append(v3[2]);
    json["v3"] = auxArray;

}
//! [1]

void Triangle::print(int indentation) const
{
    Object::print(indentation);

    const QString indent(indentation * 2, ' ');

    QTextStream(stdout) << indent << "vertice 1:\t" << v1[0] << ", "<< v1[1] << ", "<< v1[2] << "\n";
    QTextStream(stdout) << indent << "vertice 2:\t" << v2[0] << ", "<< v2[1] << ", "<< v2[2] << "\n";
    QTextStream(stdout) << indent << "vertice 3:\t" << v3[0] << ", "<< v3[1] << ", "<< v3[2] << "\n";
}

