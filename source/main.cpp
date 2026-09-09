#include <3ds.h>
#include <citro2d.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#define MAX_IMAGES 32
#define MAX_CHARACTERS 32

#define MAX_NAME_LENGTH 32
#define MAX_DIALOGUE_LENGTH 512

// image variable

struct ImageVariable
{
    char name[MAX_NAME_LENGTH];

    C2D_SpriteSheet sheet;
    C2D_Image image;
};

ImageVariable images[MAX_IMAGES];
int imageCount = 0;


// character variable

struct CharacterVariable
{
    char variable[MAX_NAME_LENGTH];
    char name[MAX_NAME_LENGTH];

    u32 color;
};

CharacterVariable characters[MAX_CHARACTERS];
int characterCount = 0;


// current screen content

C2D_Image sceneImage;
C2D_Image foregroundImage; // add multiple foreground images for handling multiple characters on screen? do later!
C2D_Image bottomImage;

bool hasScene = false;
bool hasForeground = false;
bool hasBottom = false;


// dialogue

C2D_Font dialogueFont;
C2D_TextBuf textBuffer;

C2D_Text nameText;
C2D_Text dialogueText;

bool hasDialogue = false;
bool waitingForInput = false;

FILE* scriptFile = NULL; // script

u32 currentNameColor = C2D_Color32(255, 255, 255, 255); // current character name color


// image locator

ImageVariable* findImage(const char* name)
{
    for (int i = 0; i < imageCount; i++)
    {
        if (strcmp(images[i].name, name) == 0)
            return &images[i];
    }

    return NULL;
}


// character locator

CharacterVariable* findCharacter(const char* variable)
{
    for (int i = 0; i < characterCount; i++)
    {
        if (strcmp(characters[i].variable, variable) == 0)
            return &characters[i];
    }

    return NULL;
}


// image loader

bool loadImage(const char* filename, const char* name)
{
    if (imageCount >= MAX_IMAGES)
        return false;

    ImageVariable* variable = &images[imageCount];

    strcpy(variable->name, name);

    char path[256];

    snprintf(
        path,
        sizeof(path),
        "romfs:/%s",
        filename
    );

    variable->sheet =
        C2D_SpriteSheetLoad(path);

    if (!variable->sheet)
        return false;

    variable->image =
        C2D_SpriteSheetGetImage(
            variable->sheet,
            0
        );

    imageCount++;

    return true;
}


// parse hex
u32 parseColor(const char* color)
{
    unsigned int value = 0;

    if (color[0] == '#')
        sscanf(color + 1, "%x", &value);
    else
        sscanf(color, "%x", &value);

    unsigned char r = (value >> 16) & 0xFF;
    unsigned char g = (value >> 8) & 0xFF;
    unsigned char b = value & 0xFF;

    return C2D_Color32(r, g, b, 255);
}


// character definition handler

void defineCharacter(
    const char* variable,
    const char* name,
    const char* color
)
{
    if (characterCount >= MAX_CHARACTERS)
        return;

    CharacterVariable* character =
        &characters[characterCount];

    strcpy(character->variable, variable);
    strcpy(character->name, name);

    character->color =
        parseColor(color);

    characterCount++;
}


// dialogue display handler

void displayDialogue(
    const char* characterVariable,
    const char* dialogue
)
{
    const char* characterName = "";
    
    currentNameColor =
        C2D_Color32(
            255,
            255,
            255,
            255
        );

    // character specified handler

    if (characterVariable != NULL)
    {
        CharacterVariable* character =
            findCharacter(characterVariable);

        if (character)
        {
            characterName = character->name;
            currentNameColor = character->color;
        }
    }


    // name handler

    C2D_TextBufClear(textBuffer);

    C2D_TextFontParse(
        &nameText,
        dialogueFont,
        textBuffer,
        characterName
    );

    C2D_TextOptimize(&nameText);


    // dialogue handler

    C2D_TextFontParse(
        &dialogueText,
        dialogueFont,
        textBuffer,
        dialogue
    );

    C2D_TextOptimize(&dialogueText);

    hasDialogue = true;
    waitingForInput = true;
}


// iterator

void executeLine(char* line)
{
    line[strcspn(line, "\r\n")] = '\0'; // Remove newline

    if (line[0] == '\0') // Ignore empty lines
        return;


    char command[32];
    char argument1[128];
    char argument2[128];
    char argument3[128];


    int count = sscanf(
        line,
        "%31s %127s %127s %127s",
        command,
        argument1,
        argument2,
        argument3
    );


    if (count <= 0)
        return;

    if (strcmp(command, "image") == 0) // Image command
    {
        if (count == 3)
        {
            loadImage(
                argument1,
                argument2
            );
        }
    }


    else if (strcmp(command, "character") == 0) // Character command
    {
        if (count == 4)
        {
            // Remove quotation marks from name

            char characterName[MAX_NAME_LENGTH];

            strncpy(
                characterName,
                argument2,
                sizeof(characterName)
            );

            characterName[
                sizeof(characterName) - 1
            ] = '\0';


            size_t length =
                strlen(characterName);

            if (
                length >= 2 &&
                characterName[0] == '"' &&
                characterName[length - 1] == '"'
            )
            {
                characterName[length - 1] = '\0';

                defineCharacter(
                    argument1,
                    characterName + 1,
                    argument3
                );
            }
        }
    }


    // scene command

    else if (strcmp(command, "scene") == 0)
    {
        if (count != 2)
            return;

        ImageVariable* variable =
            findImage(argument1);

        if (variable)
        {
            sceneImage =
                variable->image;

            hasScene = true;
        }
    }


    // show command

    else if (strcmp(command, "show") == 0)
    {
        if (count != 2)
            return;

        ImageVariable* variable =
            findImage(argument1);

        if (variable)
        {
            foregroundImage =
                variable->image;

            hasForeground = true;
        }
    }


    // bottom command

    else if (strcmp(command, "bottom") == 0)
    {
        if (count != 2)
            return;

        ImageVariable* variable =
            findImage(argument1);

        if (variable)
        {
            bottomImage =
                variable->image;

            hasBottom = true;
        }
    }

    else if (strcmp(command, "bottom") == 0)
    {
        if (count != 2)
            return;

        ImageVariable* variable =
            findImage(argument1);

        if (variable)
        {
            bottomImage =
                variable->image;

            hasBottom = true;
        }
    }


    // dialogue command

    else if (strcmp(command, "dia") == 0)
    {
        const char* characterVariable = NULL;
        const char* dialogue = NULL;


        // Character specified handler

        if (count >= 2 && argument1[0] == '/')
        {
            characterVariable =
                argument1 + 1;

            // Find the actual dialogue in the original line
            char* textStart =
                strstr(line, argument1);

            if (textStart)
            {
                textStart += strlen(argument1);

                while (*textStart == ' ')
                    textStart++;

                dialogue = textStart;
            }
        }


        // character unspecified handler

        else
        {
            char* textStart =
                strstr(line, command);

            if (textStart)
            {
                textStart += strlen(command);

                while (*textStart == ' ')
                    textStart++;

                dialogue = textStart;
            }
        }


        if (dialogue)
        {
            displayDialogue(
                characterVariable,
                dialogue
            );
        }
    }
}


// reading until dialogue

void continueScript()
{
    if (!scriptFile)
        return;

    char line[512];

    while (fgets(line, sizeof(line), scriptFile))
    {
        executeLine(line);

        // Dialogue pauses execution, file position remains exactly where fgets() left it, the next call to continueScript() continues with next line.

        if (waitingForInput)
            return;
    }
}


// main

int main()
{
    gfxInitDefault();
    romfsInit();

    C3D_Init(
        C3D_DEFAULT_CMDBUF_SIZE
    );

    C2D_Init(
        C2D_DEFAULT_MAX_OBJECTS
    );

    C2D_Prepare();


    // screen handlers

    C3D_RenderTarget* top =
        C2D_CreateScreenTarget(
            GFX_TOP,
            GFX_LEFT
        );

    C3D_RenderTarget* bottom =
        C2D_CreateScreenTarget(
            GFX_BOTTOM,
            GFX_LEFT
        );


    // font handler

    dialogueFont =
        C2D_FontLoad(
            "romfs:/RobotoSlab-Medium.bcfnt"
        );

    textBuffer =
        C2D_TextBufNew(4096);


    // script accessor

    scriptFile =
        fopen(
            "romfs:/game.rcp",
            "r"
        );

    if (scriptFile)
    {
        continueScript();
    }


    // main loop

    while (aptMainLoop())
    {
        hidScanInput();

        u32 kDown =
            hidKeysDown();


        // continue after dialogue

        if (waitingForInput)
        {
            if (kDown & (KEY_A | KEY_B))
            {
                hasDialogue = false;
                waitingForInput = false;

                continueScript();
            }
        }


        // START quits

        if (kDown & KEY_START)
            break;

        // Begin frame

        C3D_FrameBegin(
            C3D_FRAME_SYNCDRAW
        );


        // top

        C2D_TargetClear(
            top,
            C2D_Color32(
                0,
                0,
                0,
                255
            )
        );

        C2D_SceneBegin(top);


        if (hasScene)
        {
            C2D_DrawImageAt(
                sceneImage,
                0.0f,
                0.0f,
                0.0f
            );
        }


        if (hasForeground)
        {
            C2D_DrawImageAt(
                foregroundImage,
                0.0f,
                0.0f,
                0.0f
            );
        }


        // dialogue name handler

        if (hasDialogue)
        {
            C2D_DrawText(
                &nameText,
                C2D_WithColor,
                10.0f,
                165.0f,
                0.0f,
                1.0f,
                1.0f,
                currentNameColor
            );


            // dialogue text handler

            C2D_DrawText(
                &dialogueText,
                C2D_WithColor,
                10.0f,
                190.0f,
                0.0f,
                1.0f,
                1.0f,
                C2D_Color32(
                    255,
                    255,
                    255,
                    255
                )
            );
        }


        // bottom screen

        C2D_TargetClear(
            bottom,
            C2D_Color32(
                0,
                0,
                0,
                255
            )
        );

        C2D_SceneBegin(bottom);


        if (hasBottom)
        {
            C2D_DrawImageAt(
                bottomImage,
                0.0f,
                0.0f,
                0.0f
            );
        }


        C3D_FrameEnd(0);
    }


    // cleanup

    if (scriptFile)
        fclose(scriptFile);


    for (int i = 0; i < imageCount; i++)
    {
        C2D_SpriteSheetFree(
            images[i].sheet
        );
    }


    C2D_TextBufDelete(
        textBuffer
    );

    C2D_FontFree(
        dialogueFont
    );


    C2D_Fini();
    C3D_Fini();

    romfsExit();
    gfxExit();

    return 0;
}

// To add next:
// hide (hides image)
// forget (unloads image from memory (might not be necessary))
// transform (move, scale, rotate definitions for `show` command attribute)
//
//