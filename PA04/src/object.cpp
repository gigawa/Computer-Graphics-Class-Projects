#include "object.h"
#include <math.h>
#include <fstream>

using namespace std;

Object::Object(char * objectFile) {
  if(!LoadModel(objectFile)) {
    cout << "Unable to load model" << endl;
  }

  model = glm::mat4(1.0);

  glGenBuffers(1, &VB);
  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &IB);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
}

Object::~Object()
{
  Vertices.clear();
  Indices.clear();
}

bool Object::LoadModel(char * objectFile) {
  ifstream fin(objectFile);
  string line;
  while(!fin.eof()) {
    char firstLetter;
    fin >> ws;
		fin >> firstLetter;
    if(firstLetter == 'v') {
      cout << "Vertex" << endl;
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      glm::vec3 tempVertex = glm::vec3(x, y, z);
      cout << "(" << tempVertex.x << ", " << tempVertex.y << ", " << tempVertex.z << ")" << endl;
      glm::vec3 tempColor = glm::vec3(x, y, z);
      Vertex temp = Vertex(tempVertex, tempColor);
      Vertices.push_back(temp);
    }else if(firstLetter == 'f') {
      cout << "Face" << endl;
      unsigned int temp;
      fin >> temp;
      cout << "(" << temp;
      Indices.push_back(temp-1);
      fin >> temp;
      cout << ", " << temp;
      Indices.push_back(temp-1);
      fin >> temp;
      cout << ", " << temp << ")" << endl;
      Indices.push_back(temp-1);
    }else {
      string s;
      getline(fin, s);
    }
    fin >> ws;
	}
  fin.close();
  return true;
}

glm::mat4 Object::GetModel()
{
  return model;
}

void Object::Render()
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

  glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
