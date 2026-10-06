# Art pack for Endless Sky: Expanded

This pack has everything needed to make or redo the mod's outfit and ship images
with an image generator.

## Where to get it

Download `art-pack.zip` from the playtest release, next to the game download:
https://github.com/voidicebreaker/endless-sky-expanded/releases/tag/playtest

## What is inside

One folder per image. Each folder has:

- `prompt.txt` - the text to give the generator, the exact size, and where the
  finished image goes.
- `reference.png` - the image the mod uses now. For ships, the new image
  should have the same outline and size, because the engines and guns are placed
  to match it.
- `style-1.png`, `style-2.png`, ... - stock images to match the look of.

## Steps

1. Open a folder and give the generator the text from `prompt.txt`, along with
   `reference.png` and the `style` images.
2. The easy way: collect the results (any size, JPG is fine, a plain bright pink
   or green background is fine) into a zip and give it to Claude, who will remove
   the background, resize them, and fit the ships' guns and engines to the new art.
3. The manual way: make the background transparent, resize to exactly the size in
   `prompt.txt`, name the file as `prompt.txt` says, and upload it on GitHub to
   the folder named there ("Add file", then "Upload files"), replacing the old one.

## For whoever maintains the mod

The list of images and prompts is in `manifest.json`, in this folder. The pack is
built by `utils/expanded/make_art_pack.py` and published by the playtest workflow.
When adding a new outfit or ship, copy a stock image to the matching `expanded`
folder under `images/`, point the data file at it, and add an entry to the
manifest.
