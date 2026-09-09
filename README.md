
# Ren'cipe

A parser made in C++ that reads a `.RCP` file, populated by a custom scripting language. Currently a work in project.
## requirements:

In order to compile this, you need Devkitpro for the 3DS


## Currently working functions:
- `image` defines an image location, with `romfs` as root
```rcp
image [location] [varriable name]
```
- `character` defines character name and text colour
```rcp
character [varriable name] "[display name]" [hex value colour]
```
- `scene` loads an image image into the background layer
```rcp
scene [image varriable name]
```
- `bottom` loads an image into the background layer of the bottom screen
```rcp
bottom [image varriable name]
```
- `show` loads an image into the foreground layer
```rcp
show [image varriable name]
```
- `dia` displays dialogue on the screen
```rcp
dia [text]
```
*for no speaker name, for named speaker:*

```rcp
dia /[character varriable name] [text]
```