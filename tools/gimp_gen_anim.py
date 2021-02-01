#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gimpfu import *

def get_layer(img, name):
	found = [l for l in img.layers if l.name == name]
	if len(found) > 0:
		return found.pop()
	return None

def cut_to_new_layer(layer, parent, name, x, y, w, h):
	pdb.gimp_rect_select(layer.image, x, y, w, h, 2, 0, 0)
	pdb.gimp_edit_cut(layer)
	new_layer = gimp.Layer(layer.image, name, w, h, RGBA_IMAGE, 100, NORMAL_MODE)
	pdb.gimp_layer_translate(new_layer, x, y)
	pdb.gimp_image_insert_layer(layer.image, new_layer, parent, 0)
	sel = pdb.gimp_edit_paste(new_layer, False)
	pdb.gimp_floating_sel_anchor(sel)
	return new_layer

def generate_anim(image, layer, text, anim, size):
	image.undo_group_start()
	
	if layer == None or image == None or text == None:
		return
	
	e, x1, y1, x2, y2 = pdb.gimp_selection_bounds(image)
	
	anim_layer = get_layer(image, 'anim')
	
	group = pdb.gimp_layer_group_new(image)
	group.name = text + "_" + types[anim]
	
	pdb.gimp_image_insert_layer(image, group, anim_layer, 0)
	
	num = 0
	for x in range(x1, x2, size):
		cut_to_new_layer(layer, group, text + "_{0:02d}".format(num), x, y1, size, size)
		num += 1
	
	image.undo_group_end()

types = ["idle", "walk", "attack", "death"]
	
register(
	"python-fu-gen-anim",
	"Generate animation from selection",
	"Generate animation",
	"rtgbnm",
	"Licensed under WTFPL conditions",
	"15.01.2016",
	"Generate animation",
	"*",
	[
		(PF_IMAGE, "image", "Image", None),
		(PF_DRAWABLE, "layer", "Layer", None),
		(PF_STRING, "text", "Name", "name"),
		(PF_OPTION, "anim", "Type", 0, types),
		(PF_INT, "size", "Size",  32),
	],
	[],
	generate_anim, menu="<Image>/Tools/")
	
main()