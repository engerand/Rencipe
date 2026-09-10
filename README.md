
# Ren'cipe

A parser made in C++ that reads and executes `.RCP` files, which are populated by a custom scripting language. Currently a work-in-progress project.
## Requirements:

To compile this, you need DevkitPro for the 3DS


## Currently working functions:
- `image` defines an image location, with `romfs` as root
```rcp
image [location] [variable name]
```
- `character` defines character name and text colour
```rcp
character [varriable name] "[display name]" [hex value colour]
```
- `scene` loads an image into the background layer
```rcp
scene [image variable name]
```
- `bottom` loads an image into the background layer of the bottom screen
```rcp
bottom [image variable name]
```
- `show` loads an image into the foreground layer
```rcp
show [image variable name]
```
- `dia` displays dialogue on the screen
```rcp
dia [text]
```
*for no speaker name, for named speaker:*

```rcp
dia /[character varriable name] [text]
```
