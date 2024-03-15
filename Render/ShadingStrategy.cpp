#include "ShadingStrategy.hh"

float ShadingStrategy::computeShadow(shared_ptr<Light> light, vec3 point, shared_ptr<Scene> scene) {
    // Hem de calcular si el punt és a l'ombra o no
    // El punt en qüestió és point i diposem de la llum light
    // Per a calcular-ho obtindrem el raig que passa per la llum i pel punt
    // Comprovarem si hi ha alguna intersecció amb algun objecte de l'escena abans d'arribar a la llum
    
    vec3 vectorL = light->vectorL(point);
    Ray shadowRay(point, vectorL);
    float tmax = light->distanceToLight(point);

    // tmax marca la distància entre el punt i la llum i ens servirà per a calcular si hi ha alguna intersecció
    // Tmin és per defecte 0.01 i així ja evitem que el raig interseci amb el mateix objecte (shadow acne)
    if (scene->hit(shadowRay, shadowRay.getTmin(), tmax)) {
        return 0.0f;
    }
    else{
        return 1.0f;
    }
}