//----------------------------------------------------------------------------//
// model.h                                                                    //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MODELLOADER_H
#define MODELLOADER_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "global.h"
#include <QOpenGLWidget>
#include "trianglemesh.h"
//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class Modelloader
{
// misc
public:
  static const int STATE_IDLE;
  static const int STATE_FANCY;
  static const int STATE_MOTION;

// member variables
protected:
  int m_state;
  CalCoreModel* m_calCoreModel;
  int m_animationId[16];
  int m_animationCount;
  int m_meshId[32];
  int m_meshCount;
  GLuint m_textureId[32];
  int m_textureCount;
  float m_motionBlend[3];
  float m_renderScale ;
  float m_lodLevel;
  std::string m_path;

// constructors/destructor
public:
  Modelloader();
  virtual ~Modelloader();

  CalModel* m_calModel;


// member functions
public:
  void executeAction(int action);
  float getLodLevel();
  void getMotionBlend(float *pMotionBlend);
  float getRenderScale();
  int getState();
  bool onInit(const std::string& strFilename);
  void onRender();
  void onShutdown();
  void onUpdate(float elapsedSeconds);
  void setLodLevel(float lodLevel);
  void setMotionBlend(float *pMotionBlend, float delay);
  void setState(int state, float delay);
  void setPath( const std::string& strPath );


  //MY FUNCTIONS
  CalModel& getModelReference();
  int initializeRendering(vector<vector<QVector3D> > *verticesReturn, vector<vector<int> > *indicesReturn);
  int updateVertices(vector<vector<QVector3D> > *verticesReturn);
  void setScale(float scale);

  GLuint VAOmodel;
  GLuint VAO, VerticesBuffer, FacesBuffer;
  std::vector<float> vertices_array;
  std::vector<int> faces_array;


protected:
  GLuint loadTexture(const std::string& strFilename);
  void renderMesh(bool bWireframe, bool bLight);
  void renderSkeleton();
  void renderBoundingBox();
};

#endif

//----------------------------------------------------------------------------//
