#include "obj_loader.h"

void ObjLoader::read_file (const std::string &file_name)
{
    
    ofxAssimpModelLoader model;
    bool success = model.loadModel(file_name);
    
    if(success) {
        std::cout << "[ObjLoader] Loaded model in " << file_name << std::endl;
        
        
        std::cout << "[ObjLoader] Number of meshes: " << model.getMeshCount() << std::endl;
        
        for(size_t mindex = 0; mindex < model.getMeshCount(); ++mindex) {
            std::vector <ofMeshFace> mfaces = model.getMesh(mindex).getUniqueFaces();
            
            
            
            std::cout << "[ObjLoader] Adding mesh " << mindex << " with " << mfaces.size() << " faces." << std::endl;
            for(size_t i = 0; i < mfaces.size(); ++i) {
                cow_ptr<Face> face (new Face(Face::TRIANGLES));
                
                if(mfaces[i].hasColors())
                    face.touch()->_face_color = mfaces[i].getColor(0);
                else
                    face.touch()->_face_color = scgColor::white;
                for(size_t j = 0; j < 3; ++j) {
                    face.touch()->_vertices.push_back (mfaces[i].getVertex(j));
                    if(mfaces[i].hasColors())
                        face.touch()->_colors.push_back (mfaces[i].getColor(j));
                    if(mfaces[i].hasNormals())
                        face.touch()->_normals.push_back (mfaces[i].getNormal(j));
                    if(mfaces[i].hasTexcoords())
                        face.touch()->_texture_coordinates.push_back (mfaces[i].getTexCoord(j));
                }
                _geometry.touch()->_faces.push_back (face);
            }
        }
    }
    else {
        std::cout << "[ObjLoader] Couldn't load model in " << file_name << std::endl;
    }
}
