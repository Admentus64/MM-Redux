#include <z64.h>
#include "Sprite.h"
#include "Util.h"

extern uint8_t CFG_WS_ENABLED;

extern char DPAD_TEXTURE;
extern char PARAMETER_COUNTER_TEXTURE;
extern char PARAMETER_AMMO_DIGIT_TEXTURE;
extern char PARAMETER_CLOCK_TEXTURE;
extern char PARAMETER_NOTE_BUTTONS_TEXTURE;
extern char PARAMETER_SUN_MOON_TEXTURE;
extern char HUD_TOGGLE_TEXTURE;
extern char DUNGEON_MAP_LINK_HEAD_TEXTURE;

#define DpadTextureRaw          ((u8*)&DPAD_TEXTURE)
#define ParameterCounterRaw     ((u8*)&PARAMETER_COUNTER_TEXTURE)
#define ParameterAmmoDigitRaw   ((u8*)&PARAMETER_AMMO_DIGIT_TEXTURE)
#define ParameterClockRaw       ((u8*)&PARAMETER_CLOCK_TEXTURE)
#define ParameterNoteButtonsRaw ((u8*)&PARAMETER_NOTE_BUTTONS_TEXTURE)
#define ParameterSunMoonRaw     ((u8*)&PARAMETER_SUN_MOON_TEXTURE)
#define HudToggleRaw            ((u8*)&HUD_TOGGLE_TEXTURE)
#define DungeonMapLinkHeadRaw   ((u8*)&DUNGEON_MAP_LINK_HEAD_TEXTURE)

extern u8 FONT_TEXTURE[];

Gfx gSetupDb[] = {
    gsDPPipeSync(),
    gsSPLoadGeometryMode(0),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_DISABLE |
        G_CK_NONE | G_TC_FILT |
        G_TD_CLAMP | G_TP_NONE |
        G_TL_TILE | G_TT_NONE |
        G_PM_NPRIMITIVE | G_CYC_1CYCLE |
        G_TF_BILERP, // HI
        G_AC_NONE | G_ZS_PRIM |
        G_RM_XLU_SURF | G_RM_XLU_SURF2), // LO
    gsSPEndDisplayList(),
};

Sprite gSpriteDpad = {
    NULL, 32, 32, 1,
    G_IM_FMT_RGBA, G_IM_SIZ_32b, 4
};

Sprite gSpriteFont = {
    NULL, 8, 14, 95,
    G_IM_FMT_IA, G_IM_SIZ_8b, 1
};

Sprite gSpriteIcon = {
    NULL, 32, 32, 97,
    G_IM_FMT_RGBA, G_IM_SIZ_32b, 4
};

Sprite gSpriteIcon24 = {
    NULL, 24, 24, 12,
    G_IM_FMT_RGBA, G_IM_SIZ_32b, 4
};

// Sprite containing 5 item textures.
// Depending on the game state, this is used for either the file select hash icons, or the d-pad icons.
static Sprite gItemTexturesSprite = {
    NULL, 32, 32, 5,
    G_IM_FMT_RGBA, G_IM_SIZ_32b, 4
};

Sprite gParameterCounter = {
    NULL, 8, 16, 12,
    G_IM_FMT_I, G_IM_SIZ_8b, 1
};

Sprite gParameterAmmoDigit = {
    NULL, 8, 8, 12,
    G_IM_FMT_IA, G_IM_SIZ_8b, 1
};

Sprite gParameterClock = {
    NULL, 16, 16, 1,
    G_IM_FMT_IA, G_IM_SIZ_8b, 1
};

Sprite gParameterNoteButtons = {
    NULL, 16, 16, 5,
    G_IM_FMT_IA, G_IM_SIZ_8b, 1
};

Sprite gParameterSunMoon = {
    NULL, 24, 24, 2,
    G_IM_FMT_IA, G_IM_SIZ_8b, 1
};

Sprite gHudToggle = {
    NULL, 16, 16, 1,
    G_IM_FMT_IA, G_IM_SIZ_4b, 1
};

Sprite gDungeonMapLinkHead = {
    NULL, 16, 16, 1,
    G_IM_FMT_RGBA, G_IM_SIZ_16b, 1
};

int Sprite_GetBytesPerTile(Sprite* sprite) {
    return sprite->tileW * sprite->tileH * sprite->bytesPerTexel;
}

int Sprite_GetBytesTotal(Sprite* sprite) {
    return sprite->tileCount * Sprite_GetBytesPerTile(sprite);
}

void Sprite_Load(DispBuf* db, Sprite* sprite, int startTile, int tileCount) {
    int width = sprite->tileW;
    int height = sprite->tileH * tileCount;
    gDPLoadTextureTile(db->p++,
            sprite->buf + (startTile * Sprite_GetBytesPerTile(sprite)),
            sprite->imFmt, sprite->imSiz,
            width, height,
            0, 0,
            width - 1, height - 1,
            0,
            G_TX_WRAP, G_TX_WRAP,
            G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD, G_TX_NOLOD);
}

void Sprite_Draw(DispBuf* db, Sprite* sprite, int tileIndex, int left, int top, int width, int height) {
    Sprite_DrawCropped(db, sprite, tileIndex, left, top, width, height, CROP(0, 0));
}

void Sprite_DrawCropped(DispBuf* db, Sprite* sprite, int tileIndex, int left, int top, int width, int height, Crop crop) {
    int widthFactor = (1<<10) * sprite->tileW / width;
    int heightFactor = (1<<10) * (sprite->tileH - (crop.top + crop.bottom)) / height;
    gSPTextureRectangle(db->p++,
            left<<2, top<<2,
            (left + width)<<2, (top + height)<<2,
            0,
            0, ((tileIndex * sprite->tileH) + crop.top)<<5,
            widthFactor, heightFactor);
}

Sprite* Sprite_GetItemTexturesSprite(void) {
    return &gItemTexturesSprite;
}

void Sprite_Init(void) {
    if (CFG_WS_ENABLED)
        gSetupDb[2] = gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH + 104, SCREEN_HEIGHT);
    
    gSpriteDpad.buf           = DpadTextureRaw;
    gParameterCounter.buf     = ParameterCounterRaw;
    gParameterAmmoDigit.buf   = ParameterAmmoDigitRaw;
    gParameterClock.buf       = ParameterClockRaw;
    gParameterNoteButtons.buf = ParameterNoteButtonsRaw;
    gParameterSunMoon.buf     = ParameterSunMoonRaw;
    gHudToggle.buf            = HudToggleRaw;
    gDungeonMapLinkHead.buf   = DungeonMapLinkHeadRaw;

    // Allocate space for item textures
    int size = Sprite_GetBytesTotal(&gItemTexturesSprite);
    gItemTexturesSprite.buf = Util_HeapAlloc(size);
    
    /*UtilFile gParameterCounterStatic = {
        NULL, z2ParameterCounterStaticVaddr, z2ParameterCounterStaticVsize
    };
    Util_FileInit(&gParameterCounterStatic);
    gParameterCounter.buf = gParameterCounterStatic.buf;
    
    UtilFile gParameterAmmoDigitStatic = {
        NULL, z2ParameterAmmoDigitStaticVaddr, z2ParameterAmmoDigitStaticVsize
    };
    Util_FileInit(&gParameterAmmoDigitStatic);
    gParameterAmmoDigit.buf = gParameterAmmoDigitStatic.buf;
    
    UtilFile gParameterClockStatic = {
        NULL, z2ParameterClockStaticVaddr, z2ParameterClockStaticVsize
    };
    Util_FileInit(&gParameterClockStatic);
    gParameterClock.buf = gParameterClockStatic.buf;
    
    UtilFile gParameterNoteButtonsStatic = {
        NULL, z2ParameterNoteButtonsStaticVaddr, z2ParameterNoteButtonsStaticVsize
    };
    Util_FileInit(&gParameterNoteButtonsStatic);
    gParameterNoteButtons.buf = gParameterNoteButtonsStatic.buf;
    
    UtilFile gParameterSunMoonStatic = {
        NULL, z2ParameterSunMoonStaticVaddr, z2ParameterSunMoonStaticVsize
    };
    Util_FileInit(&gParameterSunMoonStatic);
    gParameterSunMoon.buf = gParameterSunMoonStatic.buf;
    
    UtilFile gHudToggleStatic = {
        NULL, z2HudToggleStaticVaddr, z2HudToggleStaticVsize
    };
    Util_FileInit(&gHudToggleStatic);
    gHudToggle.buf = gHudToggleStatic.buf;*/

    // Initialize font texture buffer.
    int fontBytes = Sprite_GetBytesTotal(&gSpriteFont);
    gSpriteFont.buf = Util_HeapAlloc(fontBytes);
    for (int i = 0; i < fontBytes / 2; i++) {
        gSpriteFont.buf[2*i] = (FONT_TEXTURE[i] >> 4) | 0xF0;
        gSpriteFont.buf[2*i + 1] = FONT_TEXTURE[i] | 0xF0;
    }
}
