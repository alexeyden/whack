#include "renderer.h"
#include <QtGui/QImage>
#include <QtDebug>

#include "util/util.h"
#include "level/lightingcalculator.h"

Renderer::Renderer(
	const TextureAtlas* atlas, const TextureAtlas* ents, QOpenGLShaderProgram* shader, QOpenGLFunctions_3_2_Core* gl, const Level* level, QObject* parent): QObject(parent),
	_level(level)
{
	_gl = gl;
	
	_entityAtlas = ents;
	_textureAtlas = atlas;
	_shader = shader;
	
	_sunSprite = ents->textureID("light_sun");
	_pointSprite = ents->textureID("light_point");
	
	QImage img(atlas->asTexture().data(),
						 atlas->asTexture().width(), atlas->asTexture().height(),
						 atlas->asTexture().dataWidth() * 3, QImage::Format_RGB888
						);
	img = img.rgbSwapped();
	img = img.mirrored();

	const Texture& t = atlas->textureList()[0];
	QImage tmp = img.copy(t.x(), atlas->asTexture().height() - t.y() - t.height() , t.width(), t.height());
	auto text = new QOpenGLTexture(tmp, QOpenGLTexture::DontGenerateMipMaps);
	text->setMagnificationFilter(QOpenGLTexture::Nearest);
	text->setMinificationFilter(QOpenGLTexture::Nearest);
	text->setWrapMode(QOpenGLTexture::Repeat);
	_blockTex = text;
	
	img = QImage(ents->asTexture().data(),
						 ents->asTexture().width(), ents->asTexture().height(),
						 ents->asTexture().dataWidth() * 3, QImage::Format_RGB888
						);
	img = img.rgbSwapped();
	img = img.mirrored();
	
	{
		const Texture& t = ents->textureList()[0];
		
		QImage tmp = img.copy(t.x(), ents->asTexture().height() - t.y() - t.height() , t.width(), t.height());
		
		auto text = new QOpenGLTexture(tmp, QOpenGLTexture::DontGenerateMipMaps);
		text->setMagnificationFilter(QOpenGLTexture::Nearest);
		text->setMinificationFilter(QOpenGLTexture::Nearest);
		text->setWrapMode(QOpenGLTexture::Repeat);
		_entityTex = text;
	}
	
	shader->bind();
	
	_posAttr = _shader->attributeLocation("pos");
	_uvAttr = _shader->attributeLocation("uv");
	_normAttr = _shader->attributeLocation("norm");
	_uniTex = _shader->attributeLocation("tex_sampler");
	_uniLocal = _shader->uniformLocation("local");
	_uniHl = _shader->uniformLocation("highlight");
	
	selectedBlock[0] = 0;
	selectedBlock[1] = 0;
	selectedEntity = nullptr;
	
	vao1 = new QOpenGLVertexArrayObject(this);
	vao1->create();
	vao1->bind();
	
	vbo1.create();
	vbo1.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo1.bind();
	
	shader->enableAttributeArray("pos");
	shader->enableAttributeArray("norm");
	shader->enableAttributeArray("uv");
	
	shader->setAttributeArray("pos", GL_FLOAT, (void*) 0, 3, 8 * sizeof(GLfloat));
	shader->setAttributeArray("norm", GL_FLOAT, (void*) (sizeof(GLfloat) * 3), 3, 8 * sizeof(GLfloat));
	shader->setAttributeArray("uv", GL_FLOAT, (void*) (sizeof(GLfloat) * 3 * 2), 2, 8 * sizeof(GLfloat));
	
	vao1->release();

	vao2 = new QOpenGLVertexArrayObject(this);
	vao2->create();
	vao2->bind();
	
	vbo2.create();
	vbo2.setUsagePattern(QOpenGLBuffer::StreamDraw);
	vbo2.bind();
	
	shader->enableAttributeArray("pos");
	shader->enableAttributeArray("norm");
	shader->enableAttributeArray("uv");
	
	shader->setAttributeArray("pos", GL_FLOAT, (void*) 0, 3, 8 * sizeof(GLfloat));
	shader->setAttributeArray("norm", GL_FLOAT, (void*) (sizeof(GLfloat) * 3), 3, 8 * sizeof(GLfloat));
	shader->setAttributeArray("uv", GL_FLOAT, (void*) (sizeof(GLfloat) * 3 * 2), 2, 8 * sizeof(GLfloat));
	
	vao2->release();
	
	updateVBO();
}

Renderer::~Renderer() {
	vbo1.destroy();
	vbo2.destroy();
	vao1->destroy();
	vao2->destroy();
	_entityTex->release();
	_entityTex->destroy();
	_blockTex->release();
	_blockTex->destroy();
}

void Renderer::updateLevel(Level* level)
{
	_level = level;
	updateVBO();
}

void Renderer::renderPlane(QVector3D p1, QVector3D p2, QVector3D p3, QVector3D n, GLuint tex,
													 bool ent = false, GLuint prim = GL_TRIANGLES)
{
	QVector3D p4 = p1 + (p3 - p1) - (p2 - p1);
	
	vec2 s0(
		float(_textureAtlas->texture(tex).x())/_textureAtlas->asTexture().width(),
		
		float(_textureAtlas->texture(tex).y() + _textureAtlas->texture(tex).height())/_textureAtlas->asTexture().height()
	);
	vec2 s1(
		float(_textureAtlas->texture(tex).x() + _textureAtlas->texture(tex).width())/_textureAtlas->asTexture().width(),
		float(_textureAtlas->texture(tex).y())/_textureAtlas->asTexture().height()
	);
	
	GLfloat vertices[] = {
		p1.x(), p1.y(), p1.z(), n.x(), n.y(), n.z(), (float) s0.x, (float) s0.y,
		p2.x(), p2.y(), p2.z(), n.x(), n.y(), n.z(), (float) s1.x, (float) s0.y,
		p3.x(), p3.y(), p3.z(), n.x(), n.y(), n.z(), (float) s1.x, (float) s1.y,
		
		p1.x(), p1.y(), p1.z(), n.x(), n.y(), n.z(), (float) s0.x, (float) s0.y,
		p3.x(), p3.y(), p3.z(), n.x(), n.y(), n.z(), (float) s1.x, (float) s1.y,
		p4.x(), p4.y(), p4.z(), n.x(), n.y(), n.z(), (float) s0.x, (float) s1.y
	};

	vbo2.bind();
	vbo2.allocate((void*) vertices, sizeof(GLfloat) * 6 * 8);
	vbo2.release();
	
	_blockTex->bind();
	
	vao2->bind();
	_gl->glDrawArrays(GL_TRIANGLES, 0, 6);
	vao2->release();
}

void Renderer::renderSprite(QVector3D pos, float w, float h, GLuint tex, float angle)
{
	QMatrix4x4 local;
	local.translate(pos.x(), pos.y(), pos.z());
	
	QMatrix4x4 nrot;
//	nrot.translate(-w/2.0f, -w/2.0f);
	nrot.rotate(-angle, 0, 0, 1);
	_shader->setUniformValue(_uniLocal, local * nrot);
		
	auto p1 = QVector3D(-w/2.0f, 0.0f, 0.0f);
	auto p2 = QVector3D(w/2.0f, 0.0f, 0.0);
	auto p3 = QVector3D(w/2.0f, 0.0f, h);
	auto n = QVector3D(0.0f, -1.0f, 0.0f) * nrot;
	
	QVector3D p4 = p1 + (p3 - p1) - (p2 - p1);
	
	GLfloat vertices[] = {
		p1.x(), p1.y(), p1.z(),
		p2.x(), p2.y(), p2.z(),
		p3.x(), p3.y(), p3.z(),
		
		p1.x(), p1.y(), p1.z(),
		p3.x(), p3.y(), p3.z(),
		p4.x(), p4.y(), p4.z(),
	};
	
	GLfloat norms[] = {
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z(),
		
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z()
	};
	
	vec2 s0(
		float(_entityAtlas->texture(tex).x())/_entityAtlas->asTexture().width(),
		
		float(_entityAtlas->texture(tex).y() + _entityAtlas->texture(tex).height())/_entityAtlas->asTexture().height()
	);
	vec2 s1(
		float(_entityAtlas->texture(tex).x() + _entityAtlas->texture(tex).width())/_entityAtlas->asTexture().width(),
		float(_entityAtlas->texture(tex).y())/_entityAtlas->asTexture().height()
	);
	
	GLfloat uvs[] = {
		(float) s0.x, (float) s0.y,
		(float) s1.x, (float) s0.y,
		(float) s1.x, (float) s1.y,
		                      
		(float) s0.x, (float) s0.y,
		(float) s1.x, (float) s1.y,
		(float) s0.x, (float) s1.y
	};
	
	QVector<GLfloat> res;
	for(int i = 0; i < 6; i++) {
		res.append(vertices[i*3 + 0]);
		res.append(vertices[i*3 + 1]);
		res.append(vertices[i*3 + 2]);
		
		res.append(norms[i*3 + 0]);
		res.append(norms[i*3 + 1]);
		res.append(norms[i*3 + 2]);
		
		res.append(uvs[i*2 + 0]);
		res.append(uvs[i*2 + 1]);
	}
	
	vbo2.bind();
	vbo2.allocate((void*) res.data(), sizeof(GLfloat) * res.size());
	vbo2.release();
	
	_entityTex->bind();
	
	vao2->bind();
	_gl->glDrawArrays(GL_TRIANGLES, 0, 6);
	vao2->release();
}

QVector< GLfloat > Renderer::makePlane(QVector3D p1, QVector3D p2, QVector3D p3, QVector3D n, GLuint tex)
{
	QVector3D p4 = p1 + (p3 - p1) - (p2 - p1);
	
	GLfloat vertices[] = {
		p1.x(), p1.y(), p1.z(),
		p2.x(), p2.y(), p2.z(),
		p3.x(), p3.y(), p3.z(),
		
		p1.x(), p1.y(), p1.z(),
		p3.x(), p3.y(), p3.z(),
		p4.x(), p4.y(), p4.z(),
	};
	
	GLfloat norms[] = {
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z(),
		
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z(),
		n.x(), n.y(), n.z()
	};
	
	float sy0 = _textureAtlas->texture(tex).y();
	float sy1 = (_textureAtlas->texture(tex).y() + _textureAtlas->texture(tex).height()* (p3 - p2).length());
	
	vec2 s0(
		float(_textureAtlas->texture(tex).x())/_textureAtlas->asTexture().width(),
		sy0/_textureAtlas->asTexture().height()
		
	);
	vec2 s1(
		float(_textureAtlas->texture(tex).x() + _textureAtlas->texture(tex).width())/_textureAtlas->asTexture().height(),
		sy1/_textureAtlas->asTexture().height()
	);
	
	GLfloat uvs[] = {
		(float) s0.x, (float) s0.y,
		(float) s1.x, (float) s0.y,
		(float) s1.x, (float) s1.y,
		
		(float) s0.x, (float) s0.y,
		(float) s1.x, (float) s1.y,
		(float) s0.x, (float) s1.y
	};
	
	QVector<GLfloat> res;
	for(int i = 0; i < 6; i++) {
		res.append(vertices[i*3 + 0]);
		res.append(vertices[i*3 + 1]);
		res.append(vertices[i*3 + 2]);
		
		res.append(norms[i*3 + 0]);
		res.append(norms[i*3 + 1]);
		res.append(norms[i*3 + 2]);
		
		res.append(uvs[i*2 + 0]);
		res.append(uvs[i*2 + 1]);
	}
	
	return res;
}

QVector< GLfloat > Renderer::makeBox(AABB< float > box, unsigned int tex[6], bool ceil, bool floor, bool sides=true)
{
	QVector<GLfloat> data;
	
	//floor
	if(floor)
	data +=	makePlane(
			QVector3D(box.x0, box.y0, box.z1),
			QVector3D(box.x1, box.y0, box.z1),
			QVector3D(box.x1, box.y1, box.z1),
			
			QVector3D(0.0f, 0.0f, 1.0f), tex[BlockTex::BT_FLOOR]);
	 
	if(ceil)
	data +=	makePlane(
			QVector3D(box.x1, box.y1, box.z0),
			QVector3D(box.x1, box.y0, box.z0),
			QVector3D(box.x0, box.y0, box.z0),
			
			QVector3D(0.0f, 0.0f, -1.0f), tex[BlockTex::BT_CEILING]);
	
	if(sides) {
		//north
		data +=	makePlane(
				QVector3D(box.x1, box.y1, box.z0),
				QVector3D(box.x0, box.y1, box.z0),
				QVector3D(box.x0, box.y1, box.z1),
				
				QVector3D(0.0f, 1.0f, 0.0f), tex[BlockTex::BT_NORTH]);
		
		//south
		data +=	makePlane(
				QVector3D(box.x0, box.y0, box.z0),
				QVector3D(box.x1, box.y0, box.z0),
				QVector3D(box.x1, box.y0, box.z1),
									
				QVector3D(0.0f, -1.0f, 0.0f), tex[BlockTex::BT_SOUTH]);

			//west
		data +=	makePlane(
				QVector3D(box.x0, box.y1, box.z0),
				QVector3D(box.x0, box.y0, box.z0),
				QVector3D(box.x0, box.y0, box.z1),
				
				QVector3D(-1.0f, 0.0, 0.0f), tex[BlockTex::BT_WEST]);
		
			//east
		data +=	makePlane(
				QVector3D(box.x1, box.y0, box.z0),
				QVector3D(box.x1, box.y1, box.z0),
				QVector3D(box.x1, box.y1, box.z1),
				
				QVector3D(1.0f, 0.0f, 0.0f), tex[BlockTex::BT_EAST]);
	}
	
	return data;
}

QVector< GLfloat > Renderer::makeLevel()
{
	QVector<GLfloat> data;
	
	for(int i = 0; i < _level->sizeX(); i++) {
		for(int j = 0; j < _level->sizeY(); j++) {
			const Block& block = _level->block(i, j);
			bool no_edges = false;
			
			unsigned tex[6];
			for(int t = 0; t < 6; t++)
				tex[t] = block.texture((BlockTex) t);
			
			if(block.notchHeight == 0) {
				int h = floor(block.height);
				float r = block.height - h;
				
				if(i > 0 && j > 0 && i < _level->sizeX()-1 && j < _level->sizeY()-1) {
					no_edges = _level->block(i-1, j).height    >= block.height && !_level->block(i-1, j).hasTop() &&
											_level->block(i+1, j).height >= block.height &&   !_level->block(i+1, j).hasTop() &&
											_level->block(i, j-1).height   >= block.height && !_level->block(i, j-1).hasTop() &&
											_level->block(i, j+1).height   >= block.height && !_level->block(i, j+1).hasTop();
				}
				
				bool have_cap = r > 0.0001;
				
				for(int k = 0; k < h; k++)
					data += makeBox(AABB<float>(i, j, k, i + 1, j + 1, k+1), tex, false, !have_cap && k==h-1, !no_edges);
				
				if(have_cap)
					data += makeBox(AABB<float>(i, j, h, i + 1, j + 1, h+r), tex, false, true, !no_edges);
			}
			else {
				int h = floor(block.notch);
				float r = block.notch - h;
				
				if(i > 0 && j > 0 && i < _level->sizeX()-1 && j < _level->sizeY()-1) {
					no_edges = _level->block(i-1,    j).standHeight() >= block.notch &&  !_level->block(i-1, j).hasTop() &&
 											_level->block(i+1, j).standHeight() >= block.notch &&    !_level->block(i+1, j).hasTop() &&
											_level->block(i,   j-1).standHeight() >= block.notch &&  !_level->block(i, j-1).hasTop() &&
											_level->block(i,   j+1).standHeight() >= block.notch &&   !_level->block(i, j+1).hasTop(); 
				}
				bool have_cap = r > 0.0001;
				
				for(int k = 0; k < h; k++)
					data += makeBox(AABB<float>(i, j, k, i + 1, j + 1, k+1), tex, false, !have_cap, !no_edges);
				
				if(have_cap)
					data += makeBox(AABB<float>(i, j, h, i + 1, j + 1, h+r), tex, false, true, !no_edges);
				
				h = floor(block.height - block.notch - block.notchHeight);
				r = (block.height - block.notch - block.notchHeight) - h;
				have_cap = r > 0.0001;
				
				if(i > 0 && j > 0 && i < _level->sizeX()-1 && j < _level->sizeY()-1) {
					no_edges = _level->block(i-1,    j).height >= block.height &&  !_level->block(i-1, j).hasTop() && 
											_level->block(i+1, j).height >= block.height &&    !_level->block(i+1, j).hasTop() &&
											_level->block(i,   j-1).height >= block.height &&  !_level->block(i, j-1).hasTop() &&
											_level->block(i,   j+1).height >= block.height &&  !_level->block(i, j+1).hasTop();
				}
				for(int k = 0; k < h; k++)
					data += makeBox(AABB<float>(i, j, k + block.notch + block.notchHeight, i + 1, j + 1, k+1 + block.notch + block.notchHeight),
													tex, true, !have_cap, !no_edges);
				
				if(have_cap)
					data += makeBox(AABB<float>(i, j, h + block.notch + block.notchHeight, i + 1, j + 1, block.notch + block.notchHeight + h+r), tex, true, true, !no_edges);
			}
		}
	}
	
	return data;
}

void Renderer::renderBox(QVector3D pos, float w, float h, bool wireframe, float dir=0)
{
	QMatrix4x4 local;
	
	local.translate(pos.x(), pos.y(), pos.z());
	local.rotate(dir * 180.0f/M_PI, 0, 0, 1);
	local.translate(-w/2.0f, -w/2.0f, 0);
	_shader->setUniformValue("local", local);
	
	GLuint prim = GL_TRIANGLES;
	
	//floor
		renderPlane(
			QVector3D(0.0f, 0.0f, h),
			QVector3D(w, 0.0f, h),
			QVector3D(w, w, h),
			
			QVector3D(0.0f, 0.0f, 1.0f), 0, 1, prim);
	
		//north
		renderPlane(
			QVector3D(w, w, 0.0f),
			QVector3D(0.0f, w, 0.0),
			QVector3D(0.0f, w, h),
			
			QVector3D(0.0f, 1.0f, 0.0f),
			0, 1, prim);
	
		//south
		renderPlane(
			QVector3D(0.0f, 0.0f, 0.0f),
			QVector3D(w, 0.0f, 0.0),
			QVector3D(w, 0.0f, h),
								
			QVector3D(0.0f, -1.0f, 0.0f),
			0, 1, prim);

		//west
		renderPlane(
			QVector3D(0.0f, w, 0.0f),
			QVector3D(0.0f, 0.0f, 0.0),
			QVector3D(0.0f, 0.0f, h),
			
			QVector3D(-1.0f, 0.0, 0.0f),
			0, 1, prim);
	
		//east
		renderPlane(
			QVector3D(w, 0.0f, 0.0f),
			QVector3D(w, w, 0.0),
			QVector3D(w, w, h),
			
			QVector3D(1.0f, 0.0f, 0.0f),
			0, 1, prim);
}

void Renderer::renderBlock(int i, int j)
{
	const Block& block = _level->block(i, j);

	unsigned texFloor = block.texture(BT_FLOOR); 
	unsigned texCeiling = block.texture(BT_CEILING); 
	unsigned texWallN = block.texture(BT_NORTH);
	unsigned texWallS = block.texture(BT_SOUTH); 
	unsigned texWallW = block.texture(BT_WEST); 
	unsigned texWallE = block.texture(BT_EAST); 
	
	if(block.notchHeight == 0) {
		//floor
		renderPlane(
								QVector3D(0.0f, 0.0f, block.height),
								QVector3D(1.0f, 0.0f, block.height),
								QVector3D(1.0f, 1.0f, block.height),
								
								QVector3D(0.0f, 0.0f, 1.0f),
								texFloor);
	
		//north
		if(j == _level->sizeY()-1 || _level->block(i, j+1).height < block.height || _level->block(i, j+1).notchHeight > 0)
		renderPlane(
								QVector3D(1.0f, 1.0f, 0.0f),
								QVector3D(0.0f, 1.0f, 0.0),
								QVector3D(0.0f, 1.0f, block.height),
								
								QVector3D(0.0f, 1.0f, 0.0f),
								texWallN);
	
		//south
		if(j == 0 || _level->block(i, j-1).height < block.height || _level->block(i, j-1).notchHeight > 0)
		renderPlane(
								QVector3D(0.0f, 0.0f, 0.0f),
								QVector3D(1.0f, 0.0f, 0.0),
								QVector3D(1.0f, 0.0f, block.height),
								
								QVector3D(0.0f, -1.0f, 0.0f),
								texWallS);

		//west
		if(i == 0 || _level->block(i-1, j).height < block.height || _level->block(i-1, j).notchHeight > 0)
		renderPlane(
								QVector3D(0.0f, 1.0f, 0.0f),
								QVector3D(0.0f, 0.0f, 0.0),
								QVector3D(0.0f, 0.0f, block.height),
								
								QVector3D(-1.0f, 0.0, 0.0f),
								texWallW);
	
		//east
		if(i == _level->sizeX()-1 || _level->block(i+1, j).height < block.height || _level->block(i+1, j).notchHeight > 0)
		renderPlane(
								QVector3D(1.0f, 0.0f, 0.0f),
								QVector3D(1.0f, 1.0f, 0.0),
								QVector3D(1.0f, 1.0f, block.height),
								
								QVector3D(1.0f, 0.0f, 0.0f),
								texWallE);
	}
	else {
		QVector3D n_floor(0.0f, 0.0f, 1.0f);
		QVector3D n_north(0.0f, 1.0f, 0.0f);
		QVector3D n_south(0.0f, -1.0f, 0.0f);
		QVector3D n_west(-1.0f, 0.0, 0.0f);
		QVector3D n_east(1.0f, 0.0f, 0.0f);
		QVector3D n_ceiling(-0.0f, 0.0f, -1.0f);
								
		renderPlane(QVector3D(0.0f, 0.0f, block.notch), QVector3D(1.0f, 0.0f, block.notch), QVector3D(1.0f, 1.0f, block.notch), n_floor , texFloor);
		renderPlane(QVector3D(1.0f, 1.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0), QVector3D(0.0f, 1.0f, block.notch), n_north, texWallN);
		renderPlane(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0), QVector3D(1.0f, 0.0f, block.notch), n_south, texWallS);
		renderPlane(QVector3D(0.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0), QVector3D(0.0f, 0.0f, block.notch), n_west, texWallW);
		renderPlane(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(1.0f, 1.0f, 0.0), QVector3D(1.0f, 1.0f, block.notch), n_east, texWallE);	
		
		float ne = block.notch + block.notchHeight;
		renderPlane(QVector3D(0.0f, 0.0f, block.height), QVector3D(1.0f, 0.0f, block.height), QVector3D(1.0f, 1.0f, block.height), n_floor, texFloor);
		renderPlane(QVector3D(0.0f, 0.0f, ne), QVector3D(0.0f, 1.0f, ne), QVector3D(1.0f, 1.0f, ne), n_ceiling, texCeiling);
		
		renderPlane(QVector3D(1.0f, 1.0f, ne), QVector3D(0.0f, 1.0f, ne), QVector3D(0.0f, 1.0f, block.height), n_north,texWallN);
		renderPlane(QVector3D(0.0f, 0.0f, ne), QVector3D(1.0f, 0.0f, ne), QVector3D(1.0f, 0.0f, block.height), n_south,texWallS);
		renderPlane(QVector3D(0.0f, 1.0f, ne), QVector3D(0.0f, 0.0f, ne), QVector3D(0.0f, 0.0f, block.height), n_west,texWallW);
		renderPlane(QVector3D(1.0f, 0.0f, ne), QVector3D(1.0f, 1.0f, ne), QVector3D(1.0f, 1.0f, block.height), n_east,texWallE);	
	}
		
	for(int i = 0; i < 6; i++) {
		for(const auto& d : block.decals((BlockTex) i)) {
			renderDecal((BlockTex) i, d, block);
		}
	}
}

void Renderer::updateVBO()
{
	QVector<GLfloat> data = makeLevel();
	
	vbo1.bind();
	vbo1.allocate((void*) data.data(), sizeof(GLfloat) * data.size());
	vbo1.release();
	
	_vboSize = data.size() / 8;
}

void Renderer::render(float angle)
{
	auto m = QMatrix4x4();
	m.setToIdentity();
	
	_shader->setUniformValue("sel_0", QVector2D(selectedBlock[0], selectedBlock[1]));
	_shader->setUniformValue("sel_1", QVector2D(selectedBlock[2], selectedBlock[3]));
	
	_shader->setUniformValue(_shader->uniformLocation("local"), m);
	
	_blockTex->bind();
	
	vao1->bind();
	_gl->glDrawArrays(GL_TRIANGLES, 0, _vboSize);
	vao1->release();
	
	for(int x = 0; x < _level->sizeX(); x++) {
		for(int y = 0; y < _level->sizeY(); y++) {
			const Block& block = _level->block(x, y);
			
			QMatrix4x4 local;
			local.translate(x, y);
			
			_shader->setUniformValue("local", local);
			
			for(int i = BlockTex_FIRST; i < BlockTex_SIZE; i++) {
				const auto& decals = block.decals((BlockTex) i);
				for(const auto& d : decals) {
					renderDecal((BlockTex) i, d, block);
				}
			}
		}
	}
	
	for(VisualEntity* e : _level->entities()) {
		renderSprite(QVector3D(e->x(), e->y(), e->z()),
								 e->visualWidth(), e->visualHeight(),
								 e->frameID(cos(angle * M_PI/180.0f), sin(angle * M_PI/180.0f)), angle);
		
		if(e == selectedEntity) {
			_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(1.0, 0.0, 0.0, 0.5));
			
			renderBox(QVector3D(e->x(), e->y(), e->z()), e->width(), e->height(), 0);
			
			_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(1.0, 1.0, 1.0, 0.2));
			_shader->setUniformValue(_uniHl, 1);
			renderBox(QVector3D(e->x(), e->y(), e->z()), e->visualWidth(), e->visualHeight(), 0);
			_shader->setUniformValue(_uniHl, 0);
			
			_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(0.0, 0.0, 0.0, 0.0));
		}
	}
	
	for(const Light& l : _level->lights()) {
		renderSprite(QVector3D(l.x, l.y, l.z), 1, 1, l.type == LT_SUN ? _sunSprite : _pointSprite, angle);
		float r = std::min<float>(l.red / 255.0f, 0xff);
		float g = std::min<float>(l.green /255.0f, 0xff);
		float b = std::min<float>(l.blue / 255.0f, 0xff);
		
		_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(r, g, b, 0.5));
		renderBox(QVector3D(l.x, l.y, l.z), 1, 1, 0);
		_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(0.0, 0.0, 0.0, 0.0));
	}
	
	renderPlayer();
}

void Renderer::renderPlayer()
{  
	_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(1.0, 0.2, 1.0, 0.8));
	
	renderBox(
						QVector3D(_level->player()->x(), _level->player()->y(), _level->player()->z()),
						1, 1, false, _level->player()->dir());
	renderBox(
						QVector3D(_level->player()->x() + cos(_level->player()->dir()),
											_level->player()->y() + sin(_level->player()->dir()), _level->player()->z() + 0.3f),
						0.2f, 0.2f, false, _level->player()->dir());
	
	_shader->setUniformValue(_shader->uniformLocation("color"), QVector4D(0.0, 0.0, 0.0, 0.0));
}

void Renderer::renderDecal(BlockTex side, const BlockDecal& d, const Block& block)
{
	QVector3D n_floor(0.0f, 0.0f, 1.0f);
	QVector3D n_north(0.0f, 1.0f, 0.0f);
	QVector3D n_south(0.0f, -1.0f, 0.0f);
	QVector3D n_west(-1.0f, 0.0, 0.0f);
	QVector3D n_east(1.0f, 0.0f, 0.0f);
	QVector3D n_ceiling(-0.0f, 0.0f, -1.0f);
	
	float x0 = std::min(std::max(d.offsetX, 0.0f), 1.0f);
	float y0 = d.offsetY;
	float x1 = std::max(d.offsetX + d.sizeX, 0.0f);
	float y1 = d.offsetY + d.sizeY;
	
	if(side == BT_FLOOR) {
		x0 = std::min(std::max(d.offsetX, 0.0f), 1.0f);
		y0 = std::min(std::max(d.offsetY, 0.0f), 1.0f);
		x1 = std::min(std::max(d.offsetX + d.sizeX, 0.0f), 1.0f);
		y1 = std::min(std::max(d.offsetY + d.sizeY, 0.0f), 1.0f);
		renderPlane(
								QVector3D(x0, y0, block.standHeight() + 0.001),
								QVector3D(x1, y0, block.standHeight() + 0.001),
								QVector3D(x1, y1, block.standHeight() + 0.001), n_floor, d.texture);
	}
	else if(side == BT_NORTH) {
		renderPlane(
								QVector3D(x1, 1.001, y0),
								QVector3D(x0, 1.001, y0),
								QVector3D(x0, 1.001, y1),
								n_north, d.texture);
	}
	else if(side == BT_SOUTH) {
		renderPlane(
								QVector3D(x0, -0.001f, y0),
								QVector3D(x1, -0.001f, y0),
								QVector3D(x1, -0.001f, y1), n_south, d.texture);
	}
	else if(side == BT_WEST) {
		renderPlane(
								QVector3D(-0.001f, x1, y0),
								QVector3D(-0.001f, x0, y0),
								QVector3D(-0.001f, x0, y1), n_west, d.texture);
	}
	else if(side == BT_EAST) {
		renderPlane(
								QVector3D(1.001f, x0, y0),
								QVector3D(1.001f, x1, y0),
								QVector3D(1.001f, x1, y1), n_east, d.texture);	
	}
}

