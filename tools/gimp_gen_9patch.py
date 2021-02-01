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

def generate_nondir_anim(image, layer, name, animtype):
	image.undo_group_start()
	e, x1, y1, x2, y2 = pdb.gimp_selection_bounds(image)
	anim_grp = get_layer(image, "anim")
	
	group = get_layer(anim_grp, name + animtype)
	if group == None:
		pass
	if y2 - y1 < x2 - x1:
		dx = y2 - y1
		cut_to_new_layer(layer, group, name + "_tl", x1, y1, w, h)
	
	image.undo_group_end()

def generate_9patch(image, layer, text):
	image.undo_group_start()
	
	if layer == None or image == None or text == None:
		return
	
	e, x1, y1, x2, y2 = pdb.gimp_selection_bounds(image)
	
	w = (x2 - x1)/3
	h = (y2 - y1)/3
	
	group = pdb.gimp_layer_group_new(image)
	group.name = text
	image.add_layer(group)
	
	cut_to_new_layer(layer, group, text + "_tl", x1, y1, w, h)
	cut_to_new_layer(layer, group, text + "_t", x1 + w, y1, w, h)
	cut_to_new_layer(layer, group, text + "_tr", x1 + 2*w, y1, w, h)
	cut_to_new_layer(layer, group, text + "_r", x1 + 2*w, y1 + w, w, h)
	cut_to_new_layer(layer, group, text + "_br", x1 + 2*w, y1 + 2*w, w, h)
	cut_to_new_layer(layer, group, text + "_b", x1 + w, y1 + 2*w, w, h)
	cut_to_new_layer(layer, group, text + "_bl", x1, y1 + 2 * w, w, h)
	cut_to_new_layer(layer, group, text + "_l", x1, y1 + w, w, h)
	cut_to_new_layer(layer, group, text + "_c", x1 + w, y1 + w, w, h)
	
	image.undo_group_end()
	
register(
	"python-fu-atlas-gen-9patch",
	"Generate 9patch from selection",
	"Generate 9patch",
	"rtgbnm",
	"Licensed under WTFPL conditions",
	"01.06.2014",
	"Generate 9patch",
	"*",
	[
		(PF_IMAGE, "image", "Image", None),
		(PF_DRAWABLE, "layer", "Layer", None),
		(PF_STRING, "text", "Name", "")
	],
	[],
	generate_9patch, menu="<Image>/Tools/")
	
main()