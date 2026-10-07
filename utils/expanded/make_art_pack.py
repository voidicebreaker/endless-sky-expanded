#!/usr/bin/env python3
# Copyright (c) 2026 by Endless Sky Expanded contributors
#
# Endless Sky is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later version.
#
# Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.

# Builds art-pack.zip: one folder per image that the mod still needs, each with
# the current placeholder image, a few stock images to match the style of, and
# a prompt to give to an image generator. Run from the root of the repository:
#   python3 utils/expanded/make_art_pack.py [output.zip]

import json
import struct
import sys
import zipfile

MANIFEST = 'docs/expanded/art/manifest.json'
README = 'docs/expanded/art/README.md'

STYLE = {
	'ship': (
		'Top-down view of a spaceship, seen from directly above, nose pointing straight up,'
		' engines at the bottom. Fully transparent background, no shadow, no stars, no text.'
		' Detailed, slightly weathered, semi-realistic painted style, matching the style-*.png'
		' images (sprites from the game Endless Sky).'
	),
	'thumbnail': (
		'Three-quarter view of a spaceship, from slightly above and in front, on a fully'
		' transparent background, no shadow, no text. Semi-realistic painted style, matching'
		' the style-*.png images (ship thumbnails from the game Endless Sky).'
	),
	'station': (
		'Top-down view of a space station, seen from directly above, on a fully transparent'
		' background, no shadow, no stars, no text. Detailed, semi-realistic painted style, matching'
		' the style-*.png images (station sprites from the game Endless Sky).'
	),
	'outfit': (
		'Product shot of a single starship component, seen from slightly above at an angle,'
		' centered on a fully transparent background, no shadow, no text. Semi-realistic painted'
		' style, matching the style-*.png images (outfit icons from the game Endless Sky).'
	),
}

SIZE_NOTE = {
	'ship': (
		' Keep the same overall outline and footprint as reference.png: the engines, guns and'
		' turrets are placed to match it, so the new ship should fill the same area.'
	),
	'thumbnail': '',
	'station': '',
	'outfit': '',
}


def png_size(path):
	with open(path, 'rb') as file:
		header = file.read(24)
	return struct.unpack('>II', header[16:24])


def main():
	output = sys.argv[1] if len(sys.argv) > 1 else 'art-pack.zip'
	with open(MANIFEST, encoding='utf-8') as file:
		items = json.load(file)['items']

	with zipfile.ZipFile(output, 'w', zipfile.ZIP_DEFLATED) as pack:
		pack.write(README, 'art-pack/README.md')
		for number, item in enumerate(items, 1):
			image = 'images/' + item['image'] + '.png'
			width, height = png_size(image)
			folder = 'art-pack/{:02d} {} ({})/'.format(number, item['name'], item['kind'])
			prompt = (
				STYLE[item['kind']] + '\n\n' + item['prompt'] + '\n\n'
				+ 'Size: exactly {} x {} pixels, the same as reference.png.'.format(width, height)
				+ SIZE_NOTE[item['kind']] + '\n'
			)
			instructions = (
				'PROMPT (give this to the image generator, with the images in this folder):\n\n'
				+ prompt + '\n'
				+ 'WHEN IT IS DONE: save it as a PNG with a transparent background, named\n\n'
				+ '    ' + image.split('/')[-1] + '\n\n'
				+ 'and upload it to this folder of the repository, replacing the old one:\n\n'
				+ '    ' + '/'.join(image.split('/')[:-1]) + '\n'
			)
			pack.writestr(folder + 'prompt.txt', instructions)
			pack.write(image, folder + 'reference.png')
			for index, style in enumerate(item['style'], 1):
				pack.write('images/' + style + '.png', folder + 'style-{}.png'.format(index))
	print('Wrote ' + output + ' with ' + str(len(items)) + ' images to make.')


if __name__ == '__main__':
	main()
