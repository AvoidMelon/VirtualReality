#include "Mesh.h"
#include <GL/freeglut.h>

LIB_API Mesh::Mesh() :m_has_shadows{true} {
    matrix_shadow = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
}

LIB_API Mesh::~Mesh(){

}

void LIB_API Mesh::material(std::shared_ptr<Material> material){
    m_material = material;
}

std::shared_ptr<Material LIB_API> Mesh::material() const{
    return m_material;
}

void LIB_API Mesh::LOD(int LOD) {
    m_LOD = LOD;
}

int LIB_API Mesh::LOD() const {
    return m_LOD;
}

void LIB_API Mesh::vertices(LODdata vertices)
{
    m_vertices = std::make_shared<LODdata>(vertices);
    int lods = 0;
    auto m_lod_vertices = &m_vertices->lod.at(lods).vertices;
    auto m_lod_normal = &m_vertices->lod.at(lods).normal;
    auto m_lod_uv = &m_vertices->lod.at(lods).uv;
    auto m_lod_faces = &m_vertices->lod.at(lods).faces;
    unsigned int** faces = new unsigned int*[m_lod_faces->size()];
    for (int i = 0; i < m_lod_faces->size(); i++) {
        faces[i] = new unsigned int[3];
        for (int j = 0; j < 3; j++) {
            faces[i][j] = m_lod_faces->at(i).at(j);
        }
<<<<<<< HEAD
<<<<<<< HEAD
    }*/
=======
    }
>>>>>>> parent of 0c66d09 (try but not work)
    /*
=======
    }
    /*glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
>>>>>>> parent of 215b7fe (Comments vbo try)
    glGenBuffers(1, &faceVbo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVbo);
    // Copy the face index data from system to video memory:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        m_lod_faces->size() * 3 * sizeof(unsigned int),
        faces, GL_STATIC_DRAW);

    glGenBuffers(1, &vertexVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    // Copy the vertex data from system to video memory:
    glBufferData(GL_ARRAY_BUFFER, m_lod_vertices->size() * sizeof(glm::vec3),
        &m_lod_vertices[0], GL_STATIC_DRAW);


    glGenBuffers(1, &normal);
    glBindBuffer(GL_ARRAY_BUFFER, normal);
    // Copy the normal data from system to video memory:
    glBufferData(GL_ARRAY_BUFFER, m_lod_normal->size() * sizeof(glm::vec3),
        &m_lod_normal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uv);
    glBindBuffer(GL_ARRAY_BUFFER, uv);
    // Copy the uv data from system to video memory:
    glBufferData(GL_ARRAY_BUFFER, m_lod_uv->size() * sizeof(glm::vec2),
        &m_lod_uv[0], GL_STATIC_DRAW);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/

}

LODdata LIB_API Mesh::vertices() const
{
    return *m_vertices;
}

void LIB_API Mesh::render(std::shared_ptr<Object> camera){
    int lods = 0;
    
    //Prendo matrice camera
    glm::mat4 model_view;
    model_view = dynamic_cast<Camera*>(camera.get())->inverseCamera() * getFinalMatrix();
    glLoadMatrixf(glm::value_ptr(model_view));
    
    if (m_material) {
        m_material->render(camera);
    }
    auto m_lod_vertices = &m_vertices->lod.at(lods).vertices;
    auto m_lod_normal = &m_vertices->lod.at(lods).normal;
    auto m_lod_uv = &m_vertices->lod.at(lods).uv;
    auto m_lod_faces = &m_vertices->lod.at(lods).faces;
    auto size = m_lod_faces->size();
    //itera vertici
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < size; j++) {
        //prende tutti i vertici della faccia j
        auto m_lod_faces_0 = m_lod_faces->at(j).at(0);
        auto m_lod_faces_1 = m_lod_faces->at(j).at(1);
        auto m_lod_faces_2 = m_lod_faces->at(j).at(2);
        glNormal3fv(glm::value_ptr(m_lod_normal->at(m_lod_faces_0)));
        glTexCoord2fv(glm::value_ptr(m_lod_uv->at(m_lod_faces_0)));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_0)));
        glNormal3fv(glm::value_ptr(m_lod_normal->at(m_lod_faces_1)));
        glTexCoord2fv(glm::value_ptr(m_lod_uv->at(m_lod_faces_1)));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_1)));
        glNormal3fv(glm::value_ptr(m_lod_normal->at(m_lod_faces_2)));
        glTexCoord2fv(glm::value_ptr(m_lod_uv->at(m_lod_faces_2)));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_2)));
    }
    glEnd();
    if (shadow()) {
        glm::mat4 matrix_shadow_new = matrix_shadow * getFinalMatrix();
        
        //view*model
        matrix_shadow_new = dynamic_cast<Camera*>(camera.get())->inverseCamera() * matrix_shadow_new;
        glLoadMatrixf(glm::value_ptr(matrix_shadow_new));
        render_shadow();
    }
}

void LIB_API Mesh::render_shadow(){
    int lods = 0;
    
    glm::vec3 color_shadow;
    color_shadow = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    auto m_lod_vertices = &m_vertices->lod.at(lods).vertices;
    auto m_lod_faces = &m_vertices->lod.at(lods).faces;

    auto size = m_lod_faces->size();
    
    //Renderizzo l'ombra
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < size; j++) {
        glColor3fv(glm::value_ptr(color_shadow));
       
        auto m_lod_faces_j = m_lod_faces->at(j);
        
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_j.at(0))));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_j.at(1))));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_j.at(2))));
    }
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void LIB_API Mesh::shadow(const bool& shadow)
{
    m_has_shadows = shadow;
}

bool LIB_API Mesh::shadow() const
{
    return m_has_shadows;
}

void LIB_API Mesh::VAO(int vao)
{
    m_VAO = vao;
}

int LIB_API Mesh::VAO() const
{
    return m_VAO;
}

Mesh LIB_API* Mesh::clone()
{
    return new Mesh(*this);
}
