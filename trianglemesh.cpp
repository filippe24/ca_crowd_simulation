#include <iostream>
#include "trianglemesh.h"


using namespace std;


struct CornerEdge
{
	int vertexA, vertexB, corner;

	bool operator<(const CornerEdge &cEdge) { return (vertexA < cEdge.vertexA) || ((vertexA == cEdge.vertexA) && (vertexB < cEdge.vertexB)); }
	bool operator==(const CornerEdge &cEdge) { return (vertexA == cEdge.vertexA) && (vertexB == cEdge.vertexB); }
};


int next(int corner)
{
	return 3 * (corner / 3) + (corner + 1) % 3;
}

int previous(int corner)
{
	return 3 * (corner / 3) + (corner + 2) % 3;
}


TriangleMesh::TriangleMesh() : vboVertices(QOpenGLBuffer::VertexBuffer),
										 vboNormals(QOpenGLBuffer::VertexBuffer),
										 eboTriangles(QOpenGLBuffer::IndexBuffer)
{
}

TriangleMesh::~TriangleMesh()
{
    destroy();
}


void TriangleMesh::addVertex(const QVector3D &position)
{
	vertices.push_back(position);
}
int TriangleMesh::clearVertices()
{
    int num = int(vertices.size());
    vertices.clear();
    return num;
}

void TriangleMesh::addTriangle(int v0, int v1, int v2)
{
	triangles.push_back(v0);
	triangles.push_back(v1);
	triangles.push_back(v2);
}

void TriangleMesh::buildCube()
{
	GLfloat vertices[] = {-1, -1, -1,
								  1, -1, -1,
								  1,  1, -1,
								 -1,  1, -1,
								 -1, -1,  1,
								  1, -1,  1,
								  1,  1,  1,
								 -1,  1,  1
								};

	GLuint faces[] = {3, 1, 0, 3, 2, 1,
							5, 6, 7, 4, 5, 7,
							7, 3, 0, 0, 4, 7,
							1, 2, 6, 6, 5, 1,
							0, 1, 4, 5, 4, 1,
							2, 3, 7, 7, 6, 2
						  };

	int i;

	for(i=0; i<8; i+=1)
		addVertex(0.5f * QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	for(i=0; i<12; i++)
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

bool TriangleMesh::init(QOpenGLShaderProgram *program)
{
	vector<QVector3D> replicatedVertices, normals;
	vector<unsigned int> perFaceTriangles;

	buildReplicatedVertices(replicatedVertices, normals, perFaceTriangles);

	program->bind();

	vao.destroy();
	vao.create();
	if(vao.isCreated())
		vao.bind();
	else
		return false;

	vboVertices.destroy();
	vboVertices.create();
	if(vboVertices.isCreated())
		vboVertices.bind();
	else
		return false;
    vboVertices.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	vboNormals.destroy();
	vboNormals.create();
	if(vboNormals.isCreated())
		vboNormals.bind();
	else
		return false;
    vboNormals.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	eboTriangles.destroy();
	eboTriangles.create();
	if(eboTriangles.isCreated())
		eboTriangles.bind();
	else
		return false;
	eboTriangles.setUsagePattern(QOpenGLBuffer::StaticDraw);

	fillVBOs(replicatedVertices, normals, perFaceTriangles);

	vao.release();
	program->release();

	return true;
}

void TriangleMesh::destroy()
{
	vao.destroy();
	vboVertices.destroy();
	vboNormals.destroy();
	eboTriangles.destroy();

	vertices.clear();
	triangles.clear();
}

void TriangleMesh::render(QOpenGLFunctions &gl)
{
	vao.bind();
	eboTriangles.bind();
	gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	vao.release();
    eboTriangles.release();
}

void TriangleMesh::buildReplicatedVertices(vector<QVector3D> &replicatedVertices, vector<QVector3D> &normals, vector<unsigned int> &perFaceTriangles)
{
	normals.resize(triangles.size());

	for(unsigned int i=0; i<triangles.size(); i+=3)
	{
		replicatedVertices.push_back(vertices[triangles[i]]);
		replicatedVertices.push_back(vertices[triangles[i+1]]);
		replicatedVertices.push_back(vertices[triangles[i+2]]);

		QVector3D N = QVector3D::crossProduct(vertices[triangles[i+1]] - vertices[triangles[i]], vertices[triangles[i+2]] - vertices[triangles[i]]);
		N.normalize();
		normals[i] = N;
		normals[i+1] = N;
		normals[i+2] = N;

		perFaceTriangles.push_back(perFaceTriangles.size());
		perFaceTriangles.push_back(perFaceTriangles.size());
		perFaceTriangles.push_back(perFaceTriangles.size());
	}
}

void TriangleMesh::fillVBOs(vector<QVector3D> &replicatedVertices, vector<QVector3D> &normals, vector<unsigned int> &perFaceTriangles)
{
	vboVertices.bind();
	vboVertices.allocate(&replicatedVertices[0], 3 * sizeof(float) * replicatedVertices.size());
	vboVertices.release();

	vboNormals.bind();
	vboNormals.allocate(&normals[0], 3 * sizeof(float) * normals.size());
	vboNormals.release();

	eboTriangles.bind();
	eboTriangles.allocate(&perFaceTriangles[0], sizeof(int) * perFaceTriangles.size());
	eboTriangles.release();


//    std::cout << " VBO filled with vertices: " << replicatedVertices.size() << "  noramls : " << normals.size() << "  faces: " << perFaceTriangles.size() << std::endl;
}


//getter
unsigned long TriangleMesh::getTrianglesSize()
{
    return triangles.size();
}
unsigned long TriangleMesh::getVerticesSize()
{
    return vertices.size();
}

void TriangleMesh::reloadVertices(QOpenGLShaderProgram *program)
{



    vector<QVector3D> replicatedVertices, normals;
//    vector<unsigned int> perFaceTriangles;
    normals.resize(triangles.size());


    for(unsigned int i=0; i<triangles.size(); i+=3)
    {
        replicatedVertices.push_back(vertices[triangles[i]]);
        replicatedVertices.push_back(vertices[triangles[i+1]]);
        replicatedVertices.push_back(vertices[triangles[i+2]]);

        QVector3D N = QVector3D::crossProduct(vertices[triangles[i+1]] - vertices[triangles[i]], vertices[triangles[i+2]] - vertices[triangles[i]]);
        N.normalize();
        normals[i] = N;
        normals[i+1] = N;
        normals[i+2] = N;
    }




    if(vboVertices.isCreated())
        vboVertices.bind();
    else
        std::cout << "*************no created VBO vertices**************" <<std::endl;


    //chose between allocate or write??
    vboVertices.allocate(&replicatedVertices[0], 3 * int(sizeof(float) * replicatedVertices.size()));
    vboVertices.release();

    if(vboNormals.isCreated())
        vboNormals.bind();
    else
        std::cout << "*************no created vbo normals**************" <<std::endl;

    vboNormals.allocate(&normals[0], 3 * int(sizeof(float) * normals.size()));
    vboNormals.release();

}
