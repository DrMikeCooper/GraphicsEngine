#pragma once

class Texture
{
public:
	Texture();
	Texture(const char* name)
	{
		m_id = LoadTexture(name);
	}

	static unsigned int LoadTexture(const char* name);

	unsigned int GetID() { return m_id; }

private:
	unsigned int m_id;
};
