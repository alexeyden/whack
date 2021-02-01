#ifndef LEVELVIEWER_H
#define LEVELVIEWER_H

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QMatrix4x4>
#include <QBasicTimer>
#include <QOpenGLDebugLogger>
#include <QElapsedTimer>

#include "graphics/textureatlas.h"
#include "renderer.h"
#include "level/level.h"

class LevelViewer : public QOpenGLWidget
{
	Q_OBJECT
	
public:
	explicit LevelViewer(
		Level* level,
		const TextureAtlas* atlas,
		const TextureAtlas* ents,
		QWidget* parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~LevelViewer();
	
	QPoint selectionStart;
	QPoint selectionEnd;
	VisualEntity* selectedEntity;
	
	void setLevel(Level* l) {
		_level = l;
		_renderer->updateLevel(_level);
	}

	void drawGrid(bool draw) {
		_drawGrid = draw;
	}
	void useLight(bool light) {
		_useLight = light;
	}
	
signals:
	void entitySelected();
	void entityMoved();
	void blockSelected();

public slots:
	void messageLogged(const QOpenGLDebugMessage &msg);
	
protected:
	virtual void timerEvent(QTimerEvent*);
	
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void wheelEvent(QWheelEvent*);
	
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	
	QPair<QVector3D, QVector3D> rayFromMouse(int mx, int my);
	
	QPoint pickBlock(int x, int y);
	VisualEntity* pickEntity(int x, int y);

	bool rayBoxIntersect(QVector3D b1, QVector3D b2, QVector3D ray, QVector3D rayPos);
	
	Level* _level;
	
	const TextureAtlas* _tilesAtlas;
	const TextureAtlas* _entitiesAtlas;
	
	Renderer* _renderer;
	
	QOpenGLShaderProgram* _shader;
	QOpenGLFunctions_3_2_Core* gl;
	
	GLuint _attrPos;
	GLuint _attrUV;
	GLuint _uniSampler;
	GLuint _uniProj;
	GLuint _uniTime;
	GLuint _uniGrid;
	GLuint _uniLight;
	GLuint _uniColor;
	
	QMatrix4x4 proj;
	QMatrix4x4 view;
	
	QVector3D pos;
	QVector3D look;
	QVector3D up;
	
	float angle_h;
	float angle_v;
	
	bool w,s,a,d;
	
	float time;
	bool _drawGrid;
	bool _useLight;
	bool _grab;
	
	QBasicTimer timer;
	
	double prevX, prevY;
};

#endif // LEVELVIEWER_H
