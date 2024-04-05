#include <assert.h> // Include the "assert feature"

#include "TextureHolder.h"

TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder()
{
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

Texture& TextureHolder::GetTexture(string const& filename)
{
	//Get reference to m_Texture using m_s_Instance
	auto& m = m_s_Instance->m_Textures; // auto == map<string, Texture>

	//Create an iterator to hold a key-vaule-pair (kvp)
	//and search for the required kvp
	//using the passed in file name
	auto keyVaulePair = m.find(filename); // auto == map<string, Texture)::iterator

	//Did we find match?
	if (keyVaulePair != m.end())
	{
		//If yes, return the texture, the second part of the kvp
		return keyVaulePair->second;
	}
	else
	{
		//File name not found
		//Create a new kvp using filename
		auto& texture = m[filename];
		//Load the texture from the file in the usual way
		texture.loadFromFile(filename);
		//Return the texture to the calling code
		return texture;
	}
}