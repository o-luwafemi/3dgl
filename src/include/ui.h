#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"

class UIRenderer
{
public:
	UIRenderer(int screenWidth, int screenHeight)
        : m_Shader("../shader/ui.vs", "../shader/ui.fs")
	{
		Resize(screenWidth, screenHeight);
		InitQuad();
		InitTriangle();
	}

	void Resize(int screenWidth, int screenHeight)
	{
		// y-flipped ortho so (0,0) is top-left, matching GLFW cursor coordinates
		m_Projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
	}

	void BeginFrame()
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_Shader.use();
		m_Shader.setMat4("uProjection", m_Projection);
	}

	void EndFrame()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void DrawQuad(float x, float y, float w, float h, glm::vec4 color)
	{
		m_Shader.setVec2("uPos", glm::vec2(x, y));
		m_Shader.setVec2("uSize", glm::vec2(w, h));
		m_Shader.setVec4("uColor", color);
		glBindVertexArray(m_QuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// Right-pointing triangle, used as the "Play" icon
	void DrawTriangle(float x, float y, float w, float h, glm::vec4 color)
	{
		m_Shader.setVec2("uPos", glm::vec2(x, y));
		m_Shader.setVec2("uSize", glm::vec2(w, h));
		m_Shader.setVec4("uColor", color);
		glBindVertexArray(m_TriVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	
	void InitQuad()
	{
		float verts[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};
		glGenVertexArrays(1, &m_QuadVAO);
		glGenBuffers(1, &m_QuadVBO);
		glBindVertexArray(m_QuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	}

	void InitTriangle()
	{
		float verts[] = {
			0.0f, 0.0f,
			1.0f, 0.5f,
			0.0f, 1.0f
		};
		glGenVertexArrays(1, &m_TriVAO);
		glGenBuffers(1, &m_TriVBO);
		glBindVertexArray(m_TriVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_TriVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	}

    Shader m_Shader;
	unsigned int m_QuadVAO, m_QuadVBO;
	unsigned int m_TriVAO, m_TriVBO;
	glm::mat4 m_Projection;
};