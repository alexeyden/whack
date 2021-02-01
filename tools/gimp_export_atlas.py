#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gimpfu import *
import json
import re
import os

def build_static_list(image):
	static_layer = get_layer(image, 'static')
	if not static_layer:
		return None
	
	static_list = []
	for static_img in static_layer.layers:
		static_list.append({
			'name': static_img.name,
			'rect': [static_img.offsets[0], static_img.offsets[1], static_img.width, static_img.height]
		})
	
	return static_list

def build_anim_list(image):
	anim_layer = get_layer(image, 'anim')
	if not anim_layer:
		return None
	
	dir_groups = ['front', 'back', 'left', 'right']
	
	anim_list = []
	for anim in anim_layer.layers:
		if not anim.visible:
			continue
		
		directional = len([l for l in anim.layers if l.name.startswith('front')]) > 0
		name = re.sub(r'_[^_]*$','',anim.name)
		type = anim.name.split('_')[-1]
		if not directional:
			frames = []
			for img in anim.layers:
				frames.append([img.offsets[0], img.offsets[1], img.width, img.height])
		else:
			front,back,left,right = [],[],[],[]
			
			for img in anim.layers:
				if img.name.startswith('front'):
					front.append([img.offsets[0], img.offsets[1], img.width, img.height])
				if img.name.startswith('back'):
					back.append([img.offsets[0], img.offsets[1], img.width, img.height])
				if img.name.startswith('left'):
					left.append([img.offsets[0], img.offsets[1], img.width, img.height])
				if img.name.startswith('right'):
					right.append([img.offsets[0], img.offsets[1], img.width, img.height])
			frames = {
				'front': front,
				'back': back,
				'left': left,
				'right': right 
			}
			
		anim_list.append({
			'name' : name,
			'type' : type,
			'directional' : directional,
			'frames' : frames
		})
		
	return anim_list
		
def get_layer(img, name):
	found = [l for l in img.layers if l.name == name]
	if len(found) > 0:
		return found.pop()
	return None

def build_ninepatch_list(image):
	ninepatch_layer = get_layer(image, 'ninepatch')
	if not ninepatch_layer:
		return None
	
	ninepatch_list = []
	for np in ninepatch_layer.layers:
		get_rect = lambda img : [img.offsets[0], img.offsets[1], img.width, img.height]
		ninepatch = {
			'name' : np.name
		}
		for img in np.layers:
			if img.name.endswith("_t"):
				ninepatch['top'] = get_rect(img)
			elif img.name.endswith('_b'):
				ninepatch['bottom'] = get_rect(img)
			elif img.name.endswith('_l'):
				ninepatch['left'] = get_rect(img)
			elif img.name.endswith('_r'):
				ninepatch['right'] = get_rect(img)
				
			elif img.name.endswith('_tl'):
				ninepatch['top-left'] = get_rect(img)
			elif img.name.endswith('_tr'):
				ninepatch['top-right'] = get_rect(img)
			elif img.name.endswith('_bl'):
				ninepatch['bottom-left'] = get_rect(img)
			elif img.name.endswith('_br'):
				ninepatch['bottom-right'] = get_rect(img)
				
			elif img.name.endswith('_c'):
				ninepatch['center'] = get_rect(img)
				
		ninepatch_list.append(ninepatch)
	return ninepatch_list

def atlas_save(image, pretty_json, path, img_path):
	sep = os.path.sep if not path.endswith(os.path.sep) else ''
	save_name = os.path.dirname(image.filename) + os.path.sep + path + sep + re.sub(r'.xcf$', '.json', os.path.basename(image.filename))
	bmp_name = img_path + sep + re.sub(r'\.[a-zA-Z0-9]+$', r'.bmp', os.path.basename(image.filename))
	
	f = open(save_name, 'w')
	
	json_obj = {
		'image-file' : bmp_name,
		'static' : build_static_list(image),
		'animation': build_anim_list(image),
		'ninepatch': build_ninepatch_list(image)
	}
	
	if pretty_json:
		f.write(json.dumps(json_obj, indent=2, separators=(',', ': ')))
	else:
		f.write(json.dumps(json_obj))
	f.close()
	
	pdb.gimp_message('Saved to {0}'.format(os.path.realpath(save_name)))
	
register(
	"python-fu-atlas-save-to-file",
	"Generate texture atlas from layers",
	"Generate texture atlas",
	"rtgbnm",
	"Licensed under WTFPL conditions",
	"01.06.2014",
	"Generate atlas",
	"*",
	[
		(PF_IMAGE, "image", "Image", None),
		(PF_BOOL, "pretty_json", "Format JSON", True),
		(PF_STRING, "path", "Export path",  '../textures/'),
		(PF_STRING, "img_path", "Image path", 'textures/')
	],
	[],
	atlas_save, menu="<Image>/Tools/")
	
main()