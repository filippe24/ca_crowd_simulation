//----------------------------------------------------------------------------//
// model.cpp                                                                  //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#if defined(_MSC_VER) && _MSC_VER <= 0x0600
#pragma warning(disable : 4786)
#endif

#include "modelloader.h"
#include "demo.h"
#include "menu.h"
#include "tga.h"

//----------------------------------------------------------------------------//
// Static member variables initialization                                     //
//----------------------------------------------------------------------------//

const int Modelloader::STATE_IDLE = 0;
const int Modelloader::STATE_FANCY = 1;
const int Modelloader::STATE_MOTION = 2;

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Modelloader::Modelloader()
{
  m_calCoreModel = new CalCoreModel("dummy");

  m_state = STATE_IDLE;
  m_motionBlend[0] = 0.6f;
  m_motionBlend[1] = 0.1f;
  m_motionBlend[2] = 0.3f;
  m_animationCount = 0;
  m_meshCount = 0;
  m_renderScale = 0.01f;
  m_lodLevel = 1.0f;
/* DEBUG-CODE
  Sphere.x = 0.0f;
  Sphere.y = 5.0f;
  Sphere.z = 45.0f;
*/
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

Modelloader::~Modelloader()
{
}

//----------------------------------------------------------------------------//
// Execute an action of the model                                             //
//----------------------------------------------------------------------------//

void Modelloader::executeAction(int action)
{
  switch(action)
  {
    case 0:
      m_calModel->getMixer()->executeAction(m_animationId[5], 0.3f, 0.3f);
      break;
    case 1:
      m_calModel->getMixer()->executeAction(m_animationId[6], 0.3f, 0.3f);
      break;
  }
}

//----------------------------------------------------------------------------//
// Get the lod level of the model                                             //
//----------------------------------------------------------------------------//

float Modelloader::getLodLevel()
{
  return m_lodLevel;
}

//----------------------------------------------------------------------------//
// Get the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Modelloader::getMotionBlend(float *pMotionBlend)
{
  pMotionBlend[0] = m_motionBlend[0];
  pMotionBlend[1] = m_motionBlend[1];
  pMotionBlend[2] = m_motionBlend[2];
}

//----------------------------------------------------------------------------//
// Get the render scale of the model                                          //
//----------------------------------------------------------------------------//

float Modelloader::getRenderScale()
{
  return m_renderScale;
}

//----------------------------------------------------------------------------//
// Get the animation state of the model                                       //
//----------------------------------------------------------------------------//

int Modelloader::getState()
{
  return m_state;
}

//----------------------------------------------------------------------------//
// Read a int from file stream (to avoid Little/Big endian issue)
//----------------------------------------------------------------------------//

int readInt( std::ifstream *file ) 
{
	int x = 0;
	for ( int i = 0; i < 32; i+=8 ) 
	{
		char c;
		file->read ( &c, 1 );
		x += c << i;
 	}
 	return x;
 }

//----------------------------------------------------------------------------//
// Load and create a texture from a given file                                //
//----------------------------------------------------------------------------//

GLuint Modelloader::loadTexture(const std::string& strFilename)
{
  GLuint pId=0;
  if(stricmp(strrchr(strFilename.c_str(),'.'),".raw")==0)
  {

     // open the texture file
     std::ifstream file;
     file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
     if(!file)
     {
       std::cerr << "Texture file '" << strFilename << "' not found." << std::endl;
       return 0;
     }

     // load the dimension of the texture
     int width = readInt(&file);     
     int height = readInt(&file);     
     int depth = readInt(&file);     

     // allocate a temporary buffer to load the texture to
    unsigned char *pBuffer;
    pBuffer = new unsigned char[width * height * depth];
    if(pBuffer == nullptr)
    {
      std::cerr << "Memory allocation for texture '" << strFilename << "' failed." << std::endl;
      return 0;
    }

    // load the texture
    file.read((char *)pBuffer, width * height * depth);

    // explicitely close the file
    file.close();

    // generate texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &pId);
    glBindTexture(GL_TEXTURE_2D, pId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &pBuffer[0]);
  
    // free the allocated memory
    delete [] pBuffer;
  
  }
  else if (stricmp(strrchr(strFilename.c_str(),'.'),".tga")==0)
  {

    CTga *Tga;
    Tga = new CTga();

    //Note: This will always make a 32-bit texture
    if(Tga->ReadFile(strFilename.c_str())==0)
    {
      Tga->Release();
      return false;
    }

    //Bind texture
    int width = Tga->GetSizeX();
    int height = Tga->GetSizeY();
    int depth = Tga->Bpp() / 8;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &pId);
      
    glBindTexture(GL_TEXTURE_2D, pId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     
    glTexImage2D(GL_TEXTURE_2D, 0, ((depth == 3) ? GL_RGB : GL_RGBA), width, height, 0, ((depth == 3) ? GL_RGB : GL_RGBA) , GL_UNSIGNED_BYTE, (char*)Tga->GetPointer() );

	 Tga->Release();
  }


  return pId;
}

//----------------------------------------------------------------------------//
// Initialize the model                                                       //
//----------------------------------------------------------------------------//

bool Modelloader::onInit(const std::string& strFilename)
{
  // open the model configuration file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    std::cerr << "Failed to open model configuration file '" << strFilename << "'." << std::endl;
    return false;
  }

  // initialize the data path
  std::string strPath = m_path;

  // initialize the animation count
  int animationCount;
  animationCount = 0;

  // parse all lines from the model configuration file
  int line;
  for(line = 1; ; line++)
  {
    // read the next model configuration line
    std::string strBuffer;
    std::getline(file, strBuffer);

    // stop if we reached the end of file
    if(file.eof()) break;

    // check if an error happend while reading from the file
    if(!file)
    {
      std::cerr << "Error while reading from the model configuration file '" << strFilename << "'." << std::endl;
      return false;
    }

    // find the first non-whitespace character
    std::string::size_type pos;
    pos = strBuffer.find_first_not_of(" \t");

    // check for empty lines
    if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

    // check for comment lines
    if(strBuffer[pos] == '#') continue;

    // get the key
    std::string strKey;
    strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
    pos += strKey.size();

    // get the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos);
    if((pos == std::string::npos) || (strBuffer[pos] != '='))
    {
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
      return false;
    }

    // find the first non-whitespace character after the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos + 1);

    // get the data
    std::string strData;
    strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

    // handle the model creation
    if(strKey == "scale")
    {
      // set rendering scale factor
      m_renderScale = atof(strData.c_str());
    }
    else if(strKey == "path")
    {
      // set the new path for the data files if one hasn't been set already
      if (m_path == "") strPath = strData;
    }
    else if(strKey == "skeleton")
    {
      // load core skeleton
      std::cout << "Loading skeleton '" << (strPath + strData) << "'..." << std::endl;
      if(!m_calCoreModel->loadCoreSkeleton(strPath + strData))
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "animation")
    {
      // load core animation
      std::cout << "Loading animation '" << strData << "'..." << std::endl;
      m_animationId[animationCount] = m_calCoreModel->loadCoreAnimation(strPath + strData);
      if(m_animationId[animationCount] == -1)
      {
        CalError::printLastError();
        return false;
      }

      animationCount++;
    }
    else if(strKey == "mesh")
    {
      // load core mesh
      std::cout << "Loading mesh '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMesh(strPath + strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "material")
    {
      // load core material
      std::cout << "Loading material '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMaterial(strPath + strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else
    {
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
      return false;
    }
  }

  // explicitely close the file
  file.close();

  // load all textures and store the opengl texture id in the corresponding map in the material
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // get the core material
    CalCoreMaterial *pCoreMaterial;
    pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

    // loop through all maps of the core material
    int mapId;
    for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
    {
      // get the filename of the texture
      std::string strFilename;
      strFilename = pCoreMaterial->getMapFilename(mapId);

      // load the texture from the file
      GLuint textureId;
      textureId = loadTexture(strPath + strFilename);

      // store the opengl texture id in the user data of the map
      pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
    }
  }

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc.
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel->createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
  }

  // Calculate Bounding Boxes

  m_calCoreModel->getCoreSkeleton()->calculateBoundingBoxes(m_calCoreModel);

  m_calModel = new CalModel(m_calCoreModel);

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < m_calCoreModel->getCoreMeshCount(); meshId++)
  {
    m_calModel->attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel->setMaterialSet(0);

  // set initial animation state
  m_state = STATE_MOTION;
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], 0.0f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], 0.0f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], 0.0f);

  return true;
}

//----------------------------------------------------------------------------//
// Render the skeleton of the model                                           //
//----------------------------------------------------------------------------//

void Modelloader::renderSkeleton()
{
  // draw the bone lines
  float lines[1024][2][3];
  int nrLines;
  nrLines =  m_calModel->getSkeleton()->getBoneLines(&lines[0][0][0]);
//  nrLines = m_calModel->getSkeleton()->getBoneLinesStatic(&lines[0][0][0]);

  glLineWidth(3.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
    int currLine;
    for(currLine = 0; currLine < nrLines; currLine++)
    {
      glVertex3f(lines[currLine][0][0], lines[currLine][0][1], lines[currLine][0][2]);
      glVertex3f(lines[currLine][1][0], lines[currLine][1][1], lines[currLine][1][2]);
    }
  glEnd();
  glLineWidth(1.0f);

  // draw the bone points
  float points[1024][3];
  int nrPoints;
  nrPoints =  m_calModel->getSkeleton()->getBonePoints(&points[0][0]);
//  nrPoints = m_calModel->getSkeleton()->getBonePointsStatic(&points[0][0]);

  glPointSize(4.0f);
  glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f);
    int currPoint;
    for(currPoint = 0; currPoint < nrPoints; currPoint++)
    {
      glVertex3f(points[currPoint][0], points[currPoint][1], points[currPoint][2]);
    }
  glEnd();
  glPointSize(1.0f);
}

//----------------------------------------------------------------------------//
// Render the bounding boxes of a model                                       //
//----------------------------------------------------------------------------//

void Modelloader::renderBoundingBox()
{  

   CalSkeleton *pCalSkeleton = m_calModel->getSkeleton();

   std::vector<CalBone*> &vectorCoreBone = pCalSkeleton->getVectorBone();

   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_LINES);      

   for(size_t boneId=0;boneId<vectorCoreBone.size();++boneId)
   {
      CalBoundingBox & calBoundingBox  = vectorCoreBone[boneId]->getBoundingBox();

	  CalVector p[8];
	  calBoundingBox.computePoints(p);

	  
	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[1].x,p[1].y,p[1].z);

	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[2].x,p[2].y,p[2].z);

	  glVertex3f(p[1].x,p[1].y,p[1].z);
	  glVertex3f(p[3].x,p[3].y,p[3].z);

	  glVertex3f(p[2].x,p[2].y,p[2].z);
	  glVertex3f(p[3].x,p[3].y,p[3].z);

  	  glVertex3f(p[4].x,p[4].y,p[4].z);
	  glVertex3f(p[5].x,p[5].y,p[5].z);

	  glVertex3f(p[4].x,p[4].y,p[4].z);
	  glVertex3f(p[6].x,p[6].y,p[6].z);

	  glVertex3f(p[5].x,p[5].y,p[5].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);

	  glVertex3f(p[6].x,p[6].y,p[6].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);

	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[4].x,p[4].y,p[4].z);

	  glVertex3f(p[1].x,p[1].y,p[1].z);
	  glVertex3f(p[5].x,p[5].y,p[5].z);

	  glVertex3f(p[2].x,p[2].y,p[2].z);
	  glVertex3f(p[6].x,p[6].y,p[6].z);

	  glVertex3f(p[3].x,p[3].y,p[3].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);  

   }

   glEnd();

}



//----------------------------------------------------------------------------//
// Update the model                                                           //
//----------------------------------------------------------------------------//

void Modelloader::onUpdate(float elapsedSeconds)
{
  // update the model
  m_calModel->update(elapsedSeconds);
}

//----------------------------------------------------------------------------//
// Shut the model down                                                        //
//----------------------------------------------------------------------------//

void Modelloader::onShutdown()
{
  delete m_calModel;
  delete m_calCoreModel;
}

//----------------------------------------------------------------------------//
// Set the lod level of the model                                             //
//----------------------------------------------------------------------------//

void Modelloader::setLodLevel(float lodLevel)
{
  m_lodLevel = lodLevel;

  // set the new lod level in the cal model renderer
  m_calModel->setLodLevel(m_lodLevel);
}

//----------------------------------------------------------------------------//
// Set the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Modelloader::setMotionBlend(float *pMotionBlend, float delay)
{
  m_motionBlend[0] = pMotionBlend[0];
  m_motionBlend[1] = pMotionBlend[1];
  m_motionBlend[2] = pMotionBlend[2];

  m_calModel->getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
  m_calModel->getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);

  m_state = STATE_MOTION;
}

//----------------------------------------------------------------------------//
// Set a new animation state within a given delay                             //
//----------------------------------------------------------------------------//

void Modelloader::setState(int state, float delay)
{
  // check if this is really a new state
  if(state != m_state)
  {
    if(state == STATE_IDLE)
    {
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_IDLE], 1.0f, delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_IDLE;
    }
    else if(state == STATE_FANCY)
    {
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_FANCY], 1.0f, delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_FANCY;
    }
    else if(state == STATE_MOTION)
    {
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);
      m_state = STATE_MOTION;
    }
  }
}


//----------------------------------------------------------------------------//
// Set a path to override any config file path                                //
//----------------------------------------------------------------------------//

void Modelloader::setPath( const std::string& strPath )
{
   m_path = strPath;
}

//----------------------------------------------------------------------------//













//*******************************************************************************
//*************************** MY FUNCTIONS **************************************
//*******************************************************************************


//----------------------------------------------------------------------------//
// Initialize the model                                                       //
//----------------------------------------------------------------------------//
CalModel& Modelloader::getModelReference()
{
    return *m_calModel;
}





//---------------------------------------
//RENDER THE MESH-----------------------
//---------------------------------------


//----------------------------------------------------------------------------//
// Render the mesh of the model                                               //
//----------------------------------------------------------------------------//
//return the number of submeshes
int Modelloader::initializeRendering(vector<vector<QVector3D>> *verticesReturn, vector<vector<int> > *indicesReturn)
{

  int num_of_submeshes = 0;

    //************** 1 *******************

  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel->getRenderer();

  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return -1;

              // set the global OpenGL states
              glEnable(GL_DEPTH_TEST);
              glShadeModel(GL_SMOOTH);

              glEnableClientState(GL_VERTEX_ARRAY);
              glEnableClientState(GL_NORMAL_ARRAY);


  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    num_of_submeshes += submeshCount;

    // render all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {

        //temp values
        vector<QVector3D> currentVertices;
        vector<int> currentIndices;

        // get the transformed vertices of the submesh
        static float meshVertices[30000][3];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);
        int checkVertexCount = pCalRenderer->getVertexCount();
        for( int v = 0; v < vertexCount; v++)
        {
            if(meshVertices[v][0] == 0.0f && meshVertices[v][1] == 0.0f && meshVertices[v][2] == 0.0f)
            {
                std::cout << "***************vertex i=" << v << " have x: " << meshVertices[v][0] << "  y: " << meshVertices[v][1] << "  y: " << meshVertices[v][2] << std::endl;
            }

            currentVertices.push_back(QVector3D(meshVertices[v][0]*m_renderScale, meshVertices[v][1]*m_renderScale, meshVertices[v][2]*m_renderScale));
        }
        // get the transformed normals of the submesh
        static float meshNormals[30000][3];
        pCalRenderer->getNormals(&meshNormals[0][0]);


        // get the faces of the submesh
        static CalIndex meshFaces[50000][3];
        int faceCount;
        faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);
        for( int f = 0; f < faceCount; f++)
        {
            currentIndices.push_back(meshFaces[f][0]);
            currentIndices.push_back(meshFaces[f][1]);
            currentIndices.push_back(meshFaces[f][2]);
        }

                // set the vertex and normal buffers
                glVertexPointer(3, GL_FLOAT, 0, &meshVertices[0][0]);
                glNormalPointer(GL_FLOAT, 0, &meshNormals[0][0]);


        verticesReturn->push_back(currentVertices);
        indicesReturn->push_back(currentIndices);
      }
    }
  }


  // set initial animation state
  m_state = STATE_MOTION;
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], 0.3f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], 0.3f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], 0.3f);



  // end the rendering
  pCalRenderer->endRendering();

  return num_of_submeshes;
}


int Modelloader::updateVertices(vector<vector<QVector3D> > *verticesReturn)
{


    int num_of_submeshes = 0;

      //************** 1 *******************

    // get the renderer of the model
    CalRenderer *pCalRenderer;
    pCalRenderer = m_calModel->getRenderer();

    // get the number of meshes
    int meshCount;
    meshCount = pCalRenderer->getMeshCount();

    // render all meshes of the model
    int meshId;
    for(meshId = 0; meshId < meshCount; meshId++)
    {
      // get the number of submeshes
      int submeshCount;
      submeshCount = pCalRenderer->getSubmeshCount(meshId);

      num_of_submeshes += submeshCount;

      // render all submeshes of the mesh
      int submeshId;
      for(submeshId = 0; submeshId < submeshCount; submeshId++)
      {

        // select mesh and submesh for further data access
        if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
        {

          //temp values
          vector<QVector3D> currentVertices;

          // get the transformed vertices of the submesh
          static float meshVertices[30000][3];
          int vertexCount;
          vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);
          for( int v = 0; v < vertexCount; v++)
          {
              if(meshVertices[v][0] == 0.0f && meshVertices[v][1] == 0.0f && meshVertices[v][2] == 0.0f)
              {
                  std::cout << "***************vertex i=" << v << " have x: " << meshVertices[v][0] << "  y: " << meshVertices[v][1] << "  y: " << meshVertices[v][2] << std::endl;
              }
              currentVertices.push_back(QVector3D(meshVertices[v][0]*m_renderScale, meshVertices[v][1]*m_renderScale, meshVertices[v][2]*m_renderScale));

          }
          verticesReturn->push_back(currentVertices);
        }
      }
    }

    // end the rendering
    pCalRenderer->endRendering();


    return num_of_submeshes;
}


void Modelloader::setScale(float scale)
{
    m_renderScale = scale;
}
