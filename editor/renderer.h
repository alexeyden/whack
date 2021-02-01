#ifndef RENDERER_H
#define RENDERER_H

#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtCore/QVector>
#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "level/level.h"
#include "graphics/textureatlas.h"

class Renderer :  QObject
{
	Q_OBJECT
	
public:
	explicit Renderer(
		const TextureAtlas* atlas,
		const TextureAtlas* ents,
		QOpenGLShaderProgram* shader, QOpenGLFunctions_3_2_Core* gl, const Level* level, QObject* parent = nullptr);
	virtual ~Renderer();
	void render(float angle);
	
	VisualEntity* selectedEntity;
	int selectedBlock[4];
	
	void updateLevel(Level* level);
protected:
	void renderPlane(QVector3D p1, QVector3D p2, QVector3D p3, QVector3D n,
									GLuint tex, bool ents, GLuint prim);
	void renderSprite(QVector3D pos, float w, float h, GLuint tex, float angle);
	void renderBox(QVector3D pos, float w, float h, bool wireframe, float dir);
	void renderBlock(int i, int j);
	void renderPlayer();
	void renderDecal(BlockTex side, const BlockDecal& decal, const Block& b);
	
	QVector<GLfloat> makePlane(QVector3D p1, QVector3D p2, QVector3D p3, QVector3D n, GLuint tex);
	QVector<GLfloat> makeBox(AABB< float > box, unsigned int tex[6], bool ceil, bool floor, bool sides);
	QVector<GLfloat> makeLevel();
	
	void updateVBO();

	QOpenGLBuffer vbo1;
	QOpenGLBuffer vbo2;
	QOpenGLVertexArrayObject* vao1;
	QOpenGLVertexArrayObject* vao2;
	
	GLuint _vboSize;
	QOpenGLShaderProgram* _shader;
	
	const TextureAtlas* _textureAtlas;
	const TextureAtlas* _entityAtlas;
	
	QOpenGLTexture* _blockTex;
	QOpenGLTexture* _entityTex;
	
	GLuint _posAttr, _normAttr, _uvAttr;
	GLuint _uniLocal, _uniTex, _uniHl;
	QOpenGLFunctions_3_2_Core* _gl;
	const Level* _level;
	
	unsigned _sunSprite;
	unsigned _pointSprite;
};

#endif // RENDERER_H
