#include "SceneFactoryVirtual.hh"


SceneFactoryVirtual::SceneFactoryVirtual():SceneFactory()
{

}

SceneFactoryVirtual::SceneFactoryVirtual(QString filename) {
    createScene(filename);
}

shared_ptr<Scene> SceneFactoryVirtual::createScene() {
    scene= make_shared<Scene>();
    // Pots posar el codi aqui per generar una escena de forma procedimental
    int n_spheres = 5;
    vec3 positions[] = {
        vec3(-0.7, 0.0, 0.0),
        vec3(0.7, 0.0, 0.0),
        vec3( 0.0, 0.0, 0.0 ),
        vec3(-1.0, -1.0, 0.0),        
        vec3(1.0, -1.0, 0.0)
    };

    float radius[] = {
        0.1f,
        0.1f,
        1.0f,
        0.5f,
        0.5f,
    };

    shared_ptr<Lambertian> mats[] = {
        make_shared<Lambertian>(vec3(215/255.,72/255.,148/255.)),
        make_shared<Lambertian>(vec3(215/255.,72/255.,148/255.)),
        make_shared<Lambertian>(vec3(247/255.,218/255.,234/255.)),
        make_shared<Lambertian>(vec3(236/255.,85/255.,104/255.)),
        make_shared<Lambertian>(vec3(236/255.,85/255.,104/255.)),

    };

    for (size_t i = 0; i < n_spheres ; i++)
    {
        auto sphere = make_shared<Sphere>(positions[i], radius[i]);
        sphere->setMaterial(mats[i]);
        scene->objects.push_back(sphere);    
    }
    
    return scene;
}

shared_ptr<Scene> SceneFactoryVirtual::createScene(QString filename) {

    scene= make_shared<Scene>();
    load(filename);
    print(0);

    return scene;
}


void SceneFactoryVirtual::OneSphere(shared_ptr<Scene> s, vec3 position, float radius) {
    auto sphere = make_shared<Sphere>(position, radius);
    sphere->setMaterial(make_shared<Lambertian>(vec3(0.5, 0.2, 0.7)));
    s->objects.push_back(sphere);

}

//! [3]
bool SceneFactoryVirtual::load(QString nameFile)
{
    QFile loadFile(nameFile);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open the virtual scene file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonParseError error;
    QJsonDocument loadDoc( QJsonDocument::fromJson(saveData, &error));


    if (loadDoc.isNull()) {
        qWarning("Parse error in json virtual scene file.");
        return false;
    }
    QJsonObject object = loadDoc.object();
    read(object);

    QTextStream(stdout) << "Loaded virtual scene using " << "...\n";
    return true;
}
//! [3]

//! [4]
bool SceneFactoryVirtual::save(QString nameFile) const
{
    QFile saveFile(QString(nameFile.append(".json")));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open virtual scene file.");
        return false;
    }

    QJsonObject setUpObject;
    write(setUpObject);
    saveFile.write(QJsonDocument(setUpObject).toJson());

    return true;
}

//! [0]
void SceneFactoryVirtual::read(const QJsonObject &json)
{
    if (json.contains("scene") && json["scene"].isString())
        scene->name = json["scene"].toString();
    if (json.contains("typeScene") && json["typeScene"].isString())
        currentType = getSceneFactoryType(json["typeScene"].toString());

    if (json.contains("objects") && json["objects"].isArray()) {
        QJsonArray objectsArray = json["objects"].toArray();

        for (int objectIndex = 0; objectIndex < objectsArray.size(); objectIndex++) {
            QJsonObject objectObject = objectsArray[objectIndex].toObject();
            shared_ptr<Object> o;
            if (objectObject.contains("type") && objectObject["type"].isString()) {
                QString objStr = objectObject["type"].toString().toUpper();
                o = ObjectFactory::getInstance().createObject(ObjectFactory::getInstance().getObjectType(objStr));
                o->read(objectObject);
                scene->objects.push_back(o);
            }
        }
    }

}
//! [0]

void SceneFactoryVirtual:: write(QJsonObject &json) const
{
   json["scene"] = scene->name;
   json["typeScene"] = SceneFactory::getNameType(currentType);


    QJsonArray objectsArray;
    for (const shared_ptr<Object> &o : scene->objects) {
        QJsonObject objectObject;
        int  value = ObjectFactory::getInstance().getIndexType (o);
        QString className = ObjectFactory::getInstance().getNameType(value);

        o->write(objectObject);
        objectObject["type"] = className;
        objectsArray.append(objectObject);
    }
    json["objects"] = objectsArray;
}

void SceneFactoryVirtual::print(int indentation) const
{
    const QString indent(indentation * 2, ' ');
    QTextStream(stdout) << indent << "scene:\t" << scene->name << "\n";
    QTextStream(stdout) << indent << "typeScene:\t" << getNameType(currentType) << "\n";

    QTextStream(stdout) << indent << "Objects:\t\n";
    for (unsigned int i =0; i< scene->objects.size(); i++) {
        auto value = ObjectFactory::getInstance().getIndexType (scene->objects[i]);
        QString className = ObjectFactory::getInstance().getNameType(value);
        QTextStream(stdout) << indent+2 << "name:\t" << scene->objects[i]->name <<"\n";
        QTextStream(stdout) << indent+2 << "type:\t" << className << "\n";
        scene->objects[i]->print(2);
    }
 }
