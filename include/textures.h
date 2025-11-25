#ifndef TEXTURES_H
#define TEXTURES_H

#include "raylib.h"

typedef struct {

    // Fixos
    Texture2D pedra;
    Texture2D coral;
    Texture2D concha;

    // Alga animada
    Texture2D algaCentro;
    Texture2D algaEsq;
    Texture2D algaDir;

    // Móveis
    Texture2D carangueijoParado;
    Texture2D carangueijoAnim;

    Texture2D aguaVivaCentro;
    Texture2D aguaVivaEsq;
    Texture2D aguaVivaDir;

    Texture2D baleiaParada;
    Texture2D baleiaAnimada;

    Texture2D tubaCentro;
    Texture2D tubaEsq;
    Texture2D tubaDir;

} ObstacleTextures;

#endif
