#include "NormalShading.hh"


vec3 NormalShading::shading(shared_ptr<Scene> scene, vector<shared_ptr<HitRecord>> infos, vec3 lookFrom, 
                            vec3 globalLight, vector<shared_ptr<Light>> lights, bool shadow)
{
    if (!infos.empty()) {
        // En cas que no sigui buit ens hem de fixar si estem tenint en compte les ombres o no
        if(shadow){
            // Si per configuració tenim les ombres activades mirarem si el punt està a l'ombra
            float shadowFactor;
            auto m = infos[0]->mat;
            auto p = infos[0]->p;

            // Bool per controlar si és ombra o no
            bool isShadow = false;
            vec3 color = vec3(0.0f);

            // Component ambient del material
            vec3 kaMaterial = m->Ka;

            // Suma de la llum global
            color += kaMaterial * globalLight;

            for (shared_ptr<Light> l : lights) {
                shadowFactor = computeShadow(l, p, scene);    // Retorna 0.0 si està a l'ombra, sinó 1.0
                if (glm::abs(shadowFactor) < DBL_EPSILON) {
                    // Si està a l'ombra sumem la component ambient de la llum per la del material
                    color += kaMaterial * l->getIa();
                    isShadow = true;
                }
            }

            if(isShadow){
                return color;
            } else {
                // Si no està en cap ombra retornem la normal
                return 0.5f * (infos[0]->normal + 1.0f);
            }

        } else { 
            // Si no estem tenint en compte les ombres retornem el color de la normal
            return 0.5f * (infos[0]->normal + 1.0f);
        }
    } else {
        // Tracta la situació on el vector infos és buit
        // Retorna un valor predeterminat o maneja-ho segons les teves necessitats.
        return vec3(0.0f);
    }
}
