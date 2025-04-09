#include "global_context.h"
#include "raylib.h"

// Global context
GlobalContext g_context;

Texture2D GetRandomTexture(TextureId startId, TextureId endId)
{
    // Ensure the range is valid
    if (startId >= endId || startId < TEXTURE_NONE || endId >= TEXTURE_MAX) {
        return g_context.textures[TEXTURE_NONE]; // Return default texture if range is invalid
    }

    // Calculate the range of valid indices
    int range = endId - startId + 1;
    
    // Generate a random index within the range
    int randomIndex = startId + (GetRandomValue(0, 1920) % range);
    
    // Return the randomly selected texture
    return g_context.textures[randomIndex];
}