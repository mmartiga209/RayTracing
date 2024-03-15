#include "Box.hh"

Box::Box() {
    min=vec3(0.0,0.0,0.0);
    max=vec3(0.0,0.0,0.0);
}

Box::Box(const vec3 &min, const vec3 &max)  {
    // Each component in min must be lower or equal than that in max
    this->min = min;
    this->max = max;
}

// Smits' method
bool Box::hit(Ray &raig, float tmin, float tmax) const {
    // NOTA: Per considerar la interseccio cal que tant la 1a com la 2a estiguin en (tmin,tmax)
    float min_inter = tmin, max_inter = tmax;
    int normal_comp;
    vec3 direction = raig.getDirection();
    vec3 origin = raig.getOrigin();

    // Obtencio interseccio minima i maxima tractant components
    for (int i=0; i<3; i++) {
        float t0, t1, div = 1 / direction[i];

        // Valor minim i maxim de t en aquesta component
        if (div >= 0) {
            t0 = (min[i] - origin[i]) * div;
            t1 = (max[i] - origin[i]) * div;
        } else {
            t0 = (max[i] - origin[i]) * div;
            t1 = (min[i] - origin[i]) * div;
        }

        // Interseccio impossible
        if ((min_inter > t1) || (t0 > max_inter))
            return false;
        
        // Actualitzacio minim/maxim
        if (t0 > min_inter) {
            min_inter = t0;
            normal_comp = i;
        }
        if (t1 < max_inter)
            max_inter = t1;
    }

    // Validacio final de t
    if ((tmin < min_inter) && (max_inter < tmax)) {
        shared_ptr<HitRecord> hit = make_shared<HitRecord>();
        hit->t = min_inter;
        hit->p = raig.pointAt(hit->t);
        hit->mat = material;

        // Normal i-essim vector base canonica, i component interseccio que dona la t
        vec3 temp(0.0);
        temp[normal_comp] = -direction[normal_comp]; // Direccio oposada al raig en aquella component
        hit->normal = normalize(temp);

        // Afegim al raig el hit mes proper
        raig.addHit(hit);
        return true;
    } else {
        return false;
    }
}

bool Box::allHits(Ray& raig, float tmin, float tmax) const {
    // NOTA: Per considerar les interseccions cal que tant la 1a com la 2a estiguin en (tmin,tmax)
    float min_inter = tmin, max_inter = tmax;
    int normal_comp_min, normal_comp_max;
    vec3 direction = raig.getDirection();
    vec3 origin = raig.getOrigin();

    // Obtencio interseccio minima i maxima tractant components
    for (int i=0; i<3; i++) {
        float t0, t1, div = 1 / direction[i];

        // Valor minim i maxim de t en aquesta component
        if (div >= 0) {
            t0 = (min[i] - origin[i]) * div;
            t1 = (max[i] - origin[i]) * div;
        } else {
            t0 = (max[i] - origin[i]) * div;
            t1 = (min[i] - origin[i]) * div;
        }

        // Interseccio impossible
        if ((min_inter > t1) || (t0 > max_inter))
            return false;
        
        // Actualitzacio minim/maxim
        if (t0 > min_inter) {
            min_inter = t0;
            normal_comp_min = i;
        }
        if (t1 < max_inter) {
            max_inter = t1;
            normal_comp_max = i;
        }
    }

    // Validacio final de t
    if ((tmin < min_inter) && (max_inter < tmax)) {
        // Interseccio propera
        shared_ptr<HitRecord> hit = make_shared<HitRecord>();
        hit->t = min_inter;
        hit->p = raig.pointAt(hit->t);
        hit->mat = material;

        // Normal i-essim vector base canonica, i component interseccio que dona la t
        vec3 temp(0.0);
        temp[normal_comp_min] = -direction[normal_comp_min]; // Sentit invers raig en la component
        hit->normal = normalize(temp);

        // Afegim el hit
        raig.insertHit(hit);

        // Interseccio llunyana
        hit = make_shared<HitRecord>();
        hit->t = max_inter;
        hit->p = raig.pointAt(hit->t);
        hit->mat = material;

        // Normal i-essim vector base canonica, i component interseccio que dona la t
        temp = vec3(0.0);
        temp[normal_comp_max] = direction[normal_comp_max]; // Sentit del raig en la component
        hit->normal = normalize(temp);

        // Afegim el hit
        raig.insertHit(hit);

        return true;
    } else {
        return false;
    }
}

void Box::update(int frame) {
    // TO DO: Cal ampliar-lo per a fer el update de la box
}

void Box::aplicaTG(shared_ptr<TG> t) {
}

void Box::read (const QJsonObject &json) {
    Object::read(json);

    if (json.contains("punt_min") && json["punt_min"].isArray()) {
        QJsonArray auxVec = json["punt_min"].toArray();
        min[0] = auxVec[0].toDouble();
        min[1] = auxVec[1].toDouble();
        min[2] = auxVec[2].toDouble();
    }
    if (json.contains("punt_max") && json["punt_max"].isArray()) {
        QJsonArray auxVec = json["punt_max"].toArray();
        max[0] = auxVec[0].toDouble();
        max[1] = auxVec[1].toDouble();
        max[2] = auxVec[2].toDouble();
    }
}


//! [1]
void Box::write(QJsonObject &json) const {
    Object::write(json);

    QJsonArray auxArray;
    auxArray.append(min[0]);auxArray.append(min[1]);auxArray.append(min[2]);
    json["punt_min"] = auxArray;

    QJsonArray auxArray2;
    auxArray2.append(max[0]);auxArray2.append(max[1]);auxArray2.append(max[2]);
    json["punt_max"] = auxArray2;
}
//! [1]

void Box::print(int indentation) const {
    Object::print(indentation);

    const QString indent(indentation * 2, ' ');

    QTextStream(stdout) << indent << "Min_point:\t" << min[0] << ", "<< min[1] << ", "<< min[2] << "\n";
    QTextStream(stdout) << indent << "Max_point:\t" << max[0] << ", "<< max[1] << ", "<< max[2] << "\n";
}

