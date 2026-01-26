#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
	ResourceManager(SDL_Renderer* renderer, MIX_Mixer* mixer) 
        :m_renderer(renderer), m_mixer(mixer) {};
	~ResourceManager() { cleanup(); }

    SDL_Texture* getTexture(const std::string& path) {
        auto it = m_textures.find(path);
        if (it != m_textures.end()) {
            return it->second;
        }

        SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());
        if (!texture) {
            return nullptr;
        }

        m_textures[path] = texture;
        return texture;
    }

    MIX_Audio* getAudio(const std::string& path) 
    {
        auto it = m_audios.find(path);
        if (it != m_audios.end()) {
            return it->second;
        }

        MIX_Audio* audio = MIX_LoadAudio(m_mixer, path.c_str(), false);
        if (!audio) {
            return nullptr;
        }

        m_audios[path] = audio;
        return audio;

    }

    void cleanup() {
        for (auto& pair : m_textures) {
            SDL_DestroyTexture(pair.second);
        }
        m_textures.clear();

        for (auto& pair : m_audios) MIX_DestroyAudio(pair.second);
        m_audios.clear();
    }

private:
	SDL_Renderer* m_renderer;
    MIX_Mixer* m_mixer;
	std::unordered_map<std::string, SDL_Texture*> m_textures;
    std::unordered_map<std::string, MIX_Audio*> m_audios;
};

