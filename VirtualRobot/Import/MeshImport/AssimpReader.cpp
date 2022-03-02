#include <filesystem>

#include "AssimpReader.h"
#include <VirtualRobot/Visualization/TriMeshModel.h>
#include <VirtualRobot/ManipulationObject.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>

#include <Inventor/SbImage.h>

namespace
{
    bool readScene(
        const aiScene* scene,
        const VirtualRobot::TriMeshModelPtr& t,
        const std::string& filename,
        const VirtualRobot::AssimpReader::Parameters& param,
        VirtualRobot::AssimpReader::ResultMetaData& meta
    )
    {
        meta.loadingSuccessful = false;
        meta.regeneratedNormals = false;
        meta.skippedFaces = 0;
        if (!t)
        {
            VR_ERROR << "Tri mesh is null\n";
            return false;
        }
        if (!scene)
        {
            VR_ERROR << "unable to load the file '" << filename << "'\n";
            return false;
        }
        if (!scene->mRootNode)
        {
            VR_ERROR << "mesh from '" << filename << "' has no root node\n";
            return false;
        }
        if (scene->mNumMeshes == 0)
        {
            VR_ERROR << "mesh from '" << filename << "' has no mesh\n";
            return false;
        }
        if (scene->mNumMeshes > 1 && !param.mergeMultipleMeshes)
        {
            VR_ERROR << "mesh from '" << filename << "' has not exactly one mesh"
                     << " It has " << scene->mNumMeshes << " meshes\n";
            return false;
        }
        if (!scene->mMeshes[0])
        {
            VR_ERROR << "mesh from '" << filename << "' is null\n";
            return false;
        }

        t->clear();
        for (std::size_t idxMesh = 0; idxMesh < scene->mNumMeshes; ++idxMesh)
        {
#define error_log VR_ERROR << "mesh[" << idxMesh + 1 << " / "               \
                           << scene->mNumMeshes << "] from '" << filename << "'"

            const long vertexIdxOffset = t->vertices.size();
            const aiMesh& m = *(scene->mMeshes[idxMesh]);
            if (!(m.mVertices && m.mNumVertices))
            {
                error_log << " has no vertices\n";
                return false;
            }
            if (!(m.mFaces && m.mNumFaces))
            {
                error_log << " has no vertices\n";
                return false;
            }
            if (!m.mNormals)
            {
                if (!param.ignoreMissingNormals)
                {
                    error_log << " has no normals (and none were generated when loading it)\n";
                    return false;
                }
                if (param.verbose)
                {
                    error_log << " has no normals (and none were generated when loading it) (ignoring this)\n";
                }
                meta.regeneratedNormals = true;
            }

            for (unsigned i = 0; i < m.mNumVertices; ++i)
            {
                const auto& v = m.mVertices[i];
                t->addVertex({v.x, v.y, v.z});
                if (m.mNormals)
                {
                    const auto& n = m.mNormals[i];
                    t->addNormal(Eigen::Vector3f{n.x, n.y, n.z}.normalized());
                }
            }
            for (unsigned i = 0; i < m.mNumFaces; ++i)
            {
                const auto& f = m.mFaces[i];
                if (f.mNumIndices != 3)
                {
                    if (param.verbose || !param.skipInvalidFaces)
                        error_log << " has face (# " << i
                                  << ") with the wrong number of vertices ("
                                  << f.mNumIndices << ")\n";
                    if (param.skipInvalidFaces)
                    {
                        ++meta.skippedFaces;
                        continue;
                    }

                    return false;
                }
                if (
                    f.mIndices[0] >= m.mNumVertices ||
                    f.mIndices[1] >= m.mNumVertices ||
                    f.mIndices[2] >= m.mNumVertices
                )
                {
                    error_log << " has vertex index out of bounds for face # " << i
                              << " \n";
                    return false;
                }
                VirtualRobot::MathTools::TriangleFace fc;
                fc.id1 = vertexIdxOffset + f.mIndices[0];
                fc.id2 = vertexIdxOffset + f.mIndices[1];
                fc.id3 = vertexIdxOffset + f.mIndices[2];
                fc.idNormal1 = vertexIdxOffset + f.mIndices[0];
                fc.idNormal2 = vertexIdxOffset + f.mIndices[1];
                fc.idNormal3 = vertexIdxOffset + f.mIndices[2];
                t->addFace(fc);
            }
#undef error_log
        }

        t->scale(param.scaling);
        t->mergeVertices(param.eps);
        t->addMissingNormals();
        meta.loadingSuccessful = true;
        return true;
    }
}

namespace VirtualRobot
{
    AssimpReader::AssimpReader(float eps, float scaling)
    {
        parameters.scaling = scaling;
        parameters.eps = eps;
    }

    std::string AssimpReader::get_extensions()
    {
        static const auto extensions = []
        {
            const std::string upper =
            "3D 3DS 3MF AC AC3D ACC AMJ ASE ASK B3D BLEND BVH CMS COB "  \
            "DAE DXF ENFF FBX LWO LWS LXO MD2 MD3 MD5 MDC MDL MESH MOT " \
            "MS3D NDO NFF OBJ OFF OGEX PLY PMX PRJ Q3O Q3S RAW SCN SIB " \
            "SMD STP STL STLA STLB TER UC VTA X X3D XGL ZGL";
            std::string both = upper + upper;
            std::transform(
                both.begin(), both.begin() + upper.size(), both.begin(),
                [](unsigned char c)
            {
                return std::tolower(c);
            }
            );
            return both;
        }();
        return extensions;
    }
    bool AssimpReader::can_load(const std::string& file)
    {
        std::filesystem::path p{file};
        if (!p.has_extension())
        {
            return false;
        }
        std::string ext = p.extension().string().substr(1);
        for (auto& c : ext)
        {
            c = std::toupper(c);
        }
        if (ext.empty())
        {
            return false;
        }
        return get_extensions().find(ext) != std::string::npos;
    }

    bool AssimpReader::readFileAsTriMesh(const std::string& filename, const TriMeshModelPtr& t)
    {
        //code adapted from http://assimp.sourceforge.net/lib_html/usage.html
        Assimp::Importer importer;
        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll
        // propably to request more postprocessing than we do in this example.
        const aiScene* scene = importer.ReadFile(
                                   filename,
                                   aiProcess_JoinIdenticalVertices    |
                                   aiProcess_Triangulate              |
                                   aiProcess_GenSmoothNormals         |
                                   aiProcess_SortByPType
                               );
        return readScene(scene, t, filename, parameters, resultMetaData);
    }
    bool AssimpReader::readBufferAsTriMesh(const std::string_view& v, const TriMeshModelPtr& t)
    {
        //code adapted from http://assimp.sourceforge.net/lib_html/usage.html
        Assimp::Importer importer;
        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll
        // propably to request more postprocessing than we do in this example.
        const aiScene* scene = importer.ReadFileFromMemory(
                                   v.data(), v.size(),
                                   aiProcess_JoinIdenticalVertices    |
                                   aiProcess_Triangulate              |
                                   aiProcess_GenSmoothNormals         |
                                   aiProcess_SortByPType
                               );
        return readScene(scene, t, "<MEMORY_BUFFER>", parameters, resultMetaData);
    }

    TriMeshModelPtr AssimpReader::readFileAsTriMesh(const std::string& filename)
    {
        auto tri = std::make_shared<TriMeshModel>();
        if (!readFileAsTriMesh(filename, tri))
        {
            return nullptr;
        }
        return tri;
    }

    TriMeshModelPtr AssimpReader::readBufferAsTriMesh(const std::string_view& v)
    {
        auto tri = std::make_shared<TriMeshModel>();
        if (!readBufferAsTriMesh(v, tri))
        {
            return nullptr;
        }
        return tri;
    }

    static void addPerVertexColorMaterial(aiColor4D* colors, unsigned int numColors, SoSeparator* result)
    {
        SoMaterialBinding* materialBinding = new SoMaterialBinding;
        materialBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
        result->addChild(materialBinding);

        SoMaterial* materialNode = new SoMaterial;
        materialNode->diffuseColor.setNum(numColors);

        SbColor* diffuseData = materialNode->diffuseColor.startEditing();

        for (unsigned int i = 0; i < numColors; ++i)
        {
            aiColor4D color = colors[i];
            // Cannot define alpha value per vertex in Coin!
            diffuseData[i].setValue(color.r, color.g, color.b);
        }

        materialNode->diffuseColor.finishEditing();
        materialNode->ambientColor = materialNode->diffuseColor;
        materialNode->specularColor = materialNode->diffuseColor;

        result->addChild(materialNode);
    }

    static void addTextureMaterial(aiMaterial* material, aiTextureType textureType,
                                   std::filesystem::path const& meshPath,
                                   SoSeparator* result)
    {
        aiString path;
        aiTextureMapping mapping;
        unsigned int uvindex = 0;
        ai_real blend = 0.0f;
        aiTextureOp op;
        aiTextureMapMode mapMode[3];
        aiReturn textureOk = material->GetTexture(textureType, 0,
                                                  &path, &mapping, &uvindex, &blend,
                                                  &op, mapMode);
        if (textureOk == aiReturn_SUCCESS)
        {
            SoTexture2* textureNode = new SoTexture2();
            std::filesystem::path texturePath = meshPath.parent_path() / path.C_Str();
            textureNode->filename.set(texturePath.c_str());

            // Texture map mode for the first coordinate (assimp 0, Coin S)
            if (mapMode[0] == aiTextureMapMode_Wrap)
            {
                textureNode->wrapS = SoTexture2::REPEAT;
            }
            else if (mapMode[0] == aiTextureMapMode_Clamp)
            {
                textureNode->wrapS = SoTexture2::CLAMP;
            }
            else
            {
                VR_INFO << "Could not map TextureMapMode " << mapMode[0] << "\n";
            }

            // Texture map mode for the second coordinate (assimp 1, Coin T)
            if (mapMode[1] == aiTextureMapMode_Wrap)
            {
                textureNode->wrapT = SoTexture2::REPEAT;
            }
            else if (mapMode[1] == aiTextureMapMode_Clamp)
            {
                textureNode->wrapT = SoTexture2::CLAMP;
            }
            else
            {
                VR_INFO << "Could not map TextureMapMode " << mapMode[1] << "\n";
            }

            SbImage const& image = textureNode->image.getValue();
            SbVec3s size = image.getSize();

            VR_INFO << "Tried to load image from file '" << texturePath
                    << "', size: " << size[0] << " x " << size[1] << "\n";

            result->addChild(textureNode);
        }
        else
        {
            VR_WARNING << "Could not get texture data: " << textureOk
                       << "In file: " << meshPath << "\n";
        }
    }

    static void addVertices(unsigned int numVertices, aiVector3D* vertices,
                            SoSeparator* result)
    {
        SoCoordinate3* vertexNode = new SoCoordinate3;
        vertexNode->point.setNum(numVertices);
        SbVec3f* vertexData = vertexNode->point.startEditing();
        // Consider memcpy!
        for (unsigned int i = 0; i < numVertices; ++i)
        {
            vertexData[i][0] = vertices[i].x;
            vertexData[i][1] = vertices[i].y;
            vertexData[i][2] = vertices[i].z;
        }
        vertexNode->point.finishEditing();
        result->addChild(vertexNode);
    }

    static void addNormals(unsigned int numNormals, aiVector3D* normals,
                           SoSeparator* result)
    {
        SoNormal* normalNode = new SoNormal;
        normalNode->vector.setNum(numNormals);
        SbVec3f* normalData = normalNode->vector.startEditing();
        // Consider memcpy!
        for (unsigned int i = 0; i < numNormals; ++i)
        {
            normalData[i][0] = normals[i].x;
            normalData[i][1] = normals[i].y;
            normalData[i][2] = normals[i].z;
        }
        normalNode->vector.finishEditing();

        result->addChild(normalNode);

        SoNormalBinding* normalBinding = new SoNormalBinding;
        normalBinding->value = SoNormalBinding::PER_VERTEX_INDEXED;
        result->addChild(normalBinding);
    }

    static void addTextureCoordinates(unsigned int numCoordinates, aiVector3D* coordinates,
                                      SoSeparator* result)
    {
        SoTextureCoordinate2* texCoord = new SoTextureCoordinate2;
        texCoord->point.setNum(numCoordinates);
        SbVec2f* uvData = texCoord->point.startEditing();
        for (unsigned int i = 0; i < numCoordinates; ++i)
        {
            aiVector3D uv = coordinates[i];
            uvData[i][0] = uv[0];
            uvData[i][1] = uv[1];
        }
        texCoord->point.finishEditing();
        result->addChild(texCoord);
    }

    static void addIndexedFaceSet(unsigned int numFaces, aiFace* faces,
                                  SoSeparator* result)
    {
        SoIndexedFaceSet* faceSetNode = new SoIndexedFaceSet;
        faceSetNode->coordIndex.setNum(numFaces * 4);
        int32_t* indexData = faceSetNode->coordIndex.startEditing();
        for (unsigned int i = 0; i < numFaces; ++i)
        {
            unsigned int* indices = faces[i].mIndices;
            indexData[0] = indices[0];
            indexData[1] = indices[1];
            indexData[2] = indices[2];
            //Coin requires -1 as end of face marker
            indexData[3] = -1;

            indexData += 4;
        }
        faceSetNode->coordIndex.finishEditing();

        // Assimp: Coord indices are equivalent with the normal indices
        faceSetNode->normalIndex = faceSetNode->coordIndex;

        result->addChild(faceSetNode);
    }

    SoNode* AssimpReader::readFileAsSoNode(const std::string& filename)
    {
        std::filesystem::path meshPath = filename;

        Assimp::Importer importer;
        // Triangulate, so that we can assume faces with 3 vertices (aiProcess_Triangulate)
        // Generate normals if they are not present so that the subsequent code
        // can assume that per vertex normals exist (aiProcess_GenSmoothNormals)
        // Generate UV coordinates if possible (for primitive shapes like boxes, spheres, ...)
        const aiScene* scene = importer.ReadFile(
                                   filename,
                                   aiProcess_JoinIdenticalVertices |
                                   aiProcess_Triangulate |
                                   aiProcess_GenSmoothNormals |
                                   aiProcess_GenUVCoords |
                                   aiProcess_SortByPType
                                   );

        unsigned int numMeshes = scene->mNumMeshes;
        if (numMeshes < 1)
        {
            VR_INFO << "No mesh found in file: " << filename << "\n";
            return nullptr;
        }

        // We only load the first mesh
        // If there are files with multiple meshes inside, we would need to loop over them
        aiMesh* mesh = scene->mMeshes[0];
        unsigned int numVertices = mesh->mNumVertices;
        unsigned int numFaces = mesh->mNumFaces;

        if (!mesh->HasNormals())
        {
            throw std::runtime_error("Expected assimp mesh to have normals since we requested to generate them if they are not present");
        }

        // Each mesh has a corresponding material (via mMaterialIndex)
        // We use the material to lookup textures
        unsigned int materialIndex = mesh->mMaterialIndex;
        if (materialIndex >= scene->mNumMaterials)
        {
            VR_WARNING << "Material index is invalid: " << materialIndex
                      << " >= " << scene->mNumMaterials
                      << "\nIn file: " << filename;
            return nullptr;
        }
        aiMaterial* material = scene->mMaterials[materialIndex];
        // We only lookup diffuse color textures
        // Maybe other texture types should be supported as well?
        aiTextureType textureType = aiTextureType_DIFFUSE;
        unsigned int numTextures = material->GetTextureCount(textureType);

        VR_INFO << "Loaded mesh from file '" << filename << "' with "
                << numMeshes << " mesh(es), "
                << numVertices << " vertices, "
                << numFaces << " faces, "
                << numTextures << " texture(s)\n";


        SoSeparator* result = new SoSeparator;

        // We only look at the first vertex color attribute (index 0)
        // Coin does not support multiple colors per vertex
        if (mesh->HasVertexColors(0))
        {
            addPerVertexColorMaterial(mesh->mColors[0], numVertices, result);
        }

        // Not sure what to do with the material colors
        // Ignore them for now
#if 0
        aiColor3D diffuseColor (0.f,0.f,0.f);
        bool diffuseOk = material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS;
        std::cout << "diffuseColor: " << diffuseOk << ": "
                  << diffuseColor.r << ", " << diffuseColor.g << ", " << diffuseColor.b << "\n";
#endif

        if (numTextures > 0)
        {
            addTextureMaterial(material, textureType, meshPath, result);
        }

        addVertices(numVertices, mesh->mVertices, result);
        addNormals(numVertices, mesh->mNormals, result);

        // Not all meshes have texture coordinates
        if (mesh->HasTextureCoords(0))
        {
            addTextureCoordinates(numVertices, mesh->mTextureCoords[0], result);
        }

        addIndexedFaceSet(numFaces, mesh->mFaces, result);

        return result;
    }

    ManipulationObjectPtr AssimpReader::readFileAsManipulationObject(const std::string& filename, const std::string& name)
    {
        if (auto tri = readFileAsTriMesh(filename))
        {
            return std::make_shared<ManipulationObject>(name, tri, filename);
        }
        return nullptr;
    }
    ManipulationObjectPtr AssimpReader::readBufferAsManipulationObject(const std::string_view& v, const std::string& name)
    {
        if (auto tri = readBufferAsTriMesh(v))
        {
            return std::make_shared<ManipulationObject>(name, tri);
        }
        return nullptr;
    }
}

