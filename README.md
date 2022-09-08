# Columns

Columns is a game clone of the original Columns game.  
Columns uses DGEngine core - a 2D game engine.

![columns](https://user-images.githubusercontent.com/20025614/103457653-fcb12400-4cf8-11eb-8552-1ab5f46a8450.png)

Sprites for the original are included in the gamefilesc/gfx*x folders.  

Included upscaled graphics created using [xbrz](https://sourceforge.net/projects/xbrz/).  

To have music, convert the VGM songs to ogg and place them in gamefilesc/bgm  
To get the original music, see gamefilesc/bgm/readme.txt  

[Getting original assets](#Getting-original-assets)

## keys

Action            | Key
----------------- | ----------------------------
`Player 1 Move`   | `Up`, `Down`, `Left`, `Right`
`Player 1 Rotate` | `Space`, `Right Shift`
`Player 1 Select` | `Enter`
`Player 2 Move`   | `W`, `S`, `A`, `D`
`Player 2 Rotate` | `H`, `Left Shift`
`Player 2 Select` | `G`
`Back/Cancel`     | `Esc`
`Fullscreen`      | `Alt + Enter`, `F11`
`Settings`        | `F1`

## Configuration

After pressing start button, press `F1` to change settings, apply shaders, etc..  

Unfifished games are automatically saved on exit.  

Current window position and size are automatically saved.

### Portable mode

To run in portable mode, create a folder `portable` next to the DGEngine binary (current working directory).  

On Linux, if using an `AppImage`, create a folder with the same name as the AppImage ending with `.home`.

```
Columns.AppImage         (file)
Columns.AppImage.home    (folder)
```

## Build

See how to build in [BUILD.txt](BUILD.txt).

### Getting original assets

Fork this repository, go to `Actions` and manually run the `Package_gamefiles` workflow.  
An artifact (`gamefilesc.zip`) will be created with the original assets.  
Extract that archive over the `gamefilesc` folder or delete the `gamefilesc` folder (Columns will use the zip instead).
