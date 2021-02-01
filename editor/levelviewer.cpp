#include "levelviewer.h"

#include <algorithm>
#include <qmath.h>

#include <QKeyEvent>
#include <QPainter>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_2_Core>

LevelViewer::LevelViewer(Level* level,
												 const TextureAtlas* atlas,
												 const TextureAtlas* ents,
												 QWidget* parent, Qt::WindowFlags f): QOpenGLWidget(parent, f)
{
	_level = level;
	_tilesAtlas = atlas;
	_entitiesAtlas = ents;
	_grab = false;
	
	pos = QVector3D(0, 0, 10);
	
	look = QVector3D(0, 1, 0);
	up = QVector3D(0, 0, 1);
	
	angle_h = 0;
	angle_v = 0;
	
	w = s = a = d = false;
	
	setFocusPolicy(Qt::ClickFocus);
}

LevelViewer::~LevelViewer()
{
}

void LevelViewer::messageLogged(const QOpenGLDebugMessage& msg)
{
  QString error;
 
  // Format based on severity
  switch (msg.severity())
  {
  case QOpenGLDebugMessage::NotificationSeverity:
    error += "--";
    break;
  case QOpenGLDebugMessage::HighSeverity:
    error += "!!";
    break;
  case QOpenGLDebugMessage::MediumSeverity:
    error += "!~";
    break;
  case QOpenGLDebugMessage::LowSeverity:
    error += "~~";
    break;
  }
 
  error += " (";
 
  // Format based on source
#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break
  switch (msg.source())
  {
    CASE(APISource);
    CASE(WindowSystemSource);
    CASE(ShaderCompilerSource);
    CASE(ThirdPartySource);
    CASE(ApplicationSource);
    CASE(OtherSource);
    CASE(InvalidSource);
  }
#undef CASE
 
  error += " : ";
 
  // Format based on type
#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break
  switch (msg.type())
  {
    CASE(ErrorType);
    CASE(DeprecatedBehaviorType);
    CASE(UndefinedBehaviorType);
    CASE(PortabilityType);
    CASE(PerformanceType);
    CASE(OtherType);
    CASE(MarkerType);
    CASE(GroupPushType);
    CASE(GroupPopType);
  }
#undef CASE
 
  error += ")";
  qDebug() << qPrintable(error) << "\n" << qPrintable(msg.message()) << "\n";
}


void LevelViewer::initializeGL()
{
	QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(this);
	logger->initialize();
	connect(logger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(messageLogged(QOpenGLDebugMessage)));
	logger->startLogging();

	QOpenGLFunctions_3_2_Core* gl = nullptr;
	gl = context()->versionFunctions<QOpenGLFunctions_3_2_Core>();
	
	if(!gl) {
		throw std::string("cannot obtain 3.2 core context");
	}
	
	gl->initializeOpenGLFunctions();
	
	this->gl = gl;
	
	_shader = new QOpenGLShaderProgram(this);
	QFile vs(":/shaders/vertex.glsl"), fg(":/shaders/fragment.glsl");
	vs.open(QIODevice::ReadOnly); fg.open(QIODevice::ReadOnly);
	
	if(!vs.isOpen())
		throw std::string("cannot read resources");
	
	bool vert_ok = _shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vs.readAll());
	bool frag_ok = _shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fg.readAll());
	
	_shader->bindAttributeLocation("pos", 0);
	_shader->bindAttributeLocation("norm", 1);
	_shader->bindAttributeLocation("uv", 2);
	
	bool link_ok = _shader->link();
	
	if(!vert_ok) {
		throw std::string("vert shader error");
	}
	
	if(!frag_ok) {
		throw std::string("frag shader error");
	}
	
	if(!link_ok) {
		throw std::string("linking failed");
	}
	
	_attrPos = _shader->attributeLocation("pos");
	_attrUV = _shader->attributeLocation("uv");
	_uniSampler = _shader->attributeLocation("tex_sampler");
	_uniProj = _shader->uniformLocation("proj");
	_uniTime = _shader->uniformLocation("time");
	_uniGrid = _shader->uniformLocation("grid");
	_uniColor = _shader->uniformLocation("color");
	_uniLight = _shader->uniformLocation("use_light");
	
	gl->glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
	gl->glEnable(GL_DEPTH_TEST);
	gl->glEnable(GL_CULL_FACE);
	
	gl->glEnable(GL_BLEND);
	gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	timer.start(12, this);
	
	_renderer = new Renderer(_tilesAtlas, _entitiesAtlas,  _shader, gl, _level,  this);
	_drawGrid = false;
	_useLight = true;

	selectionStart = QPoint(-1, -1);
	selectionEnd = QPoint(-1, -1);
	
	selectedEntity = nullptr;
}

void LevelViewer::timerEvent(QTimerEvent* e)
{
	view.setToIdentity();
	
	view.lookAt(pos, pos + look, up);
	
	if(_grab) {
		if(w) pos += look * 0.2;
		if(s) pos -= look * 0.2;
		if(a) pos += QVector3D::crossProduct(up, look) * 0.2;
		if(d) pos += QVector3D::crossProduct(look, up) * 0.2;
	}
	
	this->update();
	
	time += 12.0f/1000.0f;
	_level->update(12.0/1000.0);
}

void LevelViewer::mousePressEvent(QMouseEvent* e)
{
	prevX = e->x();
	prevY = e->y();
	
	if(e->modifiers() != 0)
		return;
	
	if(e->button() == Qt::LeftButton) {
		QPoint p = pickBlock(e->x(), e->y());
		if(p.x() >= 0 && p.y() >= 0) {
			selectionStart = p;
			selectionEnd = p;
		}
	} else {
		selectedEntity = pickEntity(e->x(), e->y());
		emit entitySelected();
	}
}

void LevelViewer::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->modifiers() != 0)
		return;
	
	if(e->button() == Qt::RightButton)
		return;
	
	QPoint p = pickBlock(e->x(), e->y());
	if(p.x() >= 0 && p.y() >= 0) {
		selectionEnd = p;
		emit blockSelected();
	}
}

QPoint LevelViewer::pickBlock(int mx, int my)
{
	auto start_dir = rayFromMouse(mx, my);
	auto start = start_dir.first;
	auto dir = start_dir.second;

	float length = 100000.0f;
	int bi = -1, bj = -1;
	for(int i = 0; i < _level->sizeX(); i++) {
		for(int j = 0; j < _level->sizeY(); j++) {
			auto block_pos = QVector3D(i, j, _level->block(i, j).height);
			if(rayBoxIntersect(
					QVector3D(i, j, 0),
					QVector3D(i+1,j+1,_level->block(i, j).height),
					dir, start
				) &&
				(block_pos - start).length() < length)
			{
				length = (block_pos - start).length();
				bi = i;
				bj = j;
			}
		}
	}
	
	return QPoint(bi, bj);
}

QPair< QVector3D, QVector3D > LevelViewer::rayFromMouse(int mx, int my)
{
	float x = 2.0f * float(mx)/float(this->width()) - 1.0f;
	float y = 1.0f - 2.0f * float(my)/float(this->height());
	
	QVector4D pv = QVector4D(0, 0, 1, 1);
	pv = proj.inverted().map(pv);
	pv.setW(1);
	pv = view.inverted().map(pv);
	auto start = pv.toVector3D();
	
//	printf("(%.2f %.2f %.2f)\n", start.x(), start.y(), start.z());
	
	pv = QVector4D(x, y, -1, 1);
	pv = proj.inverted().map(pv);
	pv.setZ(-1); pv.setW(0);
	pv = view.inverted().map(pv);
	
	auto dir = pv.toVector3D().normalized();
	
	return QPair<QVector3D, QVector3D>(start, dir);
}

VisualEntity* LevelViewer::pickEntity(int mx, int my)
{
	auto start_dir = rayFromMouse(mx, my);
	auto start = start_dir.first;
	auto dir = start_dir.second;
	
	QVector<VisualEntity*> hit;
	for(VisualEntity* e : _level->entities()) {
		auto aabb = e->visualAABB();
		if(rayBoxIntersect(
			QVector3D(aabb.x0, aabb.y0, aabb.z0),
			QVector3D(aabb.x1, aabb.y1, aabb.z1),
			dir, start
		)) {
			hit.append(e);
		}
	}
	
	qSort(hit.begin(), hit.end(), [&start](VisualEntity* e1, VisualEntity* e2) -> bool { 
		return (QVector3D(e1->x(), e1->y(), e1->z()) - start).length() < 
		 (QVector3D(e2->x(), e2->y(), e2->z()) - start).length();
	});
	
	if(hit.size() > 0)
		return hit.first();
	
	return nullptr;
}

bool LevelViewer::rayBoxIntersect(QVector3D b1, QVector3D b2, QVector3D ray, QVector3D rayPos)
{
	float tmin = (b1.x() - rayPos.x()) / ray.x();
	float tmax = (b2.x() - rayPos.x()) / ray.x();
	if (tmin > tmax) std::swap(tmin, tmax);
	float tymin = (b1.y() - rayPos.y()) / ray.y();
	float tymax = (b2.y() - rayPos.y()) / ray.y();
	if (tymin > tymax) std::swap(tymin, tymax);
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	float tzmin = (b1.z() - rayPos.z()) / ray.z();
	float tzmax = (b2.z() - rayPos.z()) / ray.z();
	if (tzmin > tzmax) std::swap(tzmin, tzmax);
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	
	if ((tmin > 1000) || (tmax < -1000))
		return false;
	return true;
}

void LevelViewer::mouseMoveEvent(QMouseEvent* e)
{
	if(_grab) {
		auto p = mapToGlobal(geometry().center());
		auto c = QCursor::pos();
		
		float dx = 0.05 * (c.x() - p.x());
		float dy = 0.05 * (c.y() - p.y()); 
		
		angle_h += dx;
		angle_v -= dy;
		
		QVector3D view(0, 1, 0);
		QVector3D v_axis(0, 0, 1);
		
		QMatrix4x4 mat;
		mat.rotate(angle_h, v_axis);
		view = view * mat; 
		view.normalize();
		
		QVector3D h_axis = QVector3D::crossProduct(v_axis, view);
		h_axis.normalize();
		
		mat.setToIdentity();
		mat.rotate(angle_v, h_axis);
		view = view * mat;
		view.normalize();
		
		look = view;
		up = QVector3D::crossProduct(view, h_axis);
		up.normalize();
		
		QCursor::setPos(p);
	}
	
	//move entity
	if(e->modifiers() & Qt::Modifier::CTRL && e->buttons() == Qt::RightButton) {
		if(selectedEntity != nullptr) {
			auto start_dir = rayFromMouse(e->x(), e->y());
			auto start = start_dir.first;
			auto dir = start_dir.second;
			
			float t = (selectedEntity->z() - start.z()) / dir.z();
			auto v = QVector2D(start.x() + dir.x() * t, start.y() + dir.y() * t);
			
			_level->moveEntity(selectedEntity,
												 v.x(), v.y(), selectedEntity->z());
			
			emit entityMoved();
		}
		
		goto exit;
	}
	
	//block selection end
	if(e->modifiers() == 0 && e->buttons() & Qt::MouseButton::LeftButton) {
		QPoint p = pickBlock(e->x(), e->y());
		if(p.x() >= 0 && p.y() >= 0)
			selectionEnd = p;
	}
	
	exit:
	prevX = e->x();
	prevY = e->y();
}

void LevelViewer::wheelEvent(QWheelEvent* e)
{
}

void LevelViewer::keyReleaseEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_1) {
		_grab = !_grab;
		
		setMouseTracking(_grab);
		
		if(_grab)
			grabMouse(QCursor(Qt::CrossCursor));
		else
			releaseMouse();
	}
	
	if(e->key() == Qt::Key_W) w = false;
	if(e->key() == Qt::Key_S) s = false;
	if(e->key() == Qt::Key_A) a = false;
	if(e->key() == Qt::Key_D) d = false;
}

void LevelViewer::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_W) w = true;
	if(e->key() == Qt::Key_S) s = true;
	if(e->key() == Qt::Key_A) a = true;
	if(e->key() == Qt::Key_D) d = true;
}

void LevelViewer::paintGL()
{
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	_shader->bind();
	
	_shader->setUniformValue(_uniProj, proj * view);
	_shader->setUniformValue(_uniSampler, 0);
	_shader->setUniformValue(_uniTime, time);
	_shader->setUniformValue(_uniGrid, _drawGrid);
	_shader->setUniformValue(_uniLight, _useLight);
	_shader->setUniformValue(_uniColor, QVector4D(0.0, 0.0, 0.0, 0.0));
	_shader->setUniformValue("eye", pos);
	
	_renderer->selectedBlock[0] = qMin(selectionStart.x(), selectionEnd.x());
	_renderer->selectedBlock[1] = qMin(selectionStart.y(), selectionEnd.y());
	_renderer->selectedBlock[2] = qMax(selectionStart.x(), selectionEnd.x());
	_renderer->selectedBlock[3] = qMax(selectionStart.y(), selectionEnd.y());
	_renderer->selectedEntity = selectedEntity;
	_renderer->render(angle_h);
	
	_shader->release();
}

void LevelViewer::resizeGL(int w, int h)
{
	gl->glViewport(0, 0, w, h);
	
	float ratio = float(width())/float(height());
	proj.setToIdentity();	
	proj.perspective(60.0f, ratio, 0.1f, 100.0f);
}
