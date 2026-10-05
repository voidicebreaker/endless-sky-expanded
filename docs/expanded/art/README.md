# Art pack for Endless Sky: Expanded

The new outfits and ships in this mod borrow stock images from the game for now.
This pack has everything needed to make proper ones with an image generator.

## Where to get it

Download `art-pack.zip` from the playtest release, next to the game download:
https://github.com/voidicebreaker/endless-sky-expanded/releases/tag/playtest

## What is inside

One folder per image. Each folder has:

- `prompt.txt` - the text to give the generator, the exact size, and where the
  finished image goes.
- `reference.png` - the stock image the mod uses now. For ships, the new image
  should have the same outline and size, because the engines and guns are placed
  to match it.
- `style-1.png`, `style-2.png`, ... - stock images to match the look of.

## Steps

1. Open a folder and give the generator the text from `prompt.txt`, along with
   `reference.png` and the `style` images.
2. Make sure the result has a transparent background and exactly the size given
   in `prompt.txt`. Most image editors can resize and remove a background.
3. Name the file as `prompt.txt` says (for example `lattice lance.png`).
4. On GitHub, open the folder named in `prompt.txt` (for example
   `images/outfit/expanded`), choose "Add file", then "Upload files", drop the
   image in, and commit it to the development branch. It replaces the old one.
5. The next playtest build will use it. Nothing else needs to change.

If a ship comes out with a different shape or size, upload it anyway and ask
Claude to move the engines and guns to match.

## For whoever maintains the mod

The list of images and prompts is in `manifest.json`, in this folder. The pack is
built by `utils/expanded/make_art_pack.py` and published by the playtest workflow.
When adding a new outfit or ship, copy a stock image to the matching `expanded`
folder under `images/`, point the data file at it, and add an entry to the
manifest.
